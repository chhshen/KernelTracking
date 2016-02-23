/*
 *   author           Chunhua Shen
 *   file             mouseselect.cxx
 *   created          26-Sep-2006 16:43.
 *   last revision    15-Nov-2006 18:03.
 */

#include "mouseselect.h"

params     *g_mousep;
/* a point variable used to define the tracked region */
void MouseSelect::select_rect_withoutNewImage( IplImage * frame )
{
    obj_selected = 0;
    while (0 == obj_selected)
    {
        obj_selected = get_regions_withoutNewImage (frame);

        if ( obj_selected == 0)
            fprintf (stderr,
                "- You haven't selected any rectangles.\n");
    }
}


void MouseSelect::select_rect( CvImage * img )
{
    IplImage * frame;

    frame = (IplImage *)img;//.toIplImage();

    obj_selected = 0;
	while (0 == obj_selected)
	{
		obj_selected = get_regions (frame);

		if ( obj_selected == 0)
			fprintf (stderr,
				"- You haven't selected any rectangles.\n");
	}

}

void MouseSelect::select_rect( IplImage * img )
{
    IplImage * frame = img;

    obj_selected = 0;
	while (0 == obj_selected)
	{
		obj_selected = get_regions (frame);

		if ( obj_selected == 0)
			fprintf (stderr,
				"- You haven't selected any rectangles.\n");
	}

}

void MouseSelect::select_point( IplImage * frame )
{
    obj_selected = 0;
    while (0 == obj_selected)
    {
        obj_selected = get_points (frame);

        if ( obj_selected == 0 )
            fprintf (stderr, "- You haven't selected any points.\n");
    }
}


void MouseSelect::select_point( CvImage & img )
{
    IplImage * frame;

    frame = (IplImage *)img;//.toIplImage();img.toIplImage( );

    obj_selected = 0;
   
    while ( 0 == obj_selected )
    {
        obj_selected = get_points (frame);

        if ( obj_selected == 0 )
            fprintf (stderr, "- You haven't selected any points.\n");
    }
}


/**
   @brief allows the user to interactively select object regions.
   CvRect * regions, or Rect * regions, a pointer to an array to be filled
   with rectangles defining object regions
   @param frame the frame of video in which objects are selected
   @return Returns the number of objects selected by the user
 */
int
MouseSelect::get_regions (IplImage * frame)
{
    char       *win_name = "Init Frame";
    params      p;
    int         x1, y1, x2, y2, w, h;

    /* use mouse callback to allow user to define object regions */
    p.win_name = win_name;
    p.orig_img = (IplImage *) cvClone (frame);
    p.cur_img = NULL;
    p.n = 0;
    p.mode = 1;                         // p.mode = 1 to select rectangles
    // p.mode = 2 to select points

    cvNamedWindow (win_name, 1);
    cvShowImage (win_name, frame);

    g_mousep = &p;
    /* copy p to global p (g_mousep),
       this is used to select the tracked region interactively
     */
    int     mouse_param = 5;

    #ifdef OPENCV_VER_096
    cvSetMouseCallback (win_name, on_mouse);
    #else
    cvSetMouseCallback (win_name, on_mouse, &mouse_param);
    #endif
    cvWaitKey (0);                      /* waits for key event infinitely */

    cvDestroyWindow (win_name);
    cvReleaseImage (&(p.orig_img));

    if (p.cur_img)  cvReleaseImage (&(p.cur_img));

    /* extract regions defined by user; store as an array of rectangles */
    if (p.n == 0)
    {
        regions = NULL;
        rects   = NULL;
        return 0;
    }

    regions = new CvRect[ p.n ];

    rects = new Rect [p.n];

    for (int i = 0; i < p.n; i++)
    {
        x1 = MIN (p.loc1[i].x, p.loc2[i].x);
        x2 = MAX (p.loc1[i].x, p.loc2[i].x);
        y1 = MIN (p.loc1[i].y, p.loc2[i].y);
        y2 = MAX (p.loc1[i].y, p.loc2[i].y);
        w = x2 - x1;
        h = y2 - y1;

        /* ensure odd width and height */
        w = (w % 2) ? w : w + 1;
        h = (h % 2) ? h : h + 1;
        regions[i] = cvRect (x1, y1, w, h);

        // Rect
        rects[i].set (  x1, y1, x2, y2 );

    }

    return p.n;
}

