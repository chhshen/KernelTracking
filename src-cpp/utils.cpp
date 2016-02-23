#include "utils.h"
#include "image.h"


void CMyTime::Tic()
{
    m_dTime=GetTickCount() * .001;//cur_time();
}

double CMyTime::Toc()
{
	double temp = (GetTickCount() * .001) - m_dTime;
    fprintf(stderr, "\nElapsed time is %f second(s).\n", 
            temp );
	return temp;
}

int show_result( char * filename )
{  
    //
    // The format of the file is like this:
    // 158 109 109 344 %starbucks.jpg
    //
    int x0, y0, x1, y1;
    char str[256];
	bool bFinal = false;

    std::ifstream inFile;
    inFile.open(filename);

    if (!inFile) {
        std::cout << "- Unable to open file" << std::endl;
        exit(1); // terminate with error
    }

    char file_name[128]="";
    char file_name1[128]="";

    // show the image
    IplImage * image = NULL;

    bool same_image = true;

    while ( inFile >> x0 >> y0 >> x1 >> y1 >> str ) 
    {
        if ( 0 == ( strlen(str) ) )
            continue;

        strcpy(file_name1, file_name);
        strcpy(file_name,  str);// + 1  );       

        if ( 0 != strcmp(file_name1, file_name) ) //images are different
        {
            same_image = false;

			if (!image)
				image = cvLoadImage (file_name, -1);
            if (!image)
            {
                fprintf (stderr, "- Error. I could not load image file: %s\n",
                        file_name);
                exit(1);
            }
        }

		if ((x0 == -1) && (x1 == -1) && (y1 == -1) && (y0 == -1)) {
			// last one is final detected region
			bFinal = true;
			continue;
		}

		if (!bFinal) 
			cvRectangle (image, cvPoint (x0-1, y0-1),
                cvPoint (x1+1, y1+1), CV_RGB (0, 255, 0), 1); // final result is marked as red rectangle
		else
			cvRectangle (image, cvPoint (x0-1, y0-1),
                cvPoint (x1+1, y1+1), CV_RGB (255, 0, 0), 1);

        if ( ! same_image )             //images are different
        {
            cvNamedWindow("",1);
        }

        cvShowImage ("", image);
        cvWaitKey (5);                  /* This is important to maintain the window size */
        // sleep(0.5);
    }

    fprintf(stderr, "- Press Enter to continue.");
    getchar();

    cvReleaseImage(&image);
    cvDestroyWindow("");

    inFile.close();
	return 0;
}

/** 
 * @brief 
 * Purpose:    This is a function illustrating how to display more than one 
 * image in a single window using Intel OpenCV.
 * @param char *title: Title of the window to be displayed
 * @param int nArgs:   Number of images to be displayed
 * ...:         IplImage*, which contains the images
 *
 * This function can be called like this:
 * cvShowManyImages("Images", 2, img1, img2);
 * or
 * cvShowManyImages("Images", 5, img2, img2, img3, img4, img5);
 * 
 * This function can display upto 12 images in a single window.
 * It does not check whether the arguments are of type IplImage* or not.
 * The maximum window size is 700 by 660 pixels.
 * Does not display anything if the number of arguments is less than
 * one or greater than 12.
 */


int export_frame (char* dirName, IplImage * cur_frame, int i)
{
	  char        name[256];
      
      int export_status;

	  sprintf(name, "./%s/%04d.jpg", dirName, i);
      fprintf(stdout, "export frame #%d to %s \n", i , name);
      export_status = cvSaveImage (name, cur_frame);

      if ( ! export_status )
            fprintf(stderr, "- Error in saving image %s", name );
            
      return export_status;
}

Rect setRect(int centX, int centY, int width_2, int height_2, CvImage* cur_frame)
{
	// calculate range of windows
	int x0, x1, y0, y1;
	x0 = (int)(centX - width_2);	x1 = (int)(centX + width_2);
	y0 = (int)(centY - height_2);	y1 = (int)(centY + height_2);
	if (x0 < 0) {
		x0 = 0;		x1 = width_2*2;
		if (x1 >= cur_frame->width()) x1 = cur_frame->width() - 1;
	}
	else if (x1 >= cur_frame->width()) {
		x0 = cur_frame->width() - 2*width_2 - 1;		x1 = cur_frame->width() - 1;
		if (x0 < 0) x0 = 0;
	}
	if (y0 < 0) {
		y0 = 0;		y1 = height_2*2;
		if (y1 >= cur_frame->height()) y1 = cur_frame->height() - 1;
	}
	else if (y1 >= cur_frame->height()) {
		y0 = cur_frame->height() - 2*height_2 - 1;		y1 = cur_frame->height() - 1;
		if (y0 < 0) y0 = 0;
	}
	return Rect( x0, y0, x1, y1);
}

char* GetImageName(char* dir, int i)
{
	char* strTemp = new char[512];
	sprintf(strTemp, "./%s/04d.jpg", dir, i);
	return strTemp;
}