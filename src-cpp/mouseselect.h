/*
 *   author           Chunhua Shen {http://users.rsise.anu.edu.au/~cs/}
 *   file             mouseselect.h
 *   created          01-Aug-2006 07:20.
 *   last revision    14-Nov-2006 11:34.
 */
//
// A class which allows you to select points or rectangle regions
// interactively. This is an OpenCV wrapper. Test on opencv ver 0.96 & 0.97
//

#pragma once
#ifndef __MOUSESELECT_H__
#define __MOUSESELECT_H__

#include "Image.h"

/* OpenCV library */
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "cvaux.h"



#define MAX_OBJECTS        10

// #define OPENCV_VER_096
// comment this if your OpenCV's version isn't 0.9.6
// In debian, run `pkg-config --modversion opencv` to get version

#define OPENCV_VER_097

typedef struct params
{
    CvPoint     loc1[MAX_OBJECTS];
    CvPoint     loc2[MAX_OBJECTS];
    IplImage   *objects[MAX_OBJECTS];
    char       *win_name;
    IplImage   *orig_img;
    IplImage   *cur_img;
    int         n;
    unsigned int mode;                    // mode == 1, select rectangles
                                          // mode == 2, select points
} params;

#ifdef OPENCV_VER_096
void on_mouse (int event, int x, int y, int flags);
#else                                   // OPENCV_VER_097
void on_mouse (int event, int x, int y, int flags, void *param);
#endif


class MouseSelect
{
    public:
        MouseSelect( )
        {
            regions = NULL;
            rects = NULL;
            points = NULL;
            positions = NULL;
        }

        ~MouseSelect( )
        {
            if ( regions != NULL )
                delete [] regions;
           
            if ( rects != NULL )
                delete [] rects;
           
            if ( points != NULL )
                delete [] points;
           
            if ( positions != NULL )
                delete [] positions;
        }
        
        /// select a rectangle
        void  select_rect ( IplImage * frame);
        void  select_rect_withoutNewImage ( IplImage * frame);

        /// overload to accept CvImage format
        void  select_rect ( CvImage * frame ); 
        
        /// select a point
        void  select_point ( IplImage * frame);

        /// overload to accept CvImage format
        void  select_point ( CvImage & frame ); 

        /// return opencv rectangle CvRect
        CvRect *   regions;

        /// return CvImage rectangle Rect
        Rect * rects;
        
        Point * points;
        CvPoint * positions;

        /// Note that 'rects' and 'regions' are equivalent
        /// 'points' and 'positions' are equivalent
        /// I also put OpenCV variables here in case that one needs use
        /// OpenCV's image structure, rather than my CvImage class.
        /// I might remove them someday. 

        unsigned int obj_selected;      /// number of objects the user selected

    private:
        int     get_regions (IplImage * frame);
		int		get_regions_withoutNewImage(IplImage * frame);
        int     get_points (IplImage * frame);
};

#endif                                  //__RECTANGLESELECT_H__

// vim: tw=78 ts=4 sw=4 sts=0 ft=cpp enc=utf-8 ff=unix nowrap et
// EOF.