int
MouseSelect::get_regions_withoutNewImage (IplImage * frame)
{
    char       *win_name = "Input";
    params      p;
    int         x1, y1, x2, y2, w, h;

    /* use mouse callback to allow user to define object regions */
    p.win_name = win_name;
    p.orig_img = (IplImage *) cvClone (frame);
    p.cur_img = NULL;
    p.n = 0;
    p.mode = 1;                         // p.mode = 1 to select rectangles
    // p.mode = 2 to select points

//    cvNamedWindow (win_name, 1);
  //  cvShowImage (win_name, frame);

    g_mousep = &p;
    /* copy p to global p (g_mousep),
       this is used to select the tracked region interactively
     */
    int     mouse_param = 5;

    #ifdef OPENCV_VER_096
    cvSetMouseCallback (win_name, on_mouse);
    #else
    cvSetMouseCallback (win_name, on_mouse, &mouse_param);
    #endif
    cvWaitKey (0);                      /* waits for key event infinitely */

 //   cvDestroyWindow (win_name);
 //   cvReleaseImage (&(p.orig_img));

    if (p.cur_img)  cvReleaseImage (&(p.cur_img));

    /* extract regions defined by user; store as an array of rectangles */
    if (p.n == 0)
    {
        regions = NULL;
        rects   = NULL;
        return 0;
    }

    regions = new CvRect[ p.n ];

    rects = new Rect [p.n];

    for (int i = 0; i < p.n; i++)
    {
        x1 = MIN (p.loc1[i].x, p.loc2[i].x);
        x2 = MAX (p.loc1[i].x, p.loc2[i].x);
        y1 = MIN (p.loc1[i].y, p.loc2[i].y);
        y2 = MAX (p.loc1[i].y, p.loc2[i].y);
        w = x2 - x1;
        h = y2 - y1;

        /* ensure odd width and height */
        w = (w % 2) ? w : w + 1;
        h = (h % 2) ? h : h + 1;
        regions[i] = cvRect (x1, y1, w, h);

        // Rect
        rects[i].set (  x1, y1, x2, y2 );

    }

    return p.n;
}

/**
   @brief Allows the user to interactively select points/pixels.
   @param frame the frame of video in which points are selected
   @return Returns the number of objects selected by the user
 */
int
MouseSelect::get_points (IplImage * frame)
{
    char       *win_name = "Init Frame";
    params      p;

    /* use mouse callback to allow user to define object regions */
    p.win_name = win_name;
    p.orig_img = (IplImage *) cvClone (frame);
    p.cur_img = NULL;
    p.n = 0;

    p.mode = 2;                         // p.mode = 1 to select rectangles
    // p.mode = 2 to select points

    cvNamedWindow (win_name, 1);
    cvShowImage (win_name, frame);

    g_mousep = &p;
    /* copy p to global p (g_mousep),
       this is used to select the tracked region interactively
     */
    int mouse_param = 5;

    #ifdef OPENCV_VER_096
    cvSetMouseCallback (win_name, on_mouse);
    #else
    cvSetMouseCallback (win_name, on_mouse, &mouse_param);
    #endif
    cvWaitKey (0);                      /* waits for key event infinitely */

    cvDestroyWindow (win_name);
    cvReleaseImage (&(p.orig_img));

    if (p.cur_img)
        cvReleaseImage (&(p.cur_img));

    /* extract regions defined by user; store as an array of rectangles */
    if ( 0 == p.n )
    {
        points = NULL;
        return 0;
    }

    points = new Point[ p.n ];
    positions = new CvPoint[ p.n ];

    for (int i = 0; i < p.n; i++)
    {
        // CvImage class'
        points[i].set ( p.loc2[i].x, p.loc2[i].y );

        // OpenCV's
        positions[i].x = p.loc2[i].x;
        positions[i].y = p.loc2[i].y;

    }
    return p.n;
}


