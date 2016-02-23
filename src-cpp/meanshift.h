/*
 *   author           Chunhua Shen {http://users.rsise.anu.edu.au/~cs/}
 *   file             meanshift.h
 *   created          31-Aug-2006 12:16.
 *   last revision    02-Nov-2006 11:46.
 */

#pragma once
#ifndef __MEANSHIFT_H__
#define __MEANSHIFT_H__

#include "Image.h"
#include "hist.h"
#include "defs.h"


#ifdef _DEBUG
#include <iostream>
#endif

#ifdef kPI
#undef kPI
#endif


#define kernel_Epan           0
#define kernel_TrunGaussian   1 
#define kernel_Gaussian       2
#define kernel_Linear	      3

#define DEFAULT_KERNEL        kernel_Gaussian


const double kPI = 3.14159265358979;

struct Bandwidth
{
    // Note this is 1/2 length
    double hx;
    double hy;

    // Not used yet
    double hx0;
    double hy0;
};


class Meanshift
{
    public:
        Meanshift(void);
        virtual ~Meanshift(void);
       
    private: 

    protected:
        double kernel_profile( double x, int kernel_type = DEFAULT_KERNEL );    
        double profile_derivative( double x, int kernel_type = DEFAULT_KERNEL );
        double Bhatt_dist (const Hist * hist1, const Hist * hist2);

};      // end of Meanshift class

#endif  /* __MEANSHIFT_H__ */

// vim: tw=78 ts=4 sw=4 sts=0 ft=cpp enc=utf-8 ff=unix nowrap et 
// EOF.
