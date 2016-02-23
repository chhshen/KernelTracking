/*
 *   author           Chunhua Shen {http://users.rsise.anu.edu.au/~cs/}
 *   file             Image.cxx
 *   creation         21-May-2006 10:11.
 *   last revision    15-Feb-2007 12:22.
 */

///
/// An image class;
/// So far only (unsigned char) 8-bit images supported
/// i.e. Color images (24-bit) and 8-bit gray images
///

#include "Image.h"

////////////////////////////////////////////////////////////////
Point::Point ()
{
    x = y = 0;
}


Point::Point (int x_, int y_)
{
    x = x_;
    y = y_;
}


Point & Point::operator += (const Point & p)
{
    x += p.x;
    y += p.y;
    return *this;
}


Point & Point::operator -= (const Point & p)
{
    x -= p.x;
    y -= p.y;
    return *this;
}


Point & Point::operator *= (int mag)
{
    x *= mag;
    y *= mag;
    return *this;
}


Point & Point::operator /= (int div)
{
    if (div != 0)
    {
        x /= div;
        y /= div;
    }
    return *this;
}


bool Point::operator != (const Point & p)
{
    return (x != p.x) || (y != p.y);
}


bool Point::operator == (const Point & p)
{
    return (x == p.x) && (y == p.y);
}


void Point::set (int x_, int y_)
{
    x = x_;
    y = y_;
}


void Point::get (int &x_, int &y_) const
{
    x_ = x;
    y_ = y;
}


double Point::distance (const Point & p) const
{
    return sqrt ((double) ((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y)));
}


void Point::dump_info ()
{
    fprintf (stderr, "- \n");
    fprintf (stderr, " (x, y) = (%d, %d) \n", x, y);
    fprintf (stderr, "- \n");
}


////////////////////////////////////////////////////////////////
Rect::Rect ():
    x0 (0), 
    y0 (0), 
    x1 (0), 
    y1 (0), 
    cen_x (0), 
    cen_y (0), 
    width (0),
    height (0)
{

}


Rect::Rect (unsigned int __x0, unsigned int __y0, unsigned int __x1,
        unsigned int __y1):
    x0 (0),
    y0 (0),
    x1 (0),
    y1 (0)
{
    x0 = __x0;
    y0 = __y0;
    x1 = __x1;
    y1 = __y1;
    cen_x = (float) (x0 + x1) / 2.0;
    cen_y = (float) (y0 + y1) / 2.0;
    width = x1 - x0;
    height = y1 - y0;

}


Rect::~Rect ()
{
}


void Rect::set (unsigned int __x0, unsigned int __y0, unsigned int __x1,
        unsigned int __y1)
{
    x0 = __x0;
    y0 = __y0;
    x1 = __x1;
    y1 = __y1;
    cen_x = (float) (x0 + x1) / 2.0;
    cen_y = (float) (y0 + y1) / 2.0;
    width = x1 - x0;
    height = y1 - y0;
}


Rect & Rect::move (const int x, const int y)
{
    x0 += x;
    x1 += x;
    y0 += y;
    y1 += y;
    calc_size ();
    calc_centre ();

    return *this;
}


Rect & Rect::operator= (const Rect & range)
{
    x0 = range.x0;
    x1 = range.x1;
    y0 = range.y0;
    y1 = range.y1;
    calc_size ();
    calc_centre ();

    return *this;
    // Return a reference to myself.
    // Assignment operator returns left side.
}

/// this operation scale the rectangle but keep its centre unchanged 
Rect & Rect::operator* (const double scale)
{
    double width_ =  scale * (x1 - x0);
    double height_ = scale *(y1 - y0);

    x0 = (int)(cen_x  - 0.5 * width_);
    x1 = (int)(cen_x  + 0.5 * width_);
    y0 = (int)(cen_y  - 0.5 * height_);
    y1 = (int)(cen_y  + 0.5 * height_);

    width = x1 - x0;
    height = y1 - y0;

    return *this;
    // Return a reference to myself.
    // Assignment operator returns left side.
}


/// this operation scale the rectangle but keep its centre unchanged 
Rect & Rect::operator* (const int scale)
{

    width = scale * (x1 - x0);
    height = scale *(y1 - y0);

    x0 = (int)(cen_x  - 0.5 * width);
    x1 = (int)(cen_x  + 0.5 * width);
    y0 = (int)(cen_y  - 0.5 * height);
    y1 = (int)(cen_y  + 0.5 * height);

    return *this;
    // Return a reference to myself.
    // Assignment operator returns left side.
}

/// this operation scale the rectangle but keep its centre unchanged 
Rect & Rect::operator/ (const double scale)
{
    double width_ =  1/scale * (x1 - x0);
    double height_ = 1/scale * (y1 - y0);

    x0 = (int)(cen_x  - 0.5 * width_);
    x1 = (int)(cen_x  + 0.5 * width_);
    y0 = (int)(cen_y  - 0.5 * height_);
    y1 = (int)(cen_y  + 0.5 * height_);

    width = x1 - x0;
    height = y1 - y0;

    return *this;
    // Return a reference to myself.
    // Assignment operator returns left side.
}


