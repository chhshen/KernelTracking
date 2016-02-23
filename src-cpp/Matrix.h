/*
 *   author           Chunhua Shen
 *   file             Matrix.h
 *   creation         21-Nov-2006 11:47.
 *   last revision    21-Nov-2006 13:41.
 */
#pragma once
#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <vector>

template <typename w_elem_type>
class Matrix
{
    typedef int                 t_Size;

    t_Size                      cols;
    t_Size                      rows;


    public:
    std::vector<w_elem_type>    data;

    Matrix( t_Size i_columns = 0, t_Size i_rows = 0 )
        : cols( i_columns ),
        rows( i_rows ),
        data( i_columns * i_rows )
    {

    }

    w_elem_type * operator[ ] ( t_Size i_index )
    {
        return & ( data [ i_index * rows ] );
    }

    template <typename w_Type, int w_columns, int w_rows>
        Matrix( const w_Type (&i_array)[w_columns][w_rows] )
        : cols( w_columns ),
        rows( w_rows ),
        data( & (i_array[0][0]), & (i_array[w_columns-1][w_rows]) )
    {

    }

};

//------------------------------------------------------
// An example:
// double  array[3][4] = 
// {
//       { 1.0, 2.0, 3.3, 4.4 },
//       { 1.0, 2.0, 3.3, 4.4 },
//       { 1.0, 2.0, 3.3, 4.5 },
// };
// 
// int main()
// {
//       matrix<float>       mat1( 3, 4 );
//       matrix<float>       mat2;
//       matrix<float>       mat3( array );
// 
//       mat2 = mat3;
// 
//       std::cout << mat2[2][3] << "\n";
// }
//-------------------------------------------------------

#endif /*__MATRIX_H__ */

