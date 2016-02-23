/*
 *   author           Chunhua Shen
 *   file             
 *   created          04-Oct-2006 16:44.
 *   last revision    05-Nov-2006 00:14.
 */
#ifndef __LBFGS_H__
#define __LBFGS_H__

#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <cassert>

#ifndef byte 
#define byte unsigned char
#endif
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


extern "C" {
    extern void lbfgs(int* n, int* m, double* x, double* f, double* g, 
            int* diagco, double* diag, int* iprint, double* eps, 
            double* xtol, double* w, int* iflag);
}


class LBFGS {

    private:

        /// number of variables
        int         n;           

        /// the number of corrections used in the BFGS update.
        int         m;

        /// a DOUBLE PRECISION array of length N. If DIAGCO=TRUE,
        /// then on initial entry or on re-entry with IFLAG=2, DIAG
        /// it must be set by the user to contain the values of the 
        /// diagonal matrix Hk0.  Restriction: all elements of DIAG
        /// must be positive.
        double *    diag;

        /// a DOUBLE PRECISION array of length N(2M+1)+2M used as
        /// workspace for LBFGS. This array must not be altered by the
        /// user.
        double *    w;

        int         iflag;
        
        
        /// stop threshold
        double  eps;            ///eg. 1e-7                 

    public:
        // these valuables are fixed.
        explicit LBFGS(): n(0), m(5), iflag(0) 
    {
        w = NULL;
        diag = NULL;
    };

        virtual ~LBFGS() 
        {
            if (w)      delete [] w;
            if (diag)   delete [] diag;
        };


        bool init ( int _n, int _m, double _eps = 1e-7 )
        {
            n = _n;
            m = _m;
            
            // stop threshold
            eps = _eps;

            // w.resize (n * (2 * m + 1) + 2 * m);
            // diag.resize (n);

            w = new double [ n * (2 * m + 1) + 2 * m ];
            diag = new double [ n ];

            return SUCCESS;
        }

        int run (double *x, double *f, double *g)  // vector, object, gradient
        {
            int iprint[] = {-1, 0};                     //only print iteration count

            /*  XTOL    is a  positive DOUBLE PRECISION variable that must be set by
             *  the user to an estimate of the machine precision (e.g.
             *  10**(-16) on a SUN station 3/60). The line search routine will
             *  terminate if the relative width of the interval of uncertainty
             *  is less than XTOL.
             */
            double  xtol = 1e-10;


            /// a LOGICAL variable that must be set to TRUE, if the
            /// user  wishes to provide the diagonal matrix Hk0 at each
            /// iteration. Otherwise it should be set to FALSE
            int     diagco = 0;

            // call the actual function
            lbfgs(&n, &m, x, f, g, &diagco, &diag[0], iprint, &eps, &xtol, &w[0], &iflag);

            if (iflag < 0) 
            {
                fprintf(stderr, "- LBFGS optimisation stops with unexpected error.\n");
                return -1;
            } 

            /** IFLAG 
             * is an INTEGER variable that must be set to 0 on
             * initial entry to the subroutine. A return with IFLAG < 0
             * indicates an error, and IFLAG = 0 indicates that the routine has
             * terminated without detecting errors. On a return with IFLAG=1,
             * the user must evaluate the function F and gradient G. On a
             * return with IFLAG=2, the user must provide the diagonal matrix
             * Hk0. 
             */
            if (iflag == 0)     return 0;                      // terminate

            // iflag = 1,2 (> 0)
            return iflag;                                      // evaluate next f and g
        }
};


#endif
// vim: tw=78 ts=4 sw=4 sts=0 ft=cpp enc=utf-8 ff=unix nowrap et 
// EOF.
