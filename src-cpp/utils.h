/*
 *   author           Chunhua Shen
 *   file             utils.h
 *   created          22-Sep-2006 01:45.
 *   last revision    15-Feb-2007 14:03.
 */
#pragma once
#ifndef __UTILS_H__
#define __UTILS_H__
// modified by junae

//#include <sys/time.h> // for linux
#include <Windows.h> // for windows

#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>  
#include <iostream>
#include <iomanip>
#include <fstream>
class Rect;

///@brief a stopwatch. calculate time cost
class CMyTime
{
public:
	CMyTime() { m_dTime = 0; };

	/// start a stop watch
	void Tic();

	/// end a stop watch
	double Toc();

private:
	double m_dTime;

};


/// return whether filename is exist or not
inline bool file_exists (char * filename)
{
    struct stat buf;
    int i = stat ( filename, &buf );
    if ( 0 == i  )
        return true;

    return false;       
}

/// show result
int show_result( char * filename );

///Exports a frame whose name is determined by dirName and i. 
///export image name is "./dirName/****.jpg".
///**** is indicated by i.
///@param dirName	output directory name.
///@param frame		frame to be exported
///@param i			frame number
int export_frame (char* dirName, IplImage * frame, int i);

///Set rectangle region inside of image size
///if part of rectangle lies out of image, move the rectangle to inside of image
///@param centX		center of rectangle
///@param centY		center of rectangle
///@param width_2	rectangle width / 2
///@param height_2	rectangle height / 2
///@param img		source image
///@return			setted rectangle
Rect setRect(int centX, int centY, int width_2, int height_2, CvImage* img);

/// make image name as "./dir/i.jpg"
///@param dir		dir name
///@param i			image name
///@return			image name
char* GetImageName(char* dir, int i);


#endif /*__UTILS_H__ */

// vim: tw=70 ts=4 sw=4 sts=0 ft=cpp enc=utf-8 ff=unix nowrap et 
// EOF.