/// this operation scale the rectangle but keep its centre unchanged 
Rect & Rect::operator/ (const int scale)
{

    double width_ = 1.0/(double)scale * (x1 - x0);
    double height_ = 1.0/(double)scale *(y1 - y0);

    x0 = (int)(cen_x  - 0.5 * width_ );
    x1 = (int)(cen_x  + 0.5 * width_ );
    y0 = (int)(cen_y  - 0.5 * height_ );
    y1 = (int)(cen_y  + 0.5 * height_ );

    width = x1 - x0;
    height = y1 - y0;

    return *this;
    // Return a reference to myself.
    // Assignment operator returns left side.
}



bool Rect::operator== (const Rect & range)
{
    return (x0 == range.x0 && x1 == range.x1 && y0 == range.y0
            && y1 == range.y1);
}


bool Rect::operator!= (const Rect & range)
{
    return !(x0 == range.x0 && x1 == range.x1 && y0 == range.y0
            && y1 == range.y1);
}


void Rect::calc_centre ()
{
    cen_x = (float) (x0 + x1) / 2.0;
    cen_y = (float) (y0 + y1) / 2.0;
}


/* 
 *   O---width----> X
 *   |
 *  height
 *   |
 *   Y
 **/
void Rect::calc_size ()
{
    width = x1 - x0;
    height = y1 - y0;

    assert (width > 0);
    assert (height > 0);
}


void Rect::dump_info ()
{
    fprintf (stderr, "- \n");
    fprintf (stderr, " (x0, y0) = (%d, %d) \n", x0, y0);
    fprintf (stderr, " (x1, y1) = (%d, %d) \n", x1, y1);
    fprintf (stderr, " (cen_x, cen_y) = (%f, %f)\n", cen_x, cen_y);
    fprintf (stderr, " (width, height) = (%d, %d) \n", width, height);
    fprintf (stderr, "- \n");
}

