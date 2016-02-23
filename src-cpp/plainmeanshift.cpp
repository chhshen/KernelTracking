/*
 *   author           Chunhua Shen {http://users.rsise.anu.edu.au/~cs/}
 *   file             PlainMeanshift.cxx
 *   created          01-Sep-2006 16:55.
 *   last revision    27-Nov-2006 18:50.
 */

#include "plainmeanshift.h"

// inverse of 256
const double inv_UCHAR_MAX_PIX_VAL = 1.0/(double)UCHAR_MAX_PIX_VAL;

PlainMeanshift::PlainMeanshift(void)
{
    ref_hist = new Hist;
    ref_hist->set_size( N_BINS );
	p_temp = new Hist;
	p_temp->set_size( N_BINS );
   
    num_bin_b = N_BINS_PER_CH;
    num_bin_g = N_BINS_PER_CH;
    num_bin_r = N_BINS_PER_CH;

    // set the kernel type: Epan, Truncated Gaussian or Gaussian?
    // This must be set before kernel histogram calculation
    // In most of the cases, just use Truncated Gaussian.
    // Anyway you can change the default setting
    // to other two kernels in init()
    Tracking_Kernel = kernel_TrunGaussian;

    bin_index_cache = NULL;
    use_bin_index_cache = FALSE;

}

PlainMeanshift::~PlainMeanshift(void)
{   
    if ( ref_hist )         delete ref_hist;
	if (p_temp) delete p_temp;
    if ( bin_index_cache )  delete bin_index_cache;
}

/** 
 * @brief initialisation
 * @param img input image containing the target template
 * @param subregion target template
 */
void PlainMeanshift::init(Rect* subregion, CvImage * cur_frame )
{
    // set the bandwidth
    set_bandwidth( 0.5 * subregion->width, 0.5 * subregion->height);

    // reference histogram
    kernel_hist( subregion, cur_frame, ref_hist);

#ifdef _DEBUG
    ref_hist->save("_hist.dat");
#endif

	m_iWidth = cur_frame->width();
	m_iHeight = cur_frame->height();

    // init bin_index_cache
	bin_index_cache = new int[ m_iHeight * m_iWidth ];
	memset ( bin_index_cache, -1, sizeof (int) * m_iHeight * m_iWidth );
}

/** 
 * @brief initialisation
 * @param img input image containing the target template
 * @param subregion target template 
 * @param kernel_type_ what type of kernel for tracking.
 * 0: kernel_Epan, 1: kernel_TrunGaussian, and 2:kernel_Gaussian
 */
void PlainMeanshift::init(Rect * subregion, CvImage * img, int kernel_type_ )
{
    // set the kernel type
    Tracking_Kernel = kernel_type_;
	init(subregion, img);
}


/** 
 * @brief 
 * @param id 
 * @return returns 
 */
void PlainMeanshift::get_bandwidth( Bandwidth & b)
{      
    b = bandwidth;   
}

/// note that hx and hy are half of the rectangle's sides
void PlainMeanshift::set_bandwidth(double _hx, double _hy)
{
    bandwidth.hx = _hx;
    bandwidth.hy = _hy;
}

/**
 * find the bin index, given pixel values.
 * @return an array of length 4. 
 */
unsigned int PlainMeanshift::get_bin_index (double b, double g, double r)
{
#ifdef _DEBUG
    // make sure the pixel value is normalised
    int cond1 = (b >= 0 && g >= 0 && r >= 0 && b < 1 && g < 1 && r < 1);

    if (!cond1)
    {
        fprintf (stderr, "- Error: b, g, r must be in [0,1]\n");
        exit (-1);
    }
#endif

    unsigned int index;
    unsigned int bd, gd, rd;

    bd =  (int)(b * num_bin_b);
    gd =  (int)(g * num_bin_g);
    rd =  (int)(r * num_bin_r);

    index = bd * num_bin_g * num_bin_r + gd * num_bin_r + rd;

    // make sure the bin index correct
    assert (index >= 0 && index <= N_BINS - 1);

    return index;
}


/** 
 * @brief Build a kernel weighted histogram. 
 * @param subregion the sub-region of interest.
 * @param img the input image.
 * @param kh the returned kernel histogram.
 */