/*
   Mouse callback function that allows user to specify the initial object
   regions.  Parameters are as specified in OpenCV documentation.
 */
#ifdef OPENCV_VER_096
void on_mouse (int event, int x, int y, int flags)
#else
void on_mouse (int event, int x, int y, int flags, void *param)
#endif
{

    CvPoint    *loc;
    int         n;
    IplImage   *tmp;
    static int  pressed = FALSE;
    
    int line_thick = 1;

    /* on left button press, remember first corner of rectangle around object */
    if (event == CV_EVENT_LBUTTONDOWN)
    {
        n = g_mousep->n;
        if (n == MAX_OBJECTS)
            return;

        loc = g_mousep->loc1;
        loc[n].x = x;
        loc[n].y = y;
        pressed = TRUE;
    }

    /* on left button up, finalise the rectangle and draw it in black */
    /* 1 == mode for selecting rectangles */
    else if ( 1 == g_mousep->mode && event == CV_EVENT_LBUTTONUP)
    {
        n = g_mousep->n;
        if (n == MAX_OBJECTS)
            return;
        loc = g_mousep->loc2;
        loc[n].x = x;
        loc[n].y = y;

        cvReleaseImage (&(g_mousep->cur_img));
        g_mousep->cur_img = NULL;

        cvRectangle (g_mousep->orig_img, g_mousep->loc1[n],
            loc[n], 
            CV_RGB (0, 0, 255),
            line_thick, 8, 0);
        
        cvShowImage (g_mousep->win_name, g_mousep->orig_img);
        pressed = FALSE;
        g_mousep->n++;

        /* message */
        // fprintf (stderr, "- Close this window to start\n");
    }

    /* on left button up, finalise the point and draw a point */
    /* 2 == mode for selecting points */
    else if ( 2 == g_mousep->mode && event == CV_EVENT_LBUTTONUP)
    {
        n = g_mousep->n;
        if (n == MAX_OBJECTS)
            return;

        loc = g_mousep->loc2;
        loc[n].x = x;
        loc[n].y = y;

        cvReleaseImage (&(g_mousep->cur_img));
        g_mousep->cur_img = NULL;

        int radius = 3;

        cvCircle( g_mousep->orig_img, loc[n], radius , CV_RGB (0, 255, 0), -1 );
        cvCircle( g_mousep->orig_img, loc[n], radius + 1, CV_RGB (0, 0 , 0),  2 );

        cvShowImage (g_mousep->win_name, g_mousep->orig_img);
        pressed = FALSE;
        g_mousep->n++;
    }

    /* on mouse move with left button down, draw rectangle as defined in Green */
    /* 1 == mode for selecting rectangles */
    else if ( 1 == g_mousep->mode && event == CV_EVENT_MOUSEMOVE 
            && pressed && CV_EVENT_FLAG_LBUTTON )
    {
        n = g_mousep->n;
        if (n == MAX_OBJECTS)
            return;

        tmp = (IplImage *) cvClone (g_mousep->orig_img);
        loc = g_mousep->loc1;
        
        cvRectangle ( tmp, loc[n], cvPoint (x, y), CV_RGB (0, 255, 0), line_thick, 8, 0 );
        
        cvShowImage (g_mousep->win_name, tmp);
        if (g_mousep->cur_img)
            cvReleaseImage (&(g_mousep->cur_img));
        g_mousep->cur_img = tmp;
    }
}


// vim: tw=78 ts=4 sw=4 sts=0 ft=cpp enc=utf-8 ff=unix nowrap et
// EOF.
