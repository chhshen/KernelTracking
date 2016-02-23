/*
 *   author           Chunhua Shen
 *   file             plainmeanshift.h
 *   created          26-Sep-2006 14:54.
 *   last revision    27-Nov-2006 16:07.
 */

#pragma once
#ifndef __PLAINMEANSHIFT_H__
#define __PLAINMEANSHIFT_H__

#include "meanshift.h"
#include "cxcore.h" 
#include "cxcore.hpp"

/////////////////////////////// parameter setting /////////// 
#define N_BINS_PER_CH       16

// 4096 = 16x16x16
#define N_BINS              4096 

// Convergence criterion 
// equivalent to TOLERANCE = 0.2
// we adopt the value used in C. Shen ICCV'05 
#define TOLERANCE_SQU       0.04

#define MAX_ITER            50
/////////////////////////////////////////////////////////////


/**
 * @breif a standard mean shift class
 */

class PlainMeanshift:public Meanshift      // derived class 
{
    public:
        PlainMeanshift(void);
        virtual ~PlainMeanshift(void);
        
        virtual void init(Rect * subregion, CvImage * img);
        virtual void init(Rect * subregion, CvImage * img, int kernel_type_ );
       
        virtual int run( Rect * subregion, CvImage * cur_frame );      

        void set_bandwidth(double _hx, double _hy); 
        void get_bandwidth( Bandwidth & b);
        
        void set_bincache(bool yesno_, CvImage * cur_frame);

        bool save( char * filename, char * image_name );
        
        Hist * ref_hist;
		Hist * p_temp;
       
        // the returned target region    
        Rect target_region;
       
		int clear(CvImage * img ); // with different image size
		bool m_bIsGeneralized;
        // bandwidth
        Bandwidth bandwidth;

    protected:
        int num_bin_b;
        int num_bin_g;
        int num_bin_r;
        

        // what type of kernel for tracking
        int Tracking_Kernel;

        virtual unsigned int get_bin_index  (double b, double g, double r);

        // build kernel weighted histogram
        virtual bool kernel_hist( Rect * subregion, CvImage * img, Hist * kh );
        virtual Rect calc_estimate(Rect * subregion, CvImage * cur_frame);


        double norm2(Rect * subregion1, Rect * subregion2);

      
        // Revised 20-11-2006, Chunhua Shen
        // store the index of histogram of each pixel of the current
        // image, such that it avoids redundant get_bin_index
        // calls        
        int * bin_index_cache;
        bool use_bin_index_cache;
		int m_iWidth;
		int m_iHeight;
};


#endif /*__PLAINMEANSHIFT_H__ */

// vim: tw=70 ts=4 sw=4 sts=0 ft=cpp enc=utf-8 ff=unix nowrap et 
// EOF.
