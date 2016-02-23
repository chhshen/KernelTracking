/*
 *   author           Chunhua Shen
 *   file             pmstracker.cxx
 *   created          26-Sep-2006 14:19.
 *   last revision    20-Nov-2006 12:19.
 */

/** 
 * @file
 * annealed mean shift localiser instance. \n
 * See pmstracker.cxx for an annealed mean shift tracker instance.\n
 * Reference: \n
 * Fast global kernel density mode seeking with application to localisation
 * and tracking, Chunhua Shen, Michael J. Brooks, Anton van den Hengel, IEEE
 * International Conference on Computer Vision ICCV'05, Beijing, China, October, 2005.
 */
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "defs.h"
#include "plainmeanshift.h"
#include "mouseselect.h"
#include "utils.h"
#include "KernelBasedTrack.h"

using namespace std;


int annMain(int argc, char *argv[])
{

    if (argc < 2)
    {
        cout<<"- Bad command."<<endl;
        cout<<"Usuage: " << argv[0] <<"[kernel_type=0 or 1]"<< " image_file" <<" "<<endl;
        cout<<"        kernel_type = 0 (Epan kernel) or 1 (Truncated Gaussian kernel, default)"<<endl;
        cout<<"        In terms of computational complexity, Epan kernel is faster."<<endl;
        exit(-1);
    }

	int iType = atoi(argv[1]);
	if (iType == 1) {
		MouseSelect mouse;
		FILE* pFile = NULL;

		CvImage* cur_frame = 0;


		// meanshift using LBFGS
//		CKernelTracking tracker;
//		tracker.m_bIsGeneralized = false;

		// traditional mean shift
		PlainMeanshift tracker; 

		CvCapture* capture = cvCaptureFromFile(argv[2]);// "photo.avi" //cvCaptureFromCAM(0);
		if (!capture) {
			fprintf(stderr, "\nError - cvCaptureFromFile failed.\n");
			return -1;
		}

		int kernel_type =  kernel_TrunGaussian;

		IplImage* image = 0;
		image = cvQueryFrame(capture);
		if (!image) {
			printf("\nEnd of clip\n");
			exit(1);
		}
		image->origin = 0;
		mouse.select_rect(image);

		cur_frame = new CvImage(image);
//		cvReleaseImage(&image);
				

		// whether annealed mean shift is used or not
		bool annealed;
		annealed = 0;

//		Rect rect0(208, 118, 225, 183); // woman sequence
		Rect rect0(59, 23, 87, 63); // seq_cubicle
//		Rect rect0 = mouse.rects[0];
		Rect result_rect = rect0;
		// initialise the tracker
		tracker.init(&rect0, cur_frame, kernel_type);

		Rect rect1;
		int layers = 3;

		char fileno[64]="";

		int k = 0;
		cvNamedWindow ("Input", 1);	    
		int iIndex = 0; // for debug
		pFile = fopen("output_annealed.dat", "w");
		while (1)
		{
			image = cvQueryFrame(capture);
			if (!image) {
				printf("\nEnd of clip\n");
				break;
			}
			cur_frame = new CvImage(image);
//			cvReleaseImage(&image);
			k++;
			// run the tracker
			tracker.set_bincache(TRUE, cur_frame); 
			if ( ! annealed )
			{
				tracker.run( &rect0, cur_frame ); 
				rect0 = tracker.target_region;
			}
			else        // annealed
			{    
				for ( int i = layers; i >0 ; i-- )
				{   
					if ( i == layers )
						rect1 = rect0 * pow (1.5, i-1 );
					else
						rect1 = rect0/1.5;

					printf("\nlayers = %d\n", i);
					tracker.run( &rect1, cur_frame );
					if ((tracker.target_region.height < 3) || (tracker.target_region.width < 3))
					{
						printf("Error! a target object is out of range or can't be detected.\nPress enter to quit...  ");
						getchar( );
						return SUCCESS;
					}
					rect0 = tracker.target_region;
				}
			}
			iIndex++;

			printf("\n--------------------------------------\n");
			printf("\ndetected region center : %.2f, %.2f\n", tracker.target_region.cen_x, tracker.target_region.cen_y);
			printf("\n--------------------------------------\n");

			fprintf(pFile, "%.2f, %.2f\n", tracker.target_region.cen_x, tracker.target_region.cen_y);
			IplImage* img = cvCreateImage (cvSize (cur_frame->width(), cur_frame->height()), IPL_DEPTH_8U, 3);
			memcpy (img->imageData, cur_frame->data(), sizeof (u_char) * cur_frame->height() * cur_frame->width() * 3);
			cvRectangle (img, cvPoint (tracker.target_region.x0-1, tracker.target_region.y0-1),
				cvPoint (tracker.target_region.x1+1, tracker.target_region.y1+1), CV_RGB (0, 255, 0), 1);
			cvShowImage ("Input", img);
			cvWaitKey (5);
			if (annealed)
				export_frame("output/annealmeanshift", img, iIndex);
			else
				export_frame("output/meanshift", img, iIndex);
			cvReleaseImage(&img);
		}

		cvDestroyWindow ("Input");
		fclose(pFile);
		printf("Press enter to continue.\n");
		getchar( );
	}
	else if (iType == 2) {
		// meanshift using LBFGS
//		CKernelTracking tracker;
//		tracker.m_bIsGeneralized = false;

		// traditional mean shift
	    PlainMeanshift tracker;
		tracker.m_bIsGeneralized = false;
		MouseSelect * mouse = new MouseSelect;
		CvImage img; 

		if (! img.load( argv[2] ))  exit(-1);

		int kernel_type = kernel_TrunGaussian;
		if ( argc > 2 )
		{
			kernel_type = atoi(argv[1]);

			if (kernel_type != 0 && kernel_type != 1)
			{
				cerr<<"- Wrong kernel type. It must be either 0 or 1"<<endl;
				exit(-2);
			}
		}

		// start
		mouse->select_rect( img );

		Rect rect0 = mouse->rects[0];

		// initialise the tracker
		tracker.init(&rect0, &img, kernel_type);

		mouse->select_point( img );
		Point point0 = mouse->points[0];

		delete mouse;

		// how many layers to anneal/ annealing schedule
		int layers = 6;

		Bandwidth * band = new Bandwidth[ layers ];

		band[ layers - 1 ].hx = rect0.width * 0.5;
		for (int i = layers - 2; i >= 0; i--)
			band[ i ].hx = (int)( 1.35 * band[ i+1 ].hx );

		char result_file[] = "_ams.dat";
		if ( file_exists( result_file ) )
			remove( result_file );
	 
		CMyTime* myTime = new CMyTime();
		myTime->Tic();


		// use cache to avoid redundant histogram bin index calculation
		tracker.set_bincache(TRUE, &img);

		for (int i = 0; i < layers ; i++)
		{
			band[i].hy = band[i].hx * (double)rect0.height/(double)rect0.width;

			Rect rect1( (int)(point0.x - band[i].hx),
					(int)(point0.y - band[i].hy),
					(int)(point0.x + band[i].hx),
					(int)(point0.y + band[i].hy) );

			tracker.run( &rect1, &img );

			rect0 = tracker.target_region;
			point0.x = (int) rect0.cen_x;
			point0.y = (int) rect0.cen_y;

			tracker.save(result_file, argv[2] );
		}
		myTime->Toc();
		delete myTime;

		show_result( result_file );
 		cout<<"Press enter to continue."<<endl;
		getchar( );
	}


    return SUCCESS;
}


// vim: tw=78 ts=4 sw=4 sts=0 ft=cpp enc=utf-8 ff=unix nowrap et
// EOF.