bool Rect::CheckRange(int w, int h)
{
	if ((x1 < 0) || (x0 > w) || (y1 < 0) || (y0 > h)) {
	  fprintf (stderr, "Rect - out of range \n");
	  return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////
/// construction
Image::Image ():
    width (0), 
    height (0), 
    data (NULL), 
    colormode (COLOR_24B)
{

}


/// construction
Image::Image (unsigned int w, unsigned int h):
    width (0),
    height (0),
    data (NULL),
    colormode (COLOR_24B)
{
    set_size (w, h);
}


/// construction
Image::Image (unsigned int w, unsigned int h, int set_a_colormode):
    width (0),
    height (0),
    data (NULL)
{

    // colormode must be set before set_size
    colormode = set_a_colormode;

    set_size (w, h);

}


/// destruction
Image::~Image ()
{
    if (data != NULL)
        delete [] data;
}


///
/// define operator
///
int Image::operator   () (unsigned int x, unsigned int y)
{
    assert (data != NULL);
    assert (x >= 0);
    assert (x < width);
    assert (y >= 0);
    assert (y < height);
    assert (colormode == GRAY_8B);

    return data[x + y * width];

}


/**
 * @brief overide operator ()
 * @return Returns the pixel value at (x,y, channel)
 */
int Image::operator() (unsigned int x, unsigned int y, const char channel)
{
    assert (data != NULL);
    assert (x >= 0);
    assert (x < width);
    assert (y >= 0);
    assert (y < height);
    assert (colormode == COLOR_24B);

    switch (channel)
    {
        case 'r':
        case 'R':
            return data[x * 3 + y * width * 3];

        case 'g':
        case 'G':
            return data[x * 3 + 1 + y * width * 3];

        case 'b':
        case 'B':
            return data[x * 3 + 2 + y * width * 3];

        default:
            fprintf (stderr, "- Error. Channel should be R, G, B.\n");
            return FAILURE;
    }
}


/**
 * @brief overide operator (). This version is a little faster than the above one
 * @param channel channel must be 0, 1, 2, corresponding to channel R,G,B
 * @return Returns the pixel value at (x,y, channel)
 */
    int
Image::operator() (unsigned int x, unsigned int y, const int channel)
{
    assert (data != NULL);
    assert (x >= 0);
    assert (x < width);
    assert (y >= 0);
    assert (y < height);
    assert (colormode == COLOR_24B);
    assert (channel == 0 || channel == 1 || channel == 2);

    return data[x * 3 + channel + y * width * 3];

}



/**
 * @brief set the size of an image
 * @param w image width
 * @param h image height
 * @return Returns SUCCESS or FAILURE
 */
bool Image::set_size (unsigned int w, unsigned int h)
{
    if (data != NULL)
        delete [] data;

    width = w;
    height = h;

    if (COLOR_24B == colormode)
    {
        data = new u_char[w * h * 3];
        memset (data, 0, sizeof (u_char) * width * height * 3);
    }
    else if (GRAY_8B == colormode)
    {
        data = new u_char[w * h];
        memset (data, 0, sizeof (u_char) * width * height);
    }
    else
    {
        echo ("- Error. Unknown color mode. Failed to set image size.\n");
        return FAILURE;
    }

    return SUCCESS;
}


/**
 * @brief copy image data
 * @param image_data source image data
 * @return Returns SUCCESS or FAILURE
 */
bool Image::set_value (u_char * image_data)
{
    // Not a good way for initialisation
    // data = ppm_data or pgm_data;
    // better copy the memory
#ifdef _DEBUG
    if (data == NULL)
    {
        fprintf (stderr, "- Error. The image is not initialised yet!\n");
        return (FAILURE);
    }
#endif
    if (COLOR_24B == colormode)
        memcpy (data, image_data, sizeof (u_char) * height * width * 3);
    else if (GRAY_8B == colormode)
        memcpy (data, image_data, sizeof (u_char) * height * width);
    else
    {
        echo ("- Error. Unknown color mode. Failed to copy image data.\n");
        return FAILURE;
    }
    return SUCCESS;
}


bool Image::set_pix ( unsigned int x, unsigned int y, u_char pix_value )
{
    assert (data != NULL);
    assert (x >= 0);
    assert (x < width);
    assert (y >= 0);
    assert (y < height);
    assert (colormode == GRAY_8B);

    data[x + y * width] = pix_value;

    return SUCCESS;
}


bool Image::set_pix ( unsigned int x, unsigned int y, const int channel, u_char pix_value )
{  
    assert (data != NULL);
    assert (x >= 0);
    assert (x < width);
    assert (y >= 0);
    assert (y < height);
    assert (colormode == COLOR_24B);
    assert (channel == 0 || channel == 1 || channel == 2);

    data[x * 3 + channel + y * width * 3] = pix_value;

    return SUCCESS;
}

int Image::get_pix (unsigned int x, unsigned int y, const int channel)
{
    assert (data != NULL);
    assert (x >= 0);
    assert (x < width);
    assert (y >= 0);
    assert (y < height);
    assert (colormode == COLOR_24B);
    assert (channel == 0 || channel == 1 || channel == 2);

    return data[x * 3 + channel + y * width * 3];

}


/**
 * @brief clone an image from another Image.
 * Similar to OpenCV's cvClone
 * @param img source image
 * @return returns SUCCESS or FAILURE
 */
bool Image::clone (Image & img)
{
    // Note that you should set the colormode before set_size
    colormode = img.colormode;

    if (!set_size (img.width, img.height))
        return FAILURE;

    if (COLOR_24B == colormode)
        memcpy (data, img.data, sizeof (u_char) * height * width * 3);
    else if (GRAY_8B == colormode)
        memcpy (data, img.data, sizeof (u_char) * height * width);
    else
    {
        echo ("- Error. Unknown color mode. Failed to copy image data.\n");
        return FAILURE;
    }
    return SUCCESS;
}


/**
 * @brief convert RGB (24Bits) to Gray (8Bits)
 * Use OpenCV's convert function might be faster *
 * @param new_colormode gray color mode; currently only GRAY_8B supported
 * @return Returns SUCCESS or FAILURE
 */
bool Image::togray (int new_colormode)
{
    int kk;
    if (COLOR_24B == colormode && GRAY_8B == new_colormode)
    {
        for (unsigned int y = 0; y < height; y++)
            for (unsigned int x = 0; x < width; x++)
            {
                kk = x * 3 + y * width * 3;

                data[x + 1 + y * width] = 
                    (int)( (data[ kk ] + data[ kk + 1 ] + data[ kk + 2 ]) / 3.0);
            }
        colormode = GRAY_8B;
    }
    else
    {
        echo ("- Error. Unknown color mode. Failed to convert images.\n");
        return FAILURE;
    }

    return SUCCESS;
}


/**
 * @brief to tell whether an image is empty or not
 * @return returns true or false
 */
bool Image::is_empty ()
{
    if (0 == width || 0 == height)
        return TRUE;
    else
        return FALSE;
}


/**
 * @brief dump the image information.
 * This function should be used only for the debug purpose.
 * For release version, you should not call this function
 */
    void
Image::dump_info ()
{

    fprintf (stderr, "- \n");
    fprintf (stderr, "width : %d\n", width);
    fprintf (stderr, "height: %d\n", height);
    fprintf (stderr, "color mode: %d\n", colormode);

    if (NULL == data)
        fprintf (stderr, "image data is empty (NULL).\n");
    else
    {
        fprintf (stderr, "image data is not empty.\n");
    }
    fprintf (stderr, "- \n");
}

/**
 * @brief save data to a PGM image
 * @param file_name file name, char *
 * @return Returns SUCCESS or FAILURE
 */
bool Image::writepgm (char *file_name, int mode, unsigned int max_val_ )
{

    unsigned int max_val = max_val_;

    assert (colormode == GRAY_8B);

    FILE * pFile;

    pFile = fopen (file_name, "wt");

    if (pFile == NULL)
    {
        fprintf (stderr, "- Error. Cannot create ppm file.\n");
        return (FAILURE);
    }

    /* write file type */
    switch (mode)
    {
        case 0:                         // binary
            fprintf (pFile, "P5\n");
            break;
        case 1:                         // ascii
            fprintf (pFile, "P2\n");
            break;
        default:
            fprintf (stderr, "- Illegal mode %d. File %s is not written.", mode,
                    file_name);
            fclose (pFile);
            return FAILURE;
    }

    /* write comment */
    fprintf (pFile, "#File %s written by Image::writepgm.\n", file_name);

    fprintf (pFile, "%d  %d\n%d\n", width, height, max_val);

    /* write image data */
    switch (mode)
    {
        case 0:                         // binary mode
            for (unsigned int i = 0; i < height; i++)
                for (unsigned int j = 0; j < width; j++)
                    fputc (data[j + i * width], pFile);

            break;

        case 1:                         // ascii mode
            for (unsigned int i = 0; i < height; i++)
                for (unsigned int j = 0; j < width; j++)
                {
                    fprintf (pFile, " %d ", data[j + i * width]);

                    if (j == width - 1)
                        fprintf (pFile, "\n");
                }
            break;
    }

    if ( !fclose (pFile) )
        return FAILURE;

    return SUCCESS;
}


/**
 * @brief save data to a PPM image
 * @param file_name file name, char *
 * @return Returns SUCCESS or FAILURE
 */
bool Image::writeppm (char *file_name, int mode, unsigned int max_val_ )
{

    int max_val = max_val_;

    assert (colormode == COLOR_24B);

    FILE * pFile;

    pFile = fopen (file_name, "wt");

    if (pFile == NULL)
    {
        fprintf (stderr, "- Error. Cannot create ppm file.\n");
        return (FAILURE);
    }

    /* write file type */
    switch (mode)
    {
        case 0:
            fprintf (pFile, "P6\n");
            break;
        case 1:
            fprintf (pFile, "P3\n");
            break;
        default:
            fprintf (stderr, "- Illegal mode %d. File %s is not written.", mode,
                    file_name);
            fclose (pFile);
            return FAILURE;
    }

    /* write comment */
    fprintf (pFile, "#File %s written by Image::writeppm.\n", file_name);

    fprintf (pFile, "%d  %d\n%d\n", width, height, max_val);

    switch (mode)
    {
        case 1:                         //ascii
            for (unsigned int i = 0; i < height; i++)
                for (unsigned int j = 0; j < width; j++)
                {
                    fprintf (pFile, "%d %d %d ",
                            data[j * 3 + i * width * 3],
                            data[j * 3 + 1 + i * width * 3],
                            data[j * 3 + 2 + i * width * 3]);

                    if (j == width - 1)
                        fprintf (pFile, "\n");
                }
            break;

        case 0:                         //binary
            char  array[3];
            for (unsigned int i = 0; i < height; i++)
                for (unsigned int j = 0; j < width; j++)
                {
                    array[0] = data[j * 3 + i * width * 3];
                    array[1] = data[j * 3 + 1 + i * width * 3];
                    array[2] = data[j * 3 + 2 + i * width * 3];
                    fwrite ((char *) array, sizeof (char), 3, pFile);
                }
            break;
    }

    if (!fclose (pFile))
        return FAILURE;

    return SUCCESS;
}


///
/// save a ppm image with a rectangle mark
///
bool Image::draw_rect_ppm (char *file_name, Rect & region,
        const RGBColor & color)
{
    assert (region.x0 >= 0);
    assert (region.x1 > region.x0);
    assert (region.x1 <= (int)width);
    assert (region.y0 >= 0);
    assert (region.y1 > region.y0);
    assert (region.y1 <= (int)height);
    assert (colormode == COLOR_24B);

    // copy data to another array such that the original data will not be modified
    u_char *tp_data = new u_char[height * width * 3];

    memcpy (tp_data, data, sizeof (u_char) * height * width * 3);

    int j = region.x0;

    for (int i = region.y0; i <= region.y1; i++)
    {
        tp_data[j * 3 + i * width * 3] = color.r;
        tp_data[j * 3 + 1 + i * width * 3] = color.g;
        tp_data[j * 3 + 2 + i * width * 3] = color.b;
    }

    j = region.x1;
    for (int i = region.y0; i <= region.y1; i++)
    {
        tp_data[j * 3 + i * width * 3] = color.r;
        tp_data[j * 3 + 1 + i * width * 3] = color.g;
        tp_data[j * 3 + 2 + i * width * 3] = color.b;
    }

    int i = region.y0;

    for (int j = region.x0; j <= region.x1; j++)
    {
        tp_data[j * 3 + i * width * 3] = color.r;
        tp_data[j * 3 + 1 + i * width * 3] = color.g;
        tp_data[j * 3 + 2 + i * width * 3] = color.b;
    }

    i = region.y1;
    for (int j = region.x0; j <= region.x1; j++)
    {
        tp_data[j * 3 + i * width * 3] = color.r;
        tp_data[j * 3 + 1 + i * width * 3] = color.g;
        tp_data[j * 3 + 2 + i * width * 3] = color.b;
    }

    FILE *pFile;

    pFile = fopen (file_name, "wt");

    if (pFile == NULL)
    {
        fprintf (stderr, "- Error. Cannot create ppm file.\n");
        return FAILURE;
    }

    fprintf (pFile, "P3\n %d %d\n  255 \n", width, height);

    for (unsigned int i = 0; i < height; i++)
        for (unsigned int j = 0; j < width; j++)
        {
            fprintf (pFile, " %d %d %d ",
                    tp_data[j * 3 + i * width * 3],
                    tp_data[j * 3 + 1 + i * width * 3],
                    tp_data[j * 3 + 2 + i * width * 3]);

            if (j == width - 1)
                fprintf (pFile, "\n");
        }
     
    // Upon successful completion 0 is returned.
     if ( fclose (pFile) )
     {
         fprintf (stderr, "- Error. Cannot close the ppm file.\n");
         return FAILURE;
     }

    delete [] tp_data;
    return SUCCESS;
}


/**
 * @brief read a pgm file
 * @param file_name filename, char *
 * @return returns SUCCESS or FAILURE
 */
bool Image::readpgm (char *file_name)
{
    assert (colormode == GRAY_8B);

    unsigned int  w;                 //image width
    unsigned int  h;                 //image height
    int     pix_max;
    char    buffer[200];

    int     pgm_type;

    FILE *  input;

    if ((input = fopen (file_name, "r")) == NULL)
    {
        fprintf (stderr, "- Error. Cannot open file %s.\n", file_name);
        return FAILURE;
    }

    /* read a line of input */
    fgets (buffer, 200, input);
    if (strncmp (buffer, "P5", 2) == 0)
        pgm_type = 0;                   //binary PPM image
    else if (strncmp (buffer, "P2", 2) == 0)
        pgm_type = 1;                   //ASCII PPM image
    else
    {
        fprintf (stderr, "- Error. %s is not a PGM file.\n", file_name);
        return FAILURE;
    }

#if 0
    ///// read comments
    char
        c;

    do
    {
        while ((c = fgetc (input)) != '\n')
            ;
    }
    while ((c = fgetc (input)) == '#');
    ungetc (c, input);
#endif

    /* get second line, ignoring comments */
    do
    {
        fgets (buffer, 200, input);
    }
    while (strncmp (buffer, "#", 1) == 0);

    if (sscanf (buffer, "%d %d", &w, &h) != 2)
    {
        fprintf (stderr, "- Error. Can't read sizes!\n");
        return FAILURE;
    }

    /* third line, ignoring comments */
    do
    {
        fgets (buffer, 200, input);
    }
    while (strncmp (buffer, "#", 1) == 0);

    if (sscanf (buffer, "%d", &pix_max) != 1)
    {
        fprintf (stderr, "- Error. What about max size?\n");
        return FAILURE;
    }

    if ( pix_max > 255 )
    {
        fprintf (stderr, "- Only BYTE ppm is support. Rescale it!\n");
        return FAILURE;
    }

    if (w != width || h != height)
    {
        echo ("- Warning. The image size doesn't match the pgm file.\n");
        echo ("- The image is resized.\n");
        width = w;
        height = h;
        set_size (width, height);
    }

    if (0 == pgm_type)                  //binary PGM
    {
        for (unsigned int i = 0; i < height; i++)
            for (unsigned int j = 0; j < width; j++)
            {
                data[j + i * width] = (u_char) fgetc (input);
            }
    }
    else                                //ascii PGM
    {
        int p = 0;

        while (!feof (input))
        {
            fscanf (input, "%d", (int *) (data + p));
            p++;
        }
    }

    return SUCCESS;
}


/**
 * @brief
 * you can convert * many kinds of image files to raw .ppm using "xv" or
 * "convert" It opens the file and reads the contents into an array of
 * unsigned bytes, three for each pixel.  Loads the PPM file specified in
 * "filename".
 *
 * PPM header must be:
 * - -
 * P6
 * width height
 * max_pix_value
 * - -
 */
bool Image::readppm (char *file_name)
{

    assert (colormode == COLOR_24B);

    unsigned int   w;                              //image width
    unsigned int   h;                              //image height
    int     pix_max;
    char    rgb[3];
    char    buffer[200];

    int     mode;

    FILE *     input;

    if ((input = fopen (file_name, "r")) == NULL)
    {
        fprintf (stderr, "- Error. Cannot open file %s.\n", file_name);
        return FAILURE;
    }

    /* read a line of input */
    fgets (buffer, 200, input);
    if (strncmp (buffer, "P6", 2) == 0)
        mode = 0;                       //binary PPM image
    else if (strncmp (buffer, "P3", 2) == 0)
        mode = 1;                       //ASCII PPM image
    else
    {
        fprintf (stderr, "- Error. %s is not a PPM file.\n", file_name);
        return FAILURE;
    }

    /* get second line, ignoring comments */
    do
    {
        fgets (buffer, 200, input);
    }
    while (strncmp (buffer, "#", 1) == 0);

    if (sscanf (buffer, "%d %d", &w, &h) != 2)
    {
        fprintf (stderr, "- Error. Can't read sizes!\n");
        return FAILURE;
    }

    /* third line, ignoring comments */
    do
    {
        fgets (buffer, 200, input);
    }
    while (strncmp (buffer, "#", 1) == 0);

    if (sscanf (buffer, "%d", &pix_max) != 1)
    {
        fprintf (stderr, "- Error. What about max size?\n");
        return FAILURE;
    }

    if ( pix_max > 255 )
    {
        fprintf (stderr, "- Only BYTE ppm is support. Rescale it!\n");
        return FAILURE;
    }
    //
    // fprintf(stderr,"- Reading %d columns %d rows.\n", w, h);
    //

    if (w != width || h != height)
    {
        echo ("- Warning. The image size doesn't match the ppm file.\n");
        echo ("- The image is resized.\n");
        width = w;
        height = h;
        set_size (width, height);
    }

    if (0 == mode)                      //binary PPM
    {
        for (unsigned int i = 0; i < height; i++)
            for (unsigned int j = 0; j < width; j++)
            {
                fread (rgb, sizeof (char), 3, input);
                for (int k = 0; k < 3; k++)
                {
                    data[j * 3 + k + i * width * 3] = rgb[k];
                }
            }
    }
    else                                //ascii PPM
    {
        int    p = 0;
        while (!feof (input))
        {
            fscanf (input, "%d", (int *) (data + p));
            p++;
        }
    }
    return SUCCESS;
}


#ifdef OPENCV_INSTALLED

/**
 * @brief
 * convert an Image image to an OpenCV IplImage image
 * using cvSetData( ) provided by OpenCV. The main issue is that
 * IplImage stores pixel values in BGR order while RGB for Image
 * image. cvCvtColor(image, image, CV_RGB2BGR) is then used to
 * swap between RGB and BGR.
 *
 * @return Return an OpenCV IplImage image
 */
IplImage * Image::toIplImage ()
{

    IplImage *image;

    if ( COLOR_24B == colormode )
    {
        image = cvCreateImage (cvSize (width, height), IPL_DEPTH_8U, 3);

        // You can also use
        // cvSetData( image, tp_data, width*3 );
        // But then you cannot use cvReleaseImage to release the image
        // Only cvReleaseImageHeader can be used to release the image header
        // And, I believe memcpy is as efficient as cvSetData if not more.
        memcpy (image->imageData, data, sizeof (u_char) * height * width * 3);

        // swap the R and B channels
        // OpenCV stores images as BRG BGR ...
        // my Image Class stores image channels as RGB RGB ...
        cvCvtColor (image, image, CV_RGB2BGR);
    }
    else if ( GRAY_8B == colormode )
    {
        image = cvCreateImage (cvSize (width, height), IPL_DEPTH_8U, 1);
        memcpy (image->imageData, data, sizeof (u_char) * height * width );
    }
    else
    {
        echo ("- Error. Unknown color mode. Failed to set image size.\n");
        return FAILURE;
    }

    // Note that you need to use cvReleaseImageHeader(&image) to release the
    // image
    // Change: Now I copied data to another block of memory Use
    // cvReleaseImage( &image) to release the image
    return image;
}

/**
 * @brief display an image using OpenCV
 * @param disp_mode display mode; check OpenCV document for details
 * @param win_name the title of the window
 * @return Returns SUCCESS or FAILURE
 */
bool Image::display (int disp_mode, char *win_name)
{
    // the 3rd param of cvCreateImageHeader is "int channels"
    IplImage * image;

    if (COLOR_24B == colormode)
        image = cvCreateImageHeader (cvSize (width, height), IPL_DEPTH_8U, 3);
    else if (GRAY_8B == colormode)
        image = cvCreateImageHeader (cvSize (width, height), IPL_DEPTH_8U, 1);
    else
    {
        echo ("- Error. Unknown color mode. Failed to set image size.\n");
        return FAILURE;
    }

    if (NULL == image)
    {
        fprintf (stderr, "- Error. Creating IplImage image failed.\n");
        return FAILURE;
    }

    if (COLOR_24B == colormode)
    {
        cvSetData (image, data, width * 3);
        // swap the R and B channels
        cvCvtColor (image, image, CV_RGB2BGR);
    }
    else if (GRAY_8B == colormode)
        cvSetData (image, data, width);
    else
        ;                               // keep this line


    if ( width < 100)
    {
        cvNamedWindow (win_name, 0);
        cvResizeWindow (win_name, 100 , (int)(100.0 * height/width));
    }
    else
    {
        cvNamedWindow (win_name, 1);
    }

    cvShowImage (win_name, image);

    cvWaitKey (disp_mode);              /* if 0, waits for key event infinitely */

    if (disp_mode != 5)
        cvDestroyWindow (win_name);

    // do not use cvReleaseImage; otherwise the Image data will be released
    cvReleaseImageHeader (&image);

    return SUCCESS;
}


/*
 * copy the pixel values from an IplImage image
 * Note different RGB/BGR orders. See above descriptions.
 * @param IplImage * image
 */
bool Image::set_value (IplImage * image)
{

    if ( NULL == image )
    {
        echo ("- The IplImage image is NULL.\n");
        return FAILURE;
    }
    
    // printf("%d\n" , image->depth);

    if ( image->depth >= IPL_DEPTH_32F)
    {
        echo ("- The IplImage image is 32/64-bit. Not supported yet.\n");
        return FAILURE;
    }    

    if ((unsigned int) image->width != width
            || (unsigned int) image->height != height)
    {
        echo ("- Warning. The image size doesn't match the IplImage image.\n");
        echo ("- The image is resized.\n");
        width = image->width;
        height = image->height;
        set_size (width, height);
    }


    if ( colormode == COLOR_24B )
    {
        // swap the R and B channels
        cvCvtColor (image, image, CV_RGB2BGR);
        memcpy (data, image->imageData, sizeof (u_char) * height * width * 3);
        // swap the R and B channels BACK
        cvCvtColor (image, image, CV_RGB2BGR);
    }
    else if (colormode == GRAY_8B)
    {
        memcpy (data, image->imageData, sizeof (u_char) * height * width);
    }
    else
    {
        echo ("- Error. Unknown color mode. Failed to set image values.\n");
        return FAILURE;
    }

    return SUCCESS;
}


bool Image::display (const Rect & rectangle, int disp_mode, char *win_name)
{
    IplImage *
        image = cvCreateImageHeader (cvSize (width, height), IPL_DEPTH_8U, 3);

    if (NULL == image)
    {
        fprintf (stderr, "- Error. Creating IplImage image failed.\n");
        return FAILURE;
    }

    u_char *  tp_data = new u_char[height * width * 3];

    memcpy (tp_data, data, sizeof (u_char) * height * width * 3);

    cvSetData (image, tp_data, width * 3);

    // swap the R and B channels
    cvCvtColor (image, image, CV_RGB2BGR);

    int        w = 1;
    CvScalar   color = CV_RGB (0, 255, 0);

    // draws an X at point pt on image img.  The X has radius r, weight w,
    //  and color c.
#if 0
    int
        r = 2;

    CvPoint
        pt;

    pt.x = (int) ((rectangle.x0 + rectangle.x1) / 2);
    pt.y = (int) ((rectangle.y0 + rectangle.y1) / 2);
    cvLine (image, pt, cvPoint (pt.x + r, pt.y + r), color, w, 8, 0);
    cvLine (image, pt, cvPoint (pt.x - r, pt.y + r), color, w, 8, 0);
    cvLine (image, pt, cvPoint (pt.x + r, pt.y - r), color, w, 8, 0);
    cvLine (image, pt, cvPoint (pt.x - r, pt.y - r), color, w, 8, 0);
#endif

    color = CV_RGB (0, 255, 0);

    /*
    // add text:
    if ( NULL != title )
    {

    CvFont font;
    double hScale=0.6;
    double vScale=0.6;
    w = 1;                         // thickness
    cvInitFont(&font, CV_FONT_HERSHEY_PLAIN,
    hScale,vScale,0, w );

    // background for the texts
    cvRectangle( image,  cvPoint( rectangle.x0 , rectangle.y0 - 15 ),
    cvPoint( rectangle.x0 + 20, rectangle.y0 -3 ),
    cvScalar(0,0,0) , CV_FILLED );

    cvRectangle( image,  cvPoint( rectangle.x0 , rectangle.y0 - 15 ),
    cvPoint( rectangle.x0 + 20, rectangle.y0 -3 ),
    color , 1 );

    cvPutText (image, title, cvPoint( rectangle.x0 + 2, rectangle.y0 - 5),
    &font, cvScalar(128,255,0) );

    }
     */

    // draws the rectangle sides
    w = 1;
    cvRectangle (image, cvPoint (rectangle.x0, rectangle.y0),
            cvPoint (rectangle.x1, rectangle.y1), color, w);

#ifdef IMG_DISPLAY_WIDTH
    cvNamedWindow (win_name, 0);
    cvResizeWindow (win_name, IMG_DISPLAY_WIDTH, IMG_DISPLAY_HEIGHT);
#else
    cvNamedWindow (win_name, 0);
#endif

    cvShowImage (win_name, image);

    cvWaitKey (disp_mode);
    /*if 0, waits for key event infinitely */

    if (5 != disp_mode)
        cvDestroyWindow (win_name);

    // do not use cvReleaseImage; otherwise the Image data
    // will be released
    cvReleaseImageHeader (&image);
    delete [] tp_data;

    return SUCCESS;
}


bool Image::read (char *file_name)
{
    IplImage *  image = NULL;

    // By default, the loaded image is forced to be a 3-channel color image.
    // This default can be modified by using:
    //             img=cvLoadImage(fileName,flag);
    // flag: >0 the loaded image is forced to be a 3-channel color image;
    //       =0 the loaded image is forced to be a 1 channel grayscale image
    //       <0 the loaded image is loaded as is (with number of channels in the
    //       file).

    image = cvLoadImage (file_name, -1);
    if (!image)
    {
        fprintf (stderr, "- Error. I could not load image file: %s\n",
                file_name);
        return FAILURE;
    }

    if (3 == image->nChannels && colormode != COLOR_24B)
    {
        echo
            ("- Warning. 3 color channels read, but the colormode is not COLOR_24B.\n");
        echo ("- Reset colormode to COLOR_24B.\n");
        colormode = COLOR_24B;
    }

    if (1 == image->nChannels && colormode != GRAY_8B)
    {
        echo
            ("- Warning. 1 color channel read, but the colormode is not GRAY_8B.\n");
        echo ("- Reset colormode to GRAY_8B.\n");
        colormode = GRAY_8B;
    }

    // copy image data to the class
    set_value (image);

    // release the image
    cvReleaseImage (&image);

    return SUCCESS;
}


#else                                   //OPENCV NOT INSTALLED
bool Image::read (char *file_name)
{

    // find location of file extension
    char * lastdot = strrchr (file_name, '.');

    if ( !lastdot )
        return FALSE;

    // if file has an image extension "ppm"
    if ( 0 == (strcmp (lastdot, ".ppm")) )
    {
        if (COLOR_24B != colormode)
        {
            fprintf (stderr, "- Wrong color mode! Exit.\n");
            return FAILURE;
        }

        return readppm (file_name);
    }

    if ( (strcmp (lastdot, ".pgm")) )
    {
        if (GRAY_8B != colormode)
        {
            fprintf (stderr, "- Wrong color mode! Exit.\n");
            return FAILURE;
        }
        return readpgm (file_name);
    }

    // use "convert" to convert it to ppm. Then read it.
    const char * strconvert;
    strconvert = convert_path ();

    char * tmpfile = new char[256];

    if (COLOR_24B == colormode)
        strcpy (tmpfile, "//tmp//__MyImageClassTempFile.ppm");

    if (GRAY_8B == colormode)
        strcpy (tmpfile, "//tmp//__MyImageClassTempFile.pgm");

    char * cmd = new char[2048];

    strcpy (cmd, strconvert);
    strcat (cmd, " ");
    strcat (cmd, file_name);
    strcat (cmd, " ");
    strcat (cmd, tmpfile);

    execute (cmd);

    // fprintf(stderr, "%s\n", cmd);

    if (COLOR_24B == colormode)
        bool status = readppm (tmpfile);

    if (GRAY_8B == colormode)
        bool status = readpgm (tmpfile);

    delete [] cmd;
    delete [] tmpfile;

    return status;

}
#endif                                  //END OPENCV_INSTALLED

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
/// Video
/// Simple OpenCV video wrapper
/// Require OpenCV installed, of course

#ifdef OPENCV_INSTALLED

Video::Video ()
{
    video = NULL;
    iplframe = NULL;
}


Video::Video (char *vid_file)
{
    iplframe = NULL;
    video = cvCaptureFromFile (vid_file);
    if (!video)
    {
        fprintf (stderr, "- Error in opening video file %s\n", vid_file);
    }
}


Video::~Video ()
{
    if (video)
        cvReleaseCapture (&video);
}


bool Video::read (char *vid_file)
{
    video = cvCaptureFromFile (vid_file);
    if (!video)
    {
        fprintf (stderr, "- Error in opening video file %s\n", vid_file);
        return FAILURE;
    }

    return SUCCESS;
}


bool Video::query ()
{
    assert (video != NULL);
    iplframe = cvQueryFrame (video);

    if (!iplframe)
        return FAILURE;

    frame.set_value (iplframe);

    return SUCCESS;
}


void Video::skip (unsigned int n_frames)
{
    assert (video != NULL);
    assert (n_frames > 0);

    for (unsigned int i = 1; i < n_frames; i++)
    {
        cvQueryFrame (video);
    }
};

int Video::get_frame_pos ()
{
    // Get its index where the first frame starts with an index of 0.
    return ((int) cvGetCaptureProperty (video, CV_CAP_PROP_POS_FRAMES));
}
#endif                                  // end OPENCV_INSTALLED

// vim: tw=78 ts=4 sw=4 sts=0 ft=cpp enc=utf-8 ff=unix nowrap et
// EOF.

