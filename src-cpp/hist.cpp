/*
 *   author           Chunhua Shen
 *   file             hist.cxx
 *   created          26-Sep-2006 16:08.
 *   last revision    27-Nov-2006 18:25.
 */

#include "hist.h"
#include <stdlib.h>

void Hist::set_size (unsigned int len)
{
    assert(len>0);
    length = len;
	if (data)
		delete data;
	data = (float *)malloc(sizeof(float)*len);
//	data = new double[len];
    memset ( data, 0, sizeof (float) * length );
}

void Hist::set_zeros ()
{
    memset ( data, 0, sizeof (float) * length);
}

void Hist::clear ()
{
    memset ( data, 0, sizeof (float) * length);
}

bool Hist::save (char *file_name)
{
    
    FILE *pFile;
    pFile = fopen (file_name, "wt");
    if (pFile == NULL)
    {
        fprintf ( stderr, "Error - Cannot create file %s.\n", file_name );
        return FAILURE;
    }

    for (unsigned int i = 0; i < length; i++)
        fprintf (pFile, "%d    %f\n", i, data[i]);

    if ( !fclose (pFile))
        return FAILURE;

    return SUCCESS;
}

// save histogram to online svm data
bool Hist::save (FILE *pFile, double dIndex)
{    
    if (pFile == NULL)
    {
        fprintf ( stderr, "Error - Cannot save histogram file.\n");
        return FAILURE;
    }

	fprintf(pFile, "%.1f", dIndex);
	for (unsigned int i = 0; i < length; i++) {
		if (data[i] == 0.0) continue;
		fprintf (pFile, " %d:%f", i+1, data[i]);
	}
	fprintf(pFile, "\n");

    return SUCCESS;
}

// modified by junae. return value is added.
double Hist::normalise ()
{
    float sum = 0.0f;
    for (unsigned int i = 0; i < length; i++)
    {
        sum += data[i];
    }

    if (sum < 1e-8)
    {
        fprintf( stderr, 
                "- Warning. \
                The histogram might be empty or contain negative values.\n");
        return FAILURE;
    }
    else { 
        for (unsigned int i = 0; i < length; i++)
            data[i] = data[i] / sum;
    }
    
    return (double)sum;//SUCCESS;
}


bool Hist::is_normalised ( )
{ 
    float sum = 0.0f;
    for (unsigned int i = 0; i < length; i++)
    {
        sum += data[i];
    }
  
    if ( ABS(sum -1.0f ) > 1e-6 )
        return FALSE;
    else
        return TRUE;
}

bool Hist::is_nonnegative ( )
{ 
    for (unsigned int i = 0; i < length; i++)
    {
        if ( data[i] < 0.f )
            return FALSE;
    }

    return TRUE;
}


bool Hist::is_zero ( )
{ 
    for (unsigned int i = 0; i < length; i++)
    {
        if ( ABS(data[i]) > 1e-10 )
            return FALSE;
    }

    return TRUE;
}


// vim: tw=78 ts=4 sw=4 sts=0 ft=cpp enc=utf-8 ff=unix nowrap et 
// EOF
