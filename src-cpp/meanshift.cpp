/*
 *   author           Chunhua Shen {http://users.rsise.anu.edu.au/~cs/}
 *   file             meanshift.cxx
 *   created          01-Sep-2006 14:29.
 *   last revision    19-Nov-2006 16:40.
 */

#include "meanshift.h"

Meanshift::Meanshift(void)
{

}


Meanshift::~Meanshift(void)
{

}


/*
    @param
    @return the value of the kernel profile
*/
double Meanshift::kernel_profile( double x, int kernel_type )
{

    
    ///
    /// The constant multiplied doesn't affect the histogram.
    /// You don't have to include it in order to save some computation.
    ///

    switch( kernel_type )
    {
        case kernel_Epan:               // Epanechnikov kernel used
            {
                if ( x < 1.0 )
                    return ( 1.0 - x );
                else
                    return 0.0;
            }
            break;
        case kernel_TrunGaussian:
            {
                if ( x < 1.0)
                    return ( exp( -0.5 * x ) );
                else
                    return 0.0;
            }
            break;
        case kernel_Gaussian:
            {
                return ( exp( -0.5 * x ) );
            }
            break;
        case kernel_Linear:
            {
                return ( x );
            }
            break;
			
        default:
            fprintf(stderr, "- No kernel type is defined!\n");
            return (-1);
    }
}


/*
   @param
   @return the derivative of the kernel profile
 */
double Meanshift::profile_derivative( double x, int kernel_type )
{ 

    switch (kernel_type)
    {
        case kernel_Epan:
            {
                if ( x < 1.0 )
                    return 1.0;             /// return any constant; the result will not be changed
                else
                    return 0.0;
            }
            break;
        case kernel_TrunGaussian:
            {
                if ( x < 1.0)
                    return ( exp( -0.5 * x ) );
                else
                    return 0.0;
            }
            break;
        case kernel_Gaussian:
            {
                return ( exp( -0.5 * x ) );
            }
            break;
        case kernel_Linear:
            {
                return 1.0;
            }
            break;

        default:
            fprintf(stderr, "- No kernel type is defined!\n");
            return (-1);
    }
}


/**@brief 
   Calculate the Bhattayya similarity/distance between two histograms
   1 means completely the same;
   0 means totally different
 */
double Meanshift::Bhatt_dist (const Hist * hist1, const Hist * hist2)
{
    assert ( hist1->length == hist2->length);
    double sum = 0.0;

    for (unsigned int i = 0; i < hist1->length; i++)
        sum += sqrt ( hist1->data[i] * hist2->data[i] );

    return sum;
}
