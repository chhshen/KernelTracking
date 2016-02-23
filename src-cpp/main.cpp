#include <Windows.h>
#include <iostream>


enum { 
	GeneralTracking, // generalize tracking using LBFGS and online SVM
	AnnTracking // traditional annealed meanshift. 
};


int annMain(int argc, char *argv[]);
int generalMain(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	
	int iType = GeneralTracking;//AnnTracking;AnnTracking;// Do not use annealed meanshift.... !!!!!
	switch (iType) {
        case GeneralTracking:
            return generalMain(argc, argv);
			break;

		case AnnTracking: 
			return annMain(argc, argv);
			break;						 

        default:
            fprintf (stderr, "- Error. \n");
            return 0;
			break;
   	}
	printf("\n\n");
}