bool PlainMeanshift::kernel_hist(Rect * subregion, CvImage * cur_frame, Hist * kh)
{
	double valB;
    double valG;
    double valR;   
    unsigned int bin;
    double _xx;

    assert( kh -> length == N_BINS );

    double cenx = subregion->cen_x;
    double ceny = subregion->cen_y;

	int img_width = cur_frame->width();
    int img_height = cur_frame->height();

    // inverse of 256
    double inv_UCHAR_MAX_PIX_VAL = 1.0/(double)UCHAR_MAX_PIX_VAL;

    for ( int i = subregion->y0; i < subregion->y1; i++)
        for ( int j = subregion->x0; j < subregion->x1; j++)
        {

            /// 
            /// if the pixel's location is outside of the image, ignore it
            ///
            if ( i < 0 || j < 0 || i > img_height - 1 || j > img_width - 1 )
                continue;

            // Kernel_hist is called to calculate the reference histogram
            // or for some other reasons. Do not utilise bin_index_cache  
            if ( ! use_bin_index_cache ) 
            {
                // normalise the pix value to [0,1) by dividing 256
				uchar* img_temp = (uchar*)cur_frame->data();
				valB = img_temp[i*cur_frame->step() + j*3] * inv_UCHAR_MAX_PIX_VAL;
				valG = img_temp[i*cur_frame->step() + j*3 + 1] * inv_UCHAR_MAX_PIX_VAL;
				valR = img_temp[i*cur_frame->step() + j*3 + 2] * inv_UCHAR_MAX_PIX_VAL;
                bin = get_bin_index ( valB, valG, valR );
            }
            else
            {
                int pix_pos = j + i * img_width;
                if ( bin_index_cache[ pix_pos ] >=0 )       
                    // bin_index_cache already calculated
                    // Do not calculate it again
                {
                    bin = bin_index_cache[ pix_pos ];
                }
                else             
                    // this pixel's histogram index is not calculated yet
                    // calculate it and store it in the cache
                {
                    // normalise the pix value to [0,1) by dividing 256
					uchar* img_temp = (uchar*)cur_frame->data();
					valB = img_temp[i*cur_frame->step() + j*3] * inv_UCHAR_MAX_PIX_VAL;
					valG = img_temp[i*cur_frame->step() + j*3 + 1] * inv_UCHAR_MAX_PIX_VAL;
					valR = img_temp[i*cur_frame->step() + j*3 + 2] * inv_UCHAR_MAX_PIX_VAL;
                    bin = get_bin_index ( valB, valG, valR );
                    bin_index_cache[ pix_pos ] = bin;
                }
            }

            assert( bin>=0 && bin < kh->length );
            _xx = SQR(( (double)j - cenx ) / bandwidth.hx ) + SQR(( (double)i - ceny ) /bandwidth.hy);
			if ((_xx > 2.0) || (_xx < 0))
				fprintf(stdout, "_xx = %lf\n");
//            assert(_xx<=2.0 && _xx>=0);
            kh -> data[bin] += (float)kernel_profile( _xx, Tracking_Kernel );
        }

    // normalise the histogram
    return kh->normalise();
}

/**
  Equation (10,11) in the TPAMI paper "kenerl-base object tracking",
  @param subregion the sub-region of interest, 
  i.e. its location in the previous frame.
 */
Rect PlainMeanshift::calc_estimate( Rect * subregion, CvImage * cur_frame )
{
   double valB;
    double valG;
    double valR;

    assert(ref_hist->is_nonnegative());
    assert(ref_hist->is_normalised());

    // calculate the candidate's kernel weighted histogram p
    Hist * p = new Hist;
    p->set_size( N_BINS );

    // NOTE set the bandwidth to the candidate's size
    // This is critically important
    set_bandwidth( 0.5 * subregion->width, 0.5* subregion->height);

    // calculate the candidate region's histogram
    kernel_hist(subregion, cur_frame, p);

#ifdef _DEBUG
    p->save("_hist1.dat");
#endif

    assert(p->is_nonnegative());
    assert(p->is_normalised());

    unsigned int bin;
    double _weight;

    double _sumX = 0.0;
    double _sumY = 0.0;
    double _sum  = 0.0;

    double _profd;
    double _xx;

    double cenx = subregion->cen_x;
    double ceny = subregion->cen_y;

    // image size
    int img_width = cur_frame->width();
    int img_height = cur_frame->height();

    for (int i = subregion->y0; i < subregion->y1; i++)
        for (int j = subregion->x0; j < subregion->x1; j++)
        {
            ///
            /// NOTE if the pixel location is outside of the image, ignore it
            ///
            if ( i < 0 || j < 0 || i > img_height - 1 || j > img_width - 1 )
                continue;

            if (! use_bin_index_cache)
            {
				uchar* img_temp = (uchar*)cur_frame->data();
				valB = img_temp[i*cur_frame->step() + j*3] * inv_UCHAR_MAX_PIX_VAL;
				valG = img_temp[i*cur_frame->step() + j*3 + 1] * inv_UCHAR_MAX_PIX_VAL;
				valR = img_temp[i*cur_frame->step() + j*3 + 2] * inv_UCHAR_MAX_PIX_VAL;

                bin = get_bin_index ( valB, valG, valR );
            }
            else
            {
                int pix_pos = j + i * img_width;
                if ( bin_index_cache[ pix_pos ] >=0 )       
                    // bin_index_cache already calculated
                    // Do not calculate it again
                {
                    bin = bin_index_cache[ pix_pos ];
                }
                else             
                    // this pixel's histogram index is not calculated yet
                    // calculate it and store it in the cache
                {
                    // normalise the pix value to [0,1) by dividing 256
					uchar* img_temp = (uchar*)cur_frame->data();
					valB = img_temp[i*cur_frame->step() + j*3] * inv_UCHAR_MAX_PIX_VAL;
					valG = img_temp[i*cur_frame->step() + j*3 + 1] * inv_UCHAR_MAX_PIX_VAL;
					valR = img_temp[i*cur_frame->step() + j*3 + 2] * inv_UCHAR_MAX_PIX_VAL;

                    bin = get_bin_index ( valB, valG, valR );
                    bin_index_cache[ pix_pos ] = bin;
                } 
            }

			if ( p -> data[bin]  > 0.0 ) 
                _weight = sqrt ( ref_hist -> data[bin] / p -> data[bin] );
			else 
                _weight = 0.0;


            // SQR (x) = x*x
            _xx = SQR(( j - cenx )/bandwidth.hx) + SQR(( i - ceny )/bandwidth.hy); 

            assert( ( _xx >= 0.0 ) && ( _xx <= 2.0 ) );

            _profd = profile_derivative( _xx, Tracking_Kernel );

            _sumX += j * _weight * _profd;
            _sumY += i * _weight * _profd;

            _sum += _weight * _profd;
        }

    if ( _sum < 1e-8 ) 
    {
        if ( _sumX > 1e-8 || _sumY > 1e-8 )
        {
            fprintf(stderr, "- Error in calculating the new positions\n");
            exit(-1);
        }
        _sum = 1e-5;    
    }

    // this is the new center
    double newX = _sumX/_sum + 0.5;
    double newY = _sumY/_sum + 0.5;

    assert(newX > 0);
    assert(newY > 0);
    assert(newX < cur_frame->width()-1);
    assert(newY < cur_frame->height()-1);

#if 0
    // this clip might not be necessary.
    // clip the rectange in the image
    if ( newX < bandwidth.hx ) newX = bandwidth.hx;
    if ( newY < bandwidth.hy ) newY = bandwidth.hy;
    if ( newX + bandwidth.hx > cur_frame->width()- 1)
        newX = cur_frame->width()- bandwidth.hx - 1;
    if ( newY + bandwidth.hy > cur_frame->height()- 1)
        newY = cur_frame->height()- bandwidth.hy - 1;
#endif

    // this clip might be sufficient    
    if (newX < 0 ) newX = 0;
    if (newY < 0 ) newY = 0;
    if (newX >= cur_frame->width()- 1) newX = cur_frame->width() - 1;
    if (newY >= cur_frame->height()- 1) newY = cur_frame->height() - 1;

    Rect new_rect;
    new_rect.set( (int) (newX - bandwidth.hx),
            (int) (newY - bandwidth.hy),
            (int) (newX + bandwidth.hx),
            (int) (newY + bandwidth.hy) );

    if (p) delete p;

    return new_rect;    
}


