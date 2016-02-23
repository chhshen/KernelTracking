#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "KernelBasedTrack.h"
#include "./libLBFGS/lbfgs.h"
#include "svm.h"
#include "OnlineSVM.h"


CKernelTracking::CKernelTracking(void)
{
	m_pOnline = NULL;
	bin_index_cache = NULL;
	m_bIsGeneralized = true;
	object_size_w = 128;
	object_size_h = 128;
}

CKernelTracking::~CKernelTracking(void)
{   
	if (!m_pOnline)
		delete m_pOnline;
	if (!bin_index_cache)
		delete bin_index_cache;
}

double CKernelTracking::dCalcSupportWeight ( int bin )
{
	double dResult = 0.0;
	if (m_bIsGeneralized) { // if this application uses generalized tracking
		if (m_pOnline)
			dResult = m_pOnline->GetBinValue(bin);
		else {
			printf("\nError! SVM is empty.\n");
			dResult = 0;
		}
	}
	else  // if this application uses traditional annealed mean shift with online SVM
		dResult = sqrt( ref_hist -> data[bin] );
	
	return dResult;
}

int CKernelTracking::initSupportModel( )
{
	char strSVMInput[] = "face.online.model";
	if (!m_pOnline) {
		m_pOnline = new COnlineSVM();
		m_pOnline->LoadSVM(strSVMInput);
		m_pOnline->CalcBinValue();
		return 1;
	}
	printf("\nSVM was already created");
	return 0;
}

double CKernelTracking::dCalcEstimate( Rect * subregion, CvImage * cur_frame)
{
	unsigned int bin;

	double dBetaAndRefHist = 1.0;
	double dWeight = 0.0;
	double dKernelProf, dDerivateProf;
	double dSumX = 0.0;
	double dSumY = 0.0;
	double dSum = 0.0;
	double dDerivateSumX = 0.0;
	double dDerivateSumY = 0.0;
	double dDerivateSum = 0.0;


    double _xx;

	// calculate the candidate's kernel weighted histogram p
    p_temp->set_zeros();

    // NOTE set the bandwidth to the candidate's size
    // This is critically important
	// set_bandwidth before call this function. so commant this line by junae
//	set_bandwidth( 0.5 * subregion->width, 0.5* subregion->height );

    // calculate the candidate region's histogram
    kernel_hist(subregion, cur_frame, p_temp);

	double centX = subregion->cen_x;
    double centY = subregion->cen_y;

    // image size
    int img_width = cur_frame->width();
    int img_height = cur_frame->height();
	int iLow = subregion->y0; if (iLow < 0) iLow = 0;
	int jLow = subregion->x0; if (jLow < 0) jLow = 0;
	int iHigh = subregion->y1; if (iHigh > (img_height-1)) iHigh = img_height-1;
	int jHigh = subregion->x1; if (jHigh > (img_width-1)) jHigh = img_width-1;

	m_dDeriv[0] = m_dDeriv[1] = 0;
	///////////////////////////////////////////////////
	// assume that kernel_hist is already calculated.
	for (int i = iLow; i < iHigh; i++) { // y
        for (int j = jLow; j < jHigh; j++) // x
        {
             int pix_pos = j + i * img_width;
            bin = bin_index_cache[ pix_pos ];
			if ( p_temp->data[bin]  > 0.0 ) {
				dBetaAndRefHist = dCalcSupportWeight(bin);
				dWeight = dBetaAndRefHist / sqrt( p_temp->data[bin] );
			}
			else {
				dWeight = 0.0;
				continue;
			}

            // SQR (x) = x*x
            _xx = SQR(( j - centX )/bandwidth.hx) + SQR(( i - centY )/bandwidth.hy); 

			dKernelProf = kernel_profile(_xx, Tracking_Kernel);
            dDerivateProf = profile_derivative( _xx, Tracking_Kernel );

			dSum += dWeight * dKernelProf;

			m_dDeriv[0] += dWeight * dDerivateProf * ((double)j - centX);
			m_dDeriv[1] += dWeight * dDerivateProf * ((double)i - centY);

        }
	}
	m_dF = dSum / 2.0;
	m_dDeriv[0] /= (bandwidth.hx*bandwidth.hx);
	m_dDeriv[1] /= (bandwidth.hy*bandwidth.hy);

	// for finding maximum, change sign
	m_dF *= -1; m_dDeriv[0] *= -1; m_dDeriv[1] *= -1;

	return m_dF;
}

int CKernelTracking::run( Rect * subregion, CvImage * cur_frame )
{
	int i, ret = 0;
	double x[2], fx;

	// Initialize the variables. 
	for (i = 0;i < 2;i += 2) {
		x[i] = subregion->cen_x;
		x[i+1] = subregion->cen_y;
	}
	m_dPrev[0] = x[0];
	m_dPrev[1] = x[1];
	
	//	Start the L-BFGS optimization; this will invoke the callback functions
	//	evaluate() and progress() when necessary.
	m_cur_frame = cur_frame;
	ret = lbfgs_(2, x, &fx, evaluate, NULL, NULL, NULL);

	target_region.set( (int) (x[0] - bandwidth.hx),
		(int) (x[1] - bandwidth.hy),
		(int) (x[0] + bandwidth.hx),
		(int) (x[1] + bandwidth.hy) );

	if (fx > 0) // if bandwidth is too large, face cannot be detected. 
		return -1;
	else 
		return 1;

}


