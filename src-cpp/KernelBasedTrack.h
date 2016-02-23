#pragma once
#ifndef __KERNALBASEDTRACK_H__
#define __KERNALBASEDTRACK_H__

#include "plainmeanshift.h"

struct svm_model;
class COnlineSVM;

///@breif a generalised kernal based visual tracking class
class CKernelTracking:public PlainMeanshift       
{
    public:
        CKernelTracking(void);
        virtual ~CKernelTracking(void);
        virtual int run( Rect * subregion, CvImage * cur_frame ); 

		///Save detected regions to "filename"
		///@param filename		output file name
		///@param strImageName	source image name
		///@return success or not
		bool save( char * filename, char * strImageName );
		bool save( FILE*);

		///Update Online SVM with rect0 region of a input image.
		///Update positive class using rect0, then update negative class using arbitrary selected rectangle.
		///@param rect0			target region which would be used to update SVM
		///@param cur_frame			source image
		bool updateSVM(Rect rect0, CvImage *cur_frame);

		///Update Online SVM with rect0 region of a input image.
		///@param rect0			target region which would be used to update SVM
		///@param cur_frame			source image
		///@param fYt			class index. If fYt == 1, rect0 is positive data, else rect0 is negative data
		bool updateSVM(Rect rect0, CvImage *cur_frame, float fYt);

		///Initialize tracker
		///@param cur_frame			source image
		///@param kernel_type_	kernel type at SVM
		void init(CvImage * cur_frame, int kernel_type_ );
		void init(Rect*, CvImage * cur_frame, int kernel_type_ );

		///Initialize SVM
		///This function is called when offline SVM training data is used.
		///Load "face.online.model" and Save to m_pOnline
		int initSupportModel( );    

		///Build kernel weighted histogram, then save square root value of histogram for SVM training
		///@param subregion		target region of calculating histogram 
		///@param cur_frame			source image
		///@param kh			historgam would be saved to kh
		virtual bool kernelHistforTrain(Rect * subregion, CvImage * cur_frame, Hist * kh);

		///Return kernel weight value
		///if m_bIsGeneralized is false then return sqrt(ref_hist)
		///@param bin			bin index of histogram
		///@return				kernel weight value
		double dCalcSupportWeight ( int bin );

		///Calculate similarity value(decision function value) and first derivate of target candidate
		///save first derivate to member variables(m_dDeriv[]) and save similarity value to m_dF
		///@param subregion		target region 
		///@param cur_frame		source image
		///@return				similarity value(result of equation 10)
		double dCalcEstimate( Rect * subregion, CvImage * cur_frame);

         ///The returned target region    
        Rect target_region;

		///first derivate 
		double m_dDeriv[2];

		///decision function value
		double m_dF;

		double m_dPrev[2];

		/// target region size
		int object_size_w;
		int object_size_h;


		CvImage * m_cur_frame;


	protected:

		///SVM pointer
		COnlineSVM *m_pOnline;

};

///L-BFGS Callback interface to provide objective function and gradient evaluations. 
///from C port of Limited-memory Broyden-Fletcher-Goldfarb-Shanno (L-BFGS) - http://www.chokkan.org/software/liblbfgs/
///@param instance 	The user data sent for lbfgs() function by the client.
///@param x 		The current values of variables.
///@param g			The gradient vector. The callback function must compute the gradient values for the current variables.
///@param n			The number of variables.
///@param step	 	The current step of the line search routine.
///@return			The value of the objective function for the current variables
static double evaluate(void *instance, const double *x, double *g, const int n, const double step);

///L-BFGS Callback interface to receive the progress of the optimization process. 
///in this program, nothing happened in this function
///@param instance 	The user data sent for lbfgs() function by the client.
///@param x 		The current values of variables.
///@param g 		The current gradient values of variables.
///@param fx 		The current value of the objective function.
///@param xnorm 	The Euclidean norm of the variables.
///@param gnorm 	The Euclidean norm of the gradients.
///@param step		The line-search step used for this iteration.
///@param n 		The number of variables.
///@param k 		The iteration count.
///@param ls		The number of evaluations called for this iteration.
///@return			Zero to continue the optimization process. Returning a non-zero value will cancel the optimization process.
static int progress(void *instance, const double *x, const double *g, const double fx, const double xnorm, const double gnorm, const double step, int n, int k, int ls);

///global variable of tracker
extern CKernelTracking g_tracker;

#endif //__KERNALBASEDTRACK_H__