/** 
 * @brief Run mean shift iteration
 * @param subregion previous convergence place
 * @param cur_frame CvImage; current frame
 */
int PlainMeanshift::run( Rect * subregion, CvImage * cur_frame )
{
	Rect subregion1;
    double tol;

    int iter = 0;
    while( iter < MAX_ITER)
    {
        echo("  mean shift iteration: %d\n\n", iter );
        subregion1 = calc_estimate(subregion, cur_frame);
        tol = norm2(subregion, &subregion1);  

        if (tol<TOLERANCE_SQU)
            break;
        else
            *subregion = subregion1;

        iter ++;
    }
    if ( MAX_ITER == iter ) fprintf(stderr,"- Max iterations reached.\n");

    target_region = subregion1;       // NOTE NOT subregion
	return 1;
}

/** 
 * @brief Calculate the distance between the centers of the two rectangles
 * @param subregion1 
 * @param subregion2 
 * @return returns the squared L2 distance of two rectangle (same size) 
 */
double PlainMeanshift::norm2(Rect * subregion1, Rect * subregion2)
{
    return (subregion1->cen_x- subregion2->cen_x )*(subregion1->cen_x - subregion2->cen_x ) +
        (subregion1->cen_y - subregion2->cen_y )*(subregion1->cen_y - subregion2->cen_y );
}

bool PlainMeanshift::save( char * filename, char * image_name )
{
    FILE * pfile;
    pfile = fopen (filename,"a+t");

    if (pfile)
    {

        fprintf ( pfile, "%d %d %d %d %s\n", 
                target_region.x0, 
                target_region.y0, 
                target_region.x1, 
                target_region.y1,
                image_name );

        fclose (pfile);
        return SUCCESS;        
    }
    else
    {
        fprintf(stderr,"- Cannot open file %s.\n", filename);
        return FAILURE;
    }
}

void PlainMeanshift::set_bincache( bool yesno_, CvImage * cur_frame)
{
    use_bin_index_cache = yesno_;
	if ((m_iWidth != cur_frame->width()) || (m_iHeight != cur_frame->height())) {
		m_iWidth = cur_frame->width();
		m_iHeight = cur_frame->height();
		bin_index_cache = new int[m_iWidth * m_iHeight];
	}
    for (int i = 0; i < (int)(m_iWidth * m_iHeight); i++)
    {
        bin_index_cache[ i ] = -1;   
    }    

}


// vim: tw=78 ts=4 sw=4 sts=0 ft=cpp enc=utf-8 ff=unix nowrap et 
// EOF.