bool CKernelTracking::kernelHistforTrain(Rect * subregion, CvImage * cur_frame, Hist * kh)
{
	if (kernel_hist(subregion, cur_frame, kh) == FAILURE)
		return FAILURE;
	for (int k=0; k<N_BINS; k++) {
		if (kh->data[k] > 0)
			kh -> data[k] = sqrt(kh->data[k]);
		else
			kh -> data[k] = 0;
	}
	return SUCCESS;
}

bool CKernelTracking::save( char * filename, char * strImageName )
{
    FILE * pfile;
    pfile = fopen (filename,"wt");

    if (pfile)
    {
        fprintf ( pfile, "%d %d %d %d %s\n", 
                target_region.x0, 
                target_region.y0, 
                target_region.x1, 
                target_region.y1,
                strImageName );

        fclose (pfile);
        return SUCCESS;        
    }
    else
    {
        fprintf(stderr,"- Cannot open file %s.\n", filename);
        return FAILURE;
    }

}

bool CKernelTracking::save( FILE * pfile)
{
    if (pfile)
    {
        fprintf ( pfile, "%d %d %d %d\n", 
                target_region.x0, 
                target_region.y0, 
                target_region.x1, 
                target_region.y1
                 );

        return SUCCESS;        
    }
    else
    {
        fprintf(stderr,"- Cannot open file.\n");
        return FAILURE;
    }

}
bool CKernelTracking::updateSVM(Rect rect0, CvImage *cur_frame )
{
	//update positive data
	updateSVM(rect0, cur_frame, 1.0);

	//select negative region
	Rect rect1; 
	int x0, x1, y0, y1;
	y0 = rect0.y0; y1 = rect0.y1;
	if (rect0.x0 > object_size_w) { x0 = 0;	x1 = object_size_w; }
	else { x1 = cur_frame->width()-1;		x0 = x1 - object_size_w;	}
	rect1.set(x0, y0, x1, y1);

	//update negative data
	updateSVM(rect1, cur_frame, -1.0);

	return SUCCESS;
}

bool CKernelTracking::updateSVM(Rect rect0, CvImage* cur_frame, float fYt )
{
	if (!m_pOnline) 
		m_pOnline = new COnlineSVM();
	p_temp->clear();
	if (kernelHistforTrain( &rect0, cur_frame, p_temp) != FAILURE) {
		m_pOnline->OnlineTraining(p_temp->data, fYt);
		return SUCCESS;
	}
	return FAILURE;
}

void CKernelTracking::init(CvImage* cur_frame, int kernel_type_ )
{
 	Rect subregion(0,0,cur_frame->width()-1, cur_frame->height()-1);
	PlainMeanshift::init(&subregion, cur_frame, kernel_type_);
}

void CKernelTracking::init(Rect* subregion, CvImage* cur_frame, int kernel_type_ )
{
	PlainMeanshift::init(subregion, cur_frame, kernel_type_);
}

///L-BFGS Callback 
double evaluate(void *instance, const double *x, double *g, const int n, const double step)
{
	double fx = 0.0;
	int x0, x1;
/*	int x0 = (int)(x[0] + 0.5);
	int x1 = (int)(x[1] + 0.5);
*/
	if (g_tracker.m_dPrev[0] < x[0]) {
		if ((x[0] - g_tracker.m_dPrev[0]) >= 0.05)
			x0 = (int)(x[0]+0.95);
		else
			x0 = (int)x[0];
	}
	else
		x0 = (int)x[0];

	if (g_tracker.m_dPrev[1] < x[1]) {
		if ((x[1] - g_tracker.m_dPrev[1]) >= 0.05)
			x1 = (int)(x[1]+0.95);
		else
			x1 = (int)x[1];
	}
	else
		x1 = (int)x[1];

	g_tracker.m_dPrev[0] = x0;
	g_tracker.m_dPrev[1] = x1;

	g_tracker.target_region.set( (int) (x0 - g_tracker.bandwidth.hx),
		(int) (x1 - g_tracker.bandwidth.hy),
		(int) (x0 + g_tracker.bandwidth.hx),
		(int) (x1 + g_tracker.bandwidth.hy) );

	// for assert(_xx<=2.0 && _xx>=0);
	if (g_tracker.target_region.cen_x > (double)((int)(g_tracker.target_region.x0+g_tracker.target_region.x1)/2)) {
		g_tracker.target_region.set(g_tracker.target_region.x0, g_tracker.target_region.y0, g_tracker.target_region.x1-1, g_tracker.target_region.y1);
	}
	if (g_tracker.target_region.cen_y > (double)((int)(g_tracker.target_region.y0+g_tracker.target_region.y1)/2)) {
		g_tracker.target_region.set(g_tracker.target_region.x0, g_tracker.target_region.y0, g_tracker.target_region.x1, g_tracker.target_region.y1-1);
	}


	g_tracker.dCalcEstimate(&g_tracker.target_region, g_tracker.m_cur_frame);
	g[0] = g_tracker.m_dDeriv[0];
	g[1] = g_tracker.m_dDeriv[1];
	fx = g_tracker.m_dF;
	return fx;
}

///L-BFGS Callback
int progress(void *instance, const double *x, const double *g, const double fx, const double xnorm, const double gnorm, const double step, int n, int k, int ls)
{
//	printf("Iteration %d:\n", k);
//	printf("  fx = %f, x[0] = %f, x[1] = %f\n", fx, x[0], x[1]);
//	printf("  xnorm = %f, gnorm = %f, step = %f\n", xnorm, gnorm, step);
//	printf("\n");
	return 0;
}
