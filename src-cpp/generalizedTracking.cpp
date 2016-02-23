/*
 *   author           Junae Kim
 *   file             generalizedTracking.cpp
 *   created          2007

 *   ParseInput - Load Video/Image_sequences - Tracking - Save
 */

#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "defs.h"
#include "mouseselect.h"
#include "utils.h"
#include "KernelBasedTrack.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

enum { IMAGE, AVI, CAM };
typedef struct STR_param {
	int update; // use online update
	int type;	// image, avi, cam
	int kernel; // SVM kernel type
}Sparam;

//#define UpdateOnlineSVM 1 /// for using online update
#define WindowsImageReverse 1 ///avi output for windows

//////////////////////////////////////////////
/// global variable

/// tracker
CKernelTracking g_tracker;
CvCapture* g_capture = NULL;

/// previous object location
Point g_prev_point;

//////////////////////////////////////////////
Bandwidth* calcLayers(CvImage* cur_frame, int & iLayers);
double Tracking(CvImage* cur_frame, Sparam param, int iLayers, Bandwidth * band);
Sparam check_options(int argc, char *argv[]);
CvImage* load_image(char* name, int iCount, int itype);
Bandwidth* init(char* name, Sparam param, int & iLayers);
void help(char* exec_name);

//////////////////////////////////////////////
int generalMain(int argc, char *argv[])
{
	////////////////////////////////////////
	int iPosNum, iNegNum;
	int iCount = 0;
	int iLayerCount = 0;
	Sparam param = check_options(argc, argv);
	double dTime = 0;
	Bandwidth* band = NULL;
	int iLayerNum = 1;
	g_tracker.m_bIsGeneralized = true;
	CvImage *cur_frame=NULL;
	///////////////////////////////////////

	cvNamedWindow ("tracking", 1);
	FILE* pFile = fopen("output_our.dat", "wt");
	band = init(argv[argc-1], param, iLayerNum);

	// tracking
	do {
		// load image
		cur_frame = load_image(argv[argc-1], iCount, param.type);
		if (!cur_frame) break;

		// tracking
		dTime += Tracking(cur_frame, param, iLayerNum, band);


		// draw to the screen
		IplImage* img = cvCreateImage (cvSize (cur_frame->width(), cur_frame->height()), IPL_DEPTH_8U, 3);
		memcpy (img->imageData, cur_frame->data(), sizeof (u_char) * cur_frame->height() * cur_frame->width() * 3);
		cvRectangle (img, cvPoint (g_tracker.target_region.x0-1, g_tracker.target_region.y0-1),
			cvPoint (g_tracker.target_region.x1+1, g_tracker.target_region.y1+1), CV_RGB (0, 255, 0), 1);
		cvShowImage ("tracking", img);
		cvWaitKey (5);

		// save result region to txt file
		g_tracker.save(pFile);
		// save result to image
		export_frame ("output//general", img, iCount);
		cvReleaseImage(&img);

		// set previous position for next iteration
		g_prev_point.set((int)g_tracker.target_region.cen_x, (int)g_tracker.target_region.cen_y);

		iCount++;
	} while (1);

	fprintf(stdout, "\naverage running time : %.4lf\n", dTime/(double)iCount);
	cvDestroyWindow ("tracking");
	fclose(pFile);

	printf("Press enter to quit.\n");
	getchar( );

	bool bmemoryleak = _CrtDumpMemoryLeaks();
	return SUCCESS;

}



///Calculate number of layers and bandwidth
///@param cur_frame		source image
///@param iLayer		number of layers
///@return				bandwidth
Bandwidth* calcLayers(CvImage* cur_frame, int& iLayers)
{
	double rates = 1.2;
	iLayers = 1;
	float tempW = (float)(g_tracker.object_size_w);
	float tempH = (float)(g_tracker.object_size_h);
	while(iLayers < 3) {
		tempW *= rates;
		tempH *= rates;
		if ((tempW > cur_frame->width()) || (tempH > cur_frame->height()))	break;
		iLayers++;
	};
	printf("\nTotal number of layers : %d\n", iLayers);

	// calculate bandwidth
	Bandwidth* band = NULL;
	band = new Bandwidth[ iLayers ];
	band[ iLayers - 1 ].hx = g_tracker.object_size_w / 2;
	band[ iLayers - 1 ].hy = g_tracker.object_size_h / 2;
	for (int i = iLayers - 2; i >= 0; i--) {
		band[ i ].hx = ((int)( rates * band[ i+1 ].hx ) / 2 * 2);
		band[ i ].hy = ((int)( rates * band[ i+1 ].hy ) / 2 * 2);
	}
	return band;
}


/// tracking
///@param cur_frame			source image
///@param param
///@param iLayers			the number of layers
///@param band				bandwidth
///@return					running time
double Tracking(CvImage* cur_frame, Sparam param, int iLayers, Bandwidth * band)
{
	// initialize
	g_tracker.set_bincache(true, cur_frame);

	// start stopwatch
	CMyTime* myTime = new CMyTime();
	myTime->Tic();

	int iIndex = 0;
	for (int i = 0; i < iLayers ; i++)
	{
		Rect rect1 = setRect((int)g_prev_point.x, (int)g_prev_point.y, (int)band[i].hx, (int)band[i].hy, cur_frame);
		g_tracker.set_bandwidth(band[i].hx, band[i].hy);

		//////////////////////////////////////////////////
		int iSign = g_tracker.run( &rect1, cur_frame );
		//////////////////////////////////////////////////

//		if (iSign > 0)
		// if iSign < 0, detection with band[i] is failed.
		// In this case, We should reduce the bandwidth and start again.
		{
			g_prev_point.x = (int) g_tracker.target_region.cen_x;
			g_prev_point.y = (int) g_tracker.target_region.cen_y;
		}

	}
	if (param.update)
		g_tracker.updateSVM(g_tracker.target_region, cur_frame);

	// end stopwatch
	double dTime = myTime->Toc();
	delete myTime;

	return dTime;
}


