#ifndef __ONLINESVM_H__
#define __ONLINESVM_H__

const int ConstiDataLen = 4096;

////////////////////////////////////////////////////////
/// data structure for SVM
struct SData
{
	/// data length of a support vector
	int m_iDataLen;
	/// index of non-zero data
	int m_iIndex[ConstiDataLen];
	/// value of non-zero related with m_iIndex
	float m_fData[ConstiDataLen];
	/// class index
	float m_fYt;
	/// weight value
	float m_fAlpha;
	/// for online training
	float m_mcsp_tau[2];
	/// pointer to previous support vector
	SData* m_pPrev;
	/// pointer to next support vector 
	SData* m_pNext;

	SData() {
		m_iDataLen = 0;
		m_fYt = m_fAlpha = m_mcsp_tau[0] = m_mcsp_tau[1] = 0.f;
		m_pPrev = m_pNext = NULL;
		memset ( m_fData, 0, sizeof (float) * ConstiDataLen );
		memset ( m_iIndex, 0, sizeof (int) * ConstiDataLen );
	};
};

////////////////////////////////////////////////////////
/// class for Support vector list
/// used to SVM training
class CTrainList
{
public:
	/// pointer to first support vector
	SData* m_pFirst;
	/// pointer to last support vector
	SData* m_pLast;

	int m_iLen;
	CTrainList() { m_pFirst = m_pLast = NULL;	m_iLen = 0; }
	SData* GetAt(int i);
	int Add(SData p);
	int RemoveAt(int i);
};

////////////////////////////////////////////////////////
/// Binary Online SVM class
/// using sequence
/// 1. Load - training data
/// 2. OnlineTraining
/// 3. Load - test data
/// 4. Test(int test_index)
/// or
/// 1. LoadSVM - offline trained data
/// 2. Load - test data
/// 3. Test
/// or for online SVM !!!!!!!
/// 1. Calculate histogram using kernelHistforTrain of CKernelTracking
/// 2. OnlineTraining
///////////////////////////////////////////////////////
class COnlineSVM
{
    public:
        COnlineSVM(void);
        virtual ~COnlineSVM(void);

		/// Load training data
		///@param strFile		training data file
		void Load(char* strFile);

		/// Load offline trained data of which format is libSVM
		///@param strFile		trained data file
		void LoadSVM(char* strFile);

		/// online train
		/// use MIRA (for Margin Infused Relaxed Algorithm) 
		///@param float*		support vector data list
		///@param float			class index ( 1 for positive, -1 for negative )
		bool OnlineTraining(float*, float);

		/// set SVM kernel type
		/// 0 : linear
		/// 1 : polynomial
		/// 2 : rbf
		/// 3 : sigmoid
		/// 4 : precomputed
		void SetKernelType(int i) { m_iKernel = i; }

		/// set gamma value for polynomial, rbf and sigmoid
		void SetGamma(float gamma) { m_fGamma = gamma; }

		/// calculate reference value of histogram bins
		int CalcBinValue();

		/// return reference value of bin index
		///@param int			bin index
		float GetBinValue(int);

		/// save current support vectors using libSVM's format
		///@param strOutputFile	name of output file
		void SaveSVMModel(char* strOutputFile);

		/// online training function
		bool OnlineTraining();


	protected:
		/// update bin value after online SVM update
		///@param pTemp			a support vector's value for training
		int UpdateBinValue(SData pTemp);

		/// calculate dot product of two data structure
		float fDot(SData pTrain, SData pTest );

		/// return value of kernel result according to kernel type
		float Kernel( SData pTrain, SData pTest );

		/// body of online update 
		/// one by one online training
		/// use MIRA (for Margin Infused Relaxed Algorithm) 
		///@return				alpha value
		float SubOnlineTraining(SData&); 

		/// image histogram data for training
		CTrainList m_ImgHeader; 

		/// trained data of SVM
		CTrainList m_TrainHeader; 

		/// kernel type
		/// 0 : linear, 1 : polynomial, 2 : rbf, 3 : sigmoid, 4 : precomputed
		int m_iKernel; 

		/// calculated reference bin value
		float m_fSVM[ConstiDataLen];

		/// gamma value for polynomial, rbf and sigmoid
		float m_fGamma;
		/// constant value for poly and sigmoid
		float m_fCoef0;
		/// constant value for poly
		int m_iDegree;
		/// soft margin parameter
		float m_fRho; 
		/// beta (is not used in this application)
		float m_fBeta_t;
		/// # of each classes. this application assumes to use two class SVM
		int m_i_nSV[2];

};


#endif //__ONLINESVM_H__