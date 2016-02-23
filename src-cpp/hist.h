/*
 *   author           Chunhua Shen {http://users.rsise.anu.edu.au/~cs/}
 *   file             Hist.h
 *   created          02-Aug-2006 10:05.
 *   last revision    27-Nov-2006 18:42.
 */
//
// A class for construct a 1D histogram
//

#pragma once
#ifndef __HIST_H__
#define __HIST_H__

#include <string.h>
#include <stdio.h>
#include <cassert>

#ifndef byte 
#define byte unsigned char
#endif

#ifndef FAILURE
#define FAILURE     0
#endif

#ifndef SUCCESS
#define SUCCESS 1
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef ABS
#define ABS(x) ( ( ( x ) < 0 ) ? -( x ) : ( x ) )
#endif

#ifdef _DEBUG
#if !defined(TraceInfo)
#define TraceInfo fprintf(stderr, "-- DEBUG TRACE: %s (line %i): %s()     at %s.\n",\
      __FILE__, __LINE__, __FUNCTION__, __TIME__)
#endif
#endif

class Hist
{
    public:
        unsigned int length;
        float *data;

        Hist (void)
        {
            data = NULL;    
        }

        Hist ( unsigned int len_ )
        {
            assert( len_>0 );
            length = len_;
            data = new float[ length ];
            memset ( data, 0, sizeof (float) * length );
        }

        virtual ~Hist (void)
        {
            if ( data != NULL )
                delete [] data;
        }

       
        void set_size(unsigned int len);
        void clear( );
        void set_zeros( );
        bool save (char *file_name);
		bool save (FILE *pFile, double dIndex);


		// modified by junae. return value is added.
		double normalise();
//      bool normalise( );

        bool is_normalised ( );
        bool is_nonnegative( );
        bool is_zero ( );
};

#endif         // __HIST_H__