///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////

void help(char* exec_name)
{
	fprintf(stderr, "Usuage:\n %s -u [is update?] -t [source type] -k [kernel type] [source files]\n", exec_name);
	fprintf(stderr, "[source type] 0: images, 1: a video clip, 2: CAM\n");
	fprintf(stderr, "[kernel type] 0: kernel_Epan, 1: kernel_TrunGaussian, 2: kernel_Gaussian, 3: kernel_Linear\n");
	fprintf(stderr, "In case of images, [source files] means image directory\n");
	fprintf(stderr, "For example,\n%s -u 1 -t 0 woman_sequence\n", exec_name);
	fprintf(stderr, "or \n%s -u 1 -t 1 woman.avi\n", exec_name);
	fprintf(stderr, "\"images\" option is assumed that there are 0.jpg, 1.jpg, 2.jpg and so on in image directory\n");
}

Sparam check_options(int argc, char *argv[])
{
	if (argc < 2)
    {
        fprintf(stderr, "- Bad command.\n");
		help(argv[0]);
		fprintf(stderr, "Press enter to quit.\n");
 		getchar( );
		exit(-1);
    }

	Sparam param;
	param.update = 1;
	param.type = IMAGE;
	param.kernel = kernel_Linear;

	for (int i=1; i<argc; i++) {
		char* temp = argv[i];
		switch (temp[0])
		{
		case '-':
			switch (temp[1]) {
			case 'u':
				param.update = atoi(argv[i+1]);
				i = i+1;
				break;
			case 't':
				param.type = atoi(argv[i+1]);
				i = i+1;
				break;
			case 'k':
				param.kernel = atoi(argv[i+1]);
				i = i+1;
				break;
			default:
				fprintf(stdout, "\nError: Unknown option was used.\n");
				help(argv[0]);
				fprintf(stderr, "Press enter to quit.\n");
				getchar( );
				exit(-1);
				break;
			}
			break;
		default:
			break;
		}
	}
	if (param.type != IMAGE && param.type != AVI && param.type != CAM) {
        fprintf(stderr, "- - Wrong input type. It must be 0(for images), 1(for a video file) or 2(for a Cam - not tested).\n");
		help(argv[0]);
		fprintf(stderr, "Press enter to quit.\n");
		getchar( );
		exit(-1);
	}
	return param;
}

/// initialization
///@param name		name of video or image directory
///@param param
///@param iLayers	the number of layers
///@return			bandwidth
Bandwidth* init(char* name, Sparam param, int& iLayers)
{
	CvImage* cur_frame;
	cur_frame = load_image(name, 0, param.type);
	g_tracker.init(cur_frame, param.kernel);

	IplImage* img = cvCreateImage (cvSize (cur_frame->width(), cur_frame->height()), IPL_DEPTH_8U, 3);
	memcpy (img->imageData, cur_frame->data(), sizeof (u_char) * cur_frame->height() * cur_frame->width() * 3);
	img->origin = 0;

	MouseSelect mouse;
	mouse.select_rect(img);
	Rect rect0 = mouse.rects[0];

//	Rect rect0(208, 118, 225, 183); // woman sequence of previous paper
//	Rect rect0(213, 136, 231, 170); // new woman which I selected
//	Rect rect0(91, 27, 115, 57); // dq sequence - turning around sample

	// make even number
	if (rect0.cen_x > (double)((int)(rect0.x0+rect0.x1)/2)) {
		rect0.set(rect0.x0, rect0.y0, rect0.x1+1, rect0.y1);
	}
	if (rect0.cen_y > (double)((int)(rect0.y0+rect0.y1)/2)) {
		rect0.set(rect0.x0, rect0.y0, rect0.x1, rect0.y1+1);
	}

	g_prev_point.x = (int)rect0.cen_x;
	g_prev_point.y = (int)rect0.cen_y;
	g_tracker.object_size_w = rect0.width; g_tracker.object_size_h = rect0.height;

	// update training sample
	g_tracker.updateSVM(rect0, cur_frame);

	// calculate the number of layers and bandwidth
	return calcLayers(cur_frame, iLayers);
}

/// load images from a video clip or a image sequence
///@param name		video clip name or directory name of a image sequence
///@param iCount	the index of video frame or image sequence
///@param itype		AVI for video, IMAGE for image sequence
///@return			return CvImage
CvImage* load_image(char* name, int iCount, int itype)
{
	CvImage* cur_frame;
	if(itype == AVI) {
		if (!g_capture) {
			g_capture = cvCaptureFromFile(name);
			if (!g_capture) {
				fprintf(stderr, "\nError - cvCaptureFromFile failed.\n");
				exit(-1);
			}
		}
		IplImage* image = 0;
		image = cvQueryFrame(g_capture);
		if (!image) {
			fprintf(stderr, "\nError - cvQueryFrame failed.\n");
			exit(-1);
		}
		cur_frame = new CvImage(image);
	}

	else if (itype == IMAGE) {
		char* filename = new char[512];
		sprintf(filename, "%s//%d.jpg", name, iCount);
		cur_frame = new CvImage();
		if (! cur_frame->load( filename))  {
			fprintf(stderr, "\nthe end of files or does not exist %s.\n", name);
			return NULL;
		}
		delete filename;
	}
	return cur_frame;
}


// EOF.
