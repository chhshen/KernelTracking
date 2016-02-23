/*
  *   author           Chunhua Shen 
  *   file
  *   creation         02-Apr-2006 16:34.
  *   last revision    27-Nov-2006 09:44.
  */

#ifndef DEFS_H
#define DEFS_H

#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <cassert>

//#ifndef WIN32
//#include <unistd.h>
/*
snprintf is replaced by _snprintf in windows
*/
//#define snprintf _snprintf 
//#endif

// #ifndef byte 
// #define byte unsigned char
// #endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef MIN
#define MIN(x,y) ( ( ( x ) < ( y ) )? ( x ) : ( y ) )
#endif

#ifndef MAX
#define MAX(x,y) ( ( ( x ) > ( y ) )? ( x ) : ( y ) )
#endif

#ifndef ABS
#define ABS(x) ( ( ( x ) < 0 )? -( x ) : ( x ) )
#endif

#ifndef SQR
#define SQR(x) ((x)*(x))
#endif

#ifndef SUCCESS
#define SUCCESS 1
#endif

#ifndef FAILURE
#define FAILURE 0
#endif

/*
 * For debugging
 */
#ifdef _DEBUG

#if !defined(TraceInfo)
#define TraceInfo fprintf(stderr, "-- DEBUG TRACE: %s (line %i): %s()     at %s.\n",\
      __FILE__, __LINE__, __FUNCTION__, __TIME__)
#endif

#if !defined(debug)
#define  debug(x)  std::cout <<"- DEBUG PRINT: "<<__FILE__<<" (line "<<\
                           __LINE__ << ") " << "\n" <<"  "<<  \
                           #x << " = " << (x) << "\n\n"
#endif

#else

#if !defined(TraceInfo)
#define TraceInfo           ((void)0)
#endif

#if !defined(debug)
#define debug(_Expression)  ((void)0)
#endif

#endif

// this definition will be defined by a switch -DOPENCV_INSTALLED in the
// Makefile if the configure script detects OPENCV in the system.
// 
// Fast access of the IplImage pixel value 
//
#ifdef OPENCV_INSTALLED

#define CV_GRAY(IMAGE, X, Y)  \
   ((unsigned char*)((IMAGE)->imageData  + (IMAGE)->widthStep*(Y)))[ (X)     ]

#define CV_RED(IMAGE, X, Y)   \
   ((unsigned char*)((IMAGE)->imageData + (IMAGE)->widthStep*(Y))) [ (X)*3+2 ]

#define CV_GREEN(IMAGE, X, Y) \
   ((unsigned char*)((IMAGE)->imageData + (IMAGE)->widthStep*(Y))) [ (X)*3+1 ]

#define CV_BLUE(IMAGE, X, Y)  \
   ((unsigned char*)((IMAGE)->imageData + (IMAGE)->widthStep*(Y))) [ (X)*3   ]

#endif

// 
// Fast access of the CvImage Class pixel value 
//
// CvImage img;
// WIDTH = img.width;
// IMAGE = img.get_data();
//
#define IM_GRAY(IMAGE, X, Y, WIDTH)  ((unsigned char)( IMAGE[ (X) + (Y) * (WIDTH) ] ))

#define IM_RED(IMAGE, X, Y, WIDTH)   ((unsigned char)( IMAGE[ (X)*3 +     (Y)*(WIDTH)*3 ] ))
#define IM_GREEN(IMAGE, X, Y, WIDTH) ((unsigned char)( IMAGE[ (X)*3 + 1 + (Y)*(WIDTH)*3 ] ))
#define IM_BLUE(IMAGE, X, Y, WIDTH)  ((unsigned char)( IMAGE[ (X)*3 + 2 + (Y)*(WIDTH)*3 ] ))



// Fast access of the IplImage pixel value
// access uint8 colour image
#define CVGRAY_8U(IMAGE, X, Y)   ((unsigned char*)((IMAGE)->imageData  + (IMAGE)->widthStep*(Y)))[ (X)     ]
#define CVR_24U(IMAGE, X, Y)    ((unsigned char*)((IMAGE)->imageData + (IMAGE)->widthStep*(Y))) [ (X)*3+2 ]
#define CVG_24U(IMAGE, X, Y)  ((unsigned char*)((IMAGE)->imageData + (IMAGE)->widthStep*(Y))) [ (X)*3+1 ]
#define CVB_24U(IMAGE, X, Y)  ((unsigned char*)((IMAGE)->imageData + (IMAGE)->widthStep*(Y))) [ (X)*3   ]


#endif    /* defs.h */
