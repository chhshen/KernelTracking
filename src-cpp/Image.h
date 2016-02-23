/*
 *   author           Chunhua Shen {http://users.rsise.anu.edu.au/~cs/}
 *   file             Image.h
 *   creation         01-May-2006 14:03.
 *   last revision    21-Nov-2006 14:04.
 */

#pragma once
#ifndef __IMAGE_H__
#define __IMAGE_H__


// color/colour mode
#define GRAY_8B		        1
#define GRAY_16B	        2
#define COLOR_24B		    3


// color channel
#define ChR                 0
#define ChG                 1
#define ChB                 2


// if not DEBUG, define NDEBUG whichs disables assert
#if !defined(_DEBUG) && !defined(NDEBUG) 
#define  NDEBUG
#endif

#if defined(_DEBUG) && !defined(VERBOSE)
#define VERBOSE
#endif

#if defined(VERBOSE) && defined(__GNUC__)
#define echo(format, ...) fprintf (stderr, format, ## __VA_ARGS__)
#define  dumpinfo(x)  fprintf (stderr,                                      \
                                "- DEBUG DUMP INFO: %s (line %d)\n",        \
                                __FILE__, __LINE__ );                       \
                          fprintf (stderr, "Structure of %s:\n", #x );      \
                          x.dump_info()
#else
#define echo(format, ...)  ((void)0)
#define dump_imginfo(x)      (( void )0)

#endif



// If you have no OPENCV installed, undefine this variable
// This results in some functions will not be available
// In future versions, this definition will be removed here
// Instead, a switch -DOPENCV_INSTALLED will be provided
// in the Makefile if the configure script detects OPENCV
// in the system. However I don't know how to do this for
// Visual C++ Win32.
#if !defined(OPENCV_INSTALLED)
#define OPENCV_INSTALLED
#endif

#ifdef OPENCV_INSTALLED
/* OpenCV library */
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "cvaux.h"
#endif

#include <cassert>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#ifndef SUCCESS
#define SUCCESS                     1
#endif
#ifndef FAILURE
#define FAILURE                     0
#endif
#ifndef TRUE
#define TRUE                        1
#endif
#ifndef FALSE                  
#define FALSE                       0
#endif


#define UCHAR_MAX_PIX_VAL         256

// image display size
// uncomment these two variables if you want the image to be shown in 
// its original size
// #define IMG_DISPLAY_WIDTH      320
// #define IMG_DISPLAY_HEIGHT     240

typedef unsigned char u_char;


inline void execute(const char *command) {
    //
    // win32 part has not been tested yet
    //
#ifdef WIN32
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    memset(&pi, 0, sizeof(pi));
    memset(&si, 0, sizeof(si));
    GetStartupInfo(&si);
    si.cb = sizeof(si);
    si.wShowWindow = SW_HIDE;
    si.dwFlags |= SW_HIDE;
    BOOL res = CreateProcess(0,(LPTSTR)command,0,0,FALSE,0,0,0,&si,&pi);
    if (res) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
#else
    system(command);
#endif
}

/// Return path of the ImageMagick's convert tool.
/**
   If you have installed the <a href="http://www.imagemagick.org">
   ImageMagick package</a> in a standard
   directory, this function should return the correct path of the \c
   convert tool used by the Image Library to load and save compressed
   image formats.  Conversely, if the \c convert executable is not
   auto-detected by the function, you can define the macro \c
   ImageMagick_convert_path with the correct path of the \c convert executable,
   before including Image.h in your program :
   \code
#define ImageMagick_convert_path "/home/cs/bin/convert"
#include "Image.h"

int main() {
Image img;
img.read("image.jpg");     // Read a JPEG image file.
return SUCCESS;
}
\endcode

If OpenCV installed, then OpenCV's image reader will be used.
 ***/

inline const char* convert_path() 
{
    static char * convert_path = 0;
    if ( !convert_path ) 
    {
        convert_path = new char[1024];

#ifdef ImageMagick_convert_path     
        FILE *_file = 0;	
        strcpy(convert_path,ImageMagick_convert_path);
        if (( _file = fopen( convert_path,"r"))!=0) 
        { 
            fclose( _file ); 
        }
#else
#if defined(__GNUC__) && !defined(WIN32) 
        FILE *_file = 0;
        strcpy( convert_path, "//usr//bin//convert");
        if (( _file = fopen( convert_path,"r"))!=0) 
        { 
            fclose( _file ); 
        }
#endif             // __GNUC__ WIN32
#endif             // ImageMagick_convert_path 
    }
    return convert_path;
}


///////////////////////////////////////////////////////////////////
/** 
 * @brief A color structure
 */
struct RGBColor
{
    unsigned int r;
    unsigned int g;
    unsigned int b;
};

//////////////////////////////////////////////////////////////////
class Point{
    public:
        Point();
        Point(int x, int y);
        void set(int x, int y);
        void get(int &x, int &y) const;
        Point& operator +=(const Point& p);
        Point& operator -=(const Point& p);
        Point& operator *=(int mag);
        Point& operator /=(int div);
        bool operator !=(const Point& p);
        bool operator ==(const Point& p);
        double distance(const Point& p)const;
        void dump_info();
        int x;
        int y;
};


///////////////////////////////////////////////////////////////////
/// A rectangle class
///
class Rect
{

    public:
        int x0;                    //upper left x
        int y0;                    //upper left y
        int x1;                    //bottom right x
        int y1;                    //bottom right y
        double cen_x;                        //centroid
        double cen_y;

        int width;
        int height;

        Rect ();
        Rect (unsigned int __x0, unsigned int __y0,  unsigned int __x1, unsigned int __y1 );
        ~Rect();
        void set( unsigned int __x0, unsigned int __y0, unsigned int __x1, unsigned int __y1 );
        
        Rect& move (const int x, const int y);
        void dump_info();
		bool CheckRange(int w, int h);

        Rect& operator= (const Rect &range);

        /// this operation scale the rectangle but keep its centre unchanged 
        Rect& operator* (const double scale);
        Rect& operator* (const int scale);
        Rect& operator/ (const double scale);
        Rect& operator/ (const int scale);


        bool operator== (const Rect &range);
        bool operator!= (const Rect &range);
        

    private:
        void calc_centre();
        /* 
         *   O---width----> X 
         *   |
         *  height
         *   |
         *   Y
         **/
        void calc_size();

};


////////////////////////////////////////////////////////////////
/**
 @brief
 An image class;
 So far only (unsigned char) 8-bit images supported
 i.e. Color images (24-bit) and 8-bit gray images
*/

class Image
{
    public:
        unsigned int width;
        unsigned int height;

        Image ();
        Image (unsigned int w, unsigned int h);
        Image (unsigned int w, unsigned int h, int set_a_colormode );
        ~Image ();
               
        bool set_size (unsigned int w, unsigned int h);

        bool set_value (u_char *ppm_data);
        bool clone (Image & img);
        
        void set_colormode(int _newmode){ colormode = _newmode; }
        int  get_colormode(void){ return colormode; }
        
        // return the image data
        u_char * get_data(void){ return data; }
        
    
        /// set pixel (x,y)'s value to pix_value
        bool set_pix ( unsigned int x, unsigned int y, u_char pix_value );
        bool set_pix ( unsigned int x, unsigned int y, const int channel, u_char pix_value );

		/// get pixel
		int get_pix (unsigned int x, unsigned int y, const int channel);

        /// convert a RGB image to gray
        bool togray( int new_colormode = GRAY_8B );

        bool read ( char* file_name );

        bool writepgm ( char *file_name, int mode = 0, unsigned int max_val_ = 255 );  // 0: binary mode; 1: ascii
        bool writeppm ( char *file_name, int mode = 0, unsigned int max_val_ = 255 );        
        bool draw_rect_ppm (char *file_name, Rect& region, const RGBColor&);

        bool readppm ( char* file_name );     
        bool readpgm ( char* file_name );
        
        bool is_empty();
        void dump_info();

#ifdef OPENCV_INSTALLED
        IplImage * toIplImage( );

        bool display( int disp_mode = 0, char * win_name="" );

        bool display( const Rect& rectangle, int disp_mode = 5, char * win_name="" );

        // an overloaded function
        bool set_value( IplImage * image );
#endif    
        /// for color images
        /// channel = 'r'/'g'/'b' 
        int operator () (unsigned int x, unsigned int y, const char channel);

        /// channel = 0/1/2, corresponding to 'r'/'g'/'b'. This version is
        /// a little faster than the above one
        int operator () (unsigned int x, unsigned int y, const int channel);

        /// for gray images
        int operator () (unsigned int x, unsigned int y);


    private:
        u_char *data;

    protected:
        int colormode;

};

////////////////////////////////////////////////////////////////
#ifdef OPENCV_INSTALLED
class Video
{
    public:              
        Video(char * vid_file);
        Video();
        ~Video();

        /// retrieve frames from the video. The acquired frame is stored in
        /// Image frame
        bool query();
        
        /// read a video from a file
        bool read(char * vid_file);

        /// Skip n_frames frames 
        void skip(unsigned int n_frames);
        
        /// Get the frame index where the first frame starts with an index of 0.
        int get_frame_pos();

        Image frame;
        
//    private:
        CvCapture  *video;
        IplImage   *iplframe;

};
#endif  // OPENCV_INSTALLED


//
//
//
#endif                                  /*__IMAGE_H__ */

// vim: tw=78 ts=4 sw=4 sts=0 ft=cpp enc=utf-8 ff=unix nowrap et
// EOF.
