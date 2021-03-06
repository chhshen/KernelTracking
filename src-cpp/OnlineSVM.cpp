#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include "OnlineSVM.h"
#include "svm.h"
#include <windows.h>

//////////////////////////////////////////////////////////////////
SData* CTrainList::GetAt(int i) 
{
	SData* pCurr = m_pFirst;
	int index;
	for (index=0; index<i; index++) {
		if (pCurr == NULL) 
			break;
		pCurr = pCurr->m_pNext;
	}
	return pCurr;
}

int CTrainList::RemoveAt(int i) {
	SData* pCurr = m_pFirst;
	int index;
	for (index=0; index<i; index++) {
		if (!pCurr) 
			return -1;
		pCurr = pCurr->m_pNext;
	}
	if (!pCurr)
		return -1;
	m_iLen--;
	if (m_iLen == 0) { 
		m_pFirst = NULL;
		m_pLast = NULL;
	}
	else {
		if (m_pFirst == pCurr) 
			m_pFirst = pCurr->m_pNext;
		if (m_pLast == pCurr) 
			m_pLast = pCurr->m_pPrev;
	}

	SData* pTemp = pCurr->m_pPrev;
	if (pTemp) 
		pTemp->m_pNext = pCurr->m_pNext;

	pTemp = pCurr->m_pNext;
	if (pTemp) 
		pTemp->m_pPrev = pCurr->m_pPrev;

	delete pCurr;

	return 1;
}

int CTrainList::Add(SData p) {

	SData* pTemp = new SData();
	{
		pTemp->m_iDataLen = p.m_iDataLen;
		for (int i=0; i<p.m_iDataLen; i++) {
			pTemp->m_iIndex[i] = p.m_iIndex[i];
			pTemp->m_fData[i] = p.m_fData[i];
		}
		pTemp->m_fYt = p.m_fYt; 
		pTemp->m_fAlpha = p.m_fAlpha;
		pTemp->m_mcsp_tau[0] = p.m_mcsp_tau[0];
		pTemp->m_mcsp_tau[1] = p.m_mcsp_tau[1];
	}

	if (!m_pFirst)
		m_pFirst = pTemp;
	if (!m_pLast) {
		m_pLast = pTemp;
	}
	else {
		m_pLast->m_pNext = pTemp;
		pTemp->m_pPrev = m_pLast;
		m_pLast = pTemp;
	}
	m_iLen++;
	return 1;
}

//////////////////////////////////////////////////////////


const char *svmTypeTable[] =
{
	"c_svc","nu_svc","one_class","epsilon_svr","nu_svr",NULL
};

const char *kernelTypeTable[]=
{
	"linear","polynomial","rbf","sigmoid","precomputed",NULL
};

COnlineSVM::COnlineSVM(void)
{
	m_iKernel = 0;
	m_fGamma = 0.00840336f;
	m_iDegree = 9;
	m_fCoef0 = 0.0f;
	m_fRho = 1.0f;
	m_fBeta_t = 0.0f;
	memset(m_fSVM, 0, sizeof(float)*ConstiDataLen);

}

COnlineSVM::~COnlineSVM(void)
{
	if (m_ImgHeader.m_iLen > 0) {
		for (int i=0; i<m_ImgHeader.m_iLen; ) {
			m_ImgHeader.RemoveAt(0);
		}
	}
	if (m_TrainHeader.m_iLen > 0) {
		for (int i=0; i<m_TrainHeader.m_iLen; ) {
			m_TrainHeader.RemoveAt(0);
		}
	}
}


void COnlineSVM::SaveSVMModel(char* strOutputFile)
{
	printf("\nSave result to libSVM file. - %s", strOutputFile);
	FILE *fp = fopen(strOutputFile,"w");
	if(fp==NULL) {
		printf("\nerror - svm file can't be saved.\n");
		return;
	}

	fprintf(fp,"svm_type %s\n", svmTypeTable[0]);
	fprintf(fp,"kernel_type %s\n", kernelTypeTable[m_iKernel]);

	if(m_iKernel == 1) // poly
		fprintf(fp,"degree %d\n", m_iDegree);

	if(m_iKernel == 1 || m_iKernel == 2 || m_iKernel == 3)
		fprintf(fp,"gamma %g\n", m_fGamma);

	if(m_iKernel == 1 || m_iKernel == 3)
		fprintf(fp,"coef0 %g\n", m_fCoef0);

	int nr_class = 2;
	int l = m_TrainHeader.m_iLen;
	fprintf(fp, "nr_class %d\n", nr_class);
	fprintf(fp, "total_sv %d\n",l);
	
	{
		fprintf(fp, "rho");
		for(int i=0;i<nr_class*(nr_class-1)/2;i++)
			fprintf(fp," %g",m_fBeta_t);
		fprintf(fp, "\n");
	}
	
//	(model->label)
	{
		fprintf(fp, "label");
		fprintf(fp, " 1 -1\n");
	}


//	(model->nSV)
	{
		fprintf(fp, "nr_sv");
		for(int i=nr_class-1;i>=0;i--)
			fprintf(fp," %d",m_i_nSV[i]);
		fprintf(fp, "\n");
	}

	fprintf(fp, "SV\n");

	SData *pChain;
	for(int i=0;i<l;i++)
	{
		pChain = m_TrainHeader.GetAt(i);
		fprintf(fp, "%.16g ",pChain->m_fAlpha);

		int ll = pChain->m_iDataLen;
		for (int j=0; j<ll; j++) {
			float p = pChain->m_fData[j];
			int pi = pChain->m_iIndex[j];
			fprintf(fp,"%d:%.8g ",pi+1,p);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}

bool COnlineSVM::OnlineTraining()
{
	if (m_ImgHeader.m_iLen == 0)
		return false;

	double dFlag = 1;
	SData* pTest;
	m_i_nSV[0] = m_i_nSV[1] = 0;
	m_fRho = 0.1f;
	m_fBeta_t = 0.0f;

	printf("\nTraining .");
	for (int i=0; i<m_ImgHeader.m_iLen;) {
		pTest = m_ImgHeader.GetAt(i);
		dFlag = SubOnlineTraining(*pTest);
		m_ImgHeader.RemoveAt(i); // for debug
		if ((m_ImgHeader.m_iLen)%10==0)printf(".");
	}

	printf("\nKernel = %s", kernelTypeTable[m_iKernel]);
	if (m_iKernel != 0)
		printf("\ngamma = %f, degree = %d, coef0 = %f", m_fGamma, m_iDegree, m_fCoef0);
	printf("\nrho = %f", m_fBeta_t);
	printf("\nnSV = %d, nBSV = %d", m_i_nSV[1], m_i_nSV[0]);
	printf("\nTotal nSV = %d", m_i_nSV[1] + m_i_nSV[0]);
	printf("\n");
	return true;
}

bool COnlineSVM::OnlineTraining(float* fData, float yt)
{
	float dFlag = 1.f;
	SData pTest;
	int i, j;

	m_fRho = 0.1f;
	m_fBeta_t = 0.0f;

	printf("\nTraining");
	if (yt > 0)
		printf(" positive data..\n");
	else
		printf(" negative data..\n");

	j = 0;
	// copy values to SData structure
	pTest.m_fYt = yt;
	pTest.m_fAlpha = yt;
	pTest.m_mcsp_tau[0] = pTest.m_mcsp_tau[1] = 0;
	for (i=0; i<ConstiDataLen; i++) {
		if (fData[i] > 0.f) {
			pTest.m_iIndex[j] = i;
			pTest.m_fData[j++] = fData[i];
			pTest.m_iDataLen++;
		}
	}
	// online SVM update
	float fFlag = SubOnlineTraining(pTest);

	// if pTest is saved to SVM list, update reference bin value
	if (fFlag != 0.0f)
		UpdateBinValue(pTest);

	return true;
}



float COnlineSVM::SubOnlineTraining(SData& pTest)
{ /// use MIRA (for Margin Infused Relaxed Algorithm) 	

	int iLen = m_TrainHeader.m_iLen;

	if (iLen < 1) {
		float temp = 1.f;

		pTest.m_fAlpha = temp * pTest.m_fYt;
		pTest.m_mcsp_tau[0] = pTest.m_mcsp_tau[1] = 0;
		if (pTest.m_fYt < 0) {
			pTest.m_mcsp_tau[1] = temp;
			pTest.m_mcsp_tau[0] = -temp;
			m_i_nSV[1]++;
		}
		else {
			m_i_nSV[0]++;
			pTest.m_mcsp_tau[1] = -temp;
			pTest.m_mcsp_tau[0] = temp;
		}

		m_TrainHeader.Add(pTest);
		return pTest.m_fAlpha;//1.0f
	}

	float alpha_t = 0;
	const int numClass = 2;
	int sps_i, r;
	int class_errors = 0;
	float redopt_def_a;
	float redopt_def_b[2];

	int i;
	float kernel_values;
	redopt_def_b[0] = redopt_def_b[1] = 0;

	for (i=0; i<iLen; i++) {
		SData* pTrain = m_TrainHeader.GetAt(i);
		kernel_values = Kernel(pTest, *pTrain);
		for (int r=0; r<numClass; r++) {
			redopt_def_b[r] += pTrain->m_mcsp_tau[r] * kernel_values;
		}
	}
	redopt_def_a = Kernel(pTest, pTest);
	if (pTest.m_fYt < 0)
		sps_i = 1;
	else sps_i = 0;
	redopt_def_b[sps_i] -= 0.1f; // beta

	struct vector{
		int index;
		float value;
	};
	vector vector_d[2];
	int mistake_k=0;
	float sum_d = 0;

	for (r=0; r<2; r++) {
		if (redopt_def_b[r] > redopt_def_b[sps_i]) {
			vector_d[mistake_k].index = r;
			vector_d[mistake_k].value = ((redopt_def_b[r]) / redopt_def_a);
			sum_d += vector_d[mistake_k].value;
			mistake_k++;
		}
		else {
			pTest.m_mcsp_tau[r] = 0;
		}
	}
	if (mistake_k == 0)
		return 0;

	vector_d[mistake_k].index = sps_i;
	vector_d[mistake_k].value = ((redopt_def_b[sps_i]) / redopt_def_a);
	
	float temp = 0.5f * (vector_d[0].value - vector_d[1].value);
	temp = (temp < 1.0f) ? temp : 1;
	pTest.m_mcsp_tau[vector_d[0].index] = -temp;
	pTest.m_mcsp_tau[vector_d[1].index] = temp;
	pTest.m_fAlpha = temp * pTest.m_fYt;
	if (pTest.m_fYt < 0)
		m_i_nSV[1]++;
	else
		m_i_nSV[0]++;

	m_TrainHeader.Add(pTest);
	return pTest.m_fAlpha;//1.0f

}


void COnlineSVM::LoadSVM(char* strFile)
{
	printf("\nLoad Input SVM file. - %s\n", strFile);

	FILE *fp;

	fp = fopen(strFile, "rb");
	if(fp==NULL) {
		printf("\nerror : Input file don't exist.\n");
		return;
	}
	
	int svm_type, nr_class, model_l;
	double *rho, *probA, *probB; // never be used in this program...
	double **sv_coef;
	int *label, *nSV;

	char cmd[81];
	while(1)
	{
		fscanf(fp,"%80s",cmd);
		if(strcmp(cmd,"svm_type")==0)
		{
			fscanf(fp,"%80s",cmd);
			int i;
			for(i=0;svmTypeTable[i];i++)
			{
				if(strcmp(svmTypeTable[i],cmd)==0)
				{
					svm_type=i;
					break;
				}
			}
			if(svmTypeTable[i] == NULL)
			{
				fprintf(stderr,"unknown svm type.\n");
				return;
			}
		}
		else if(strcmp(cmd,"kernel_type")==0)
		{		
			fscanf(fp,"%80s",cmd);
			int i;
			for(i=0;kernelTypeTable[i];i++)
			{
				if(strcmp(kernelTypeTable[i],cmd)==0)
				{
					m_iKernel=i;
					break;
				}
			}
			if(kernelTypeTable[i] == NULL)
			{
				fprintf(stderr,"unknown kernel function.\n");
				return;
			}
		}
		else if(strcmp(cmd,"degree")==0)
			fscanf(fp,"%d",&m_iDegree);
		else if(strcmp(cmd,"gamma")==0) {
			double dTemp;
			fscanf(fp,"%lf",&dTemp);
			m_fGamma = (float)dTemp;
		}
		else if(strcmp(cmd,"coef0")==0) {
			double dTemp;
			fscanf(fp,"%lf",&dTemp);
			m_fCoef0 = (float)dTemp;
		}
		else if(strcmp(cmd,"nr_class")==0)
			fscanf(fp,"%d",&nr_class);
		else if(strcmp(cmd,"total_sv")==0)
			fscanf(fp,"%d",&model_l);
		else if(strcmp(cmd,"rho")==0)
		{
			int n = nr_class * (nr_class-1)/2;
			rho = new double[n];
			for(int i=0;i<n;i++)
				fscanf(fp,"%lf",&rho[i]);
		}
		else if(strcmp(cmd,"label")==0)
		{
			int n = nr_class;
			label = new int[n];
			for(int i=0;i<n;i++)
				fscanf(fp,"%d",&label[i]);
		}
		else if(strcmp(cmd,"probA")==0)
		{
			int n = nr_class * (nr_class-1)/2;
			probA = new double[n];
			for(int i=0;i<n;i++)
				fscanf(fp,"%lf",&probA[i]);
		}
		else if(strcmp(cmd,"probB")==0)
		{
			int n = nr_class * (nr_class-1)/2;
			probB = new double[n];
			for(int i=0;i<n;i++)
				fscanf(fp,"%lf",&probB[i]);
		}
		else if(strcmp(cmd,"nr_sv")==0)
		{
			int n = nr_class;
			nSV = new int[n];
			for(int i=0;i<n;i++)
				fscanf(fp,"%d",&nSV[i]);
			m_i_nSV[0] = nSV[0];
			m_i_nSV[1] = nSV[1];
		}
		else if(strcmp(cmd,"SV")==0)
		{
			while(1)
			{
				int c = getc(fp);
				if(c==EOF || c=='\n') break;	
			}
			break;
		}
		else
		{
			fprintf(stderr,"unknown text in model file: [%s]\n",cmd);
			free(rho);
			free(label);
			free(nSV);
			return;
		}
	}

	// read sv_coef and SV
	model_l++;
	int* elements = new int[model_l];
	long pos = ftell(fp);

	int model_index=0;
	elements[model_index] = 0;
	while(1)
	{
		int c = fgetc(fp);
		switch(c)
		{
			case '\n':
				model_index++;
				elements[model_index] = 0;
				break;
			case ':':
				++elements[model_index];
				break;
			case EOF:
				goto out;
			default:
				;
		}
	}
out:
	fseek(fp,pos,SEEK_SET);
	if (elements[model_l-1] == 0)
		model_l--;

	int m = nr_class - 1;
	int l = model_l;
	sv_coef = new double*[m];
	int i;
	for(i=0;i<m;i++)
		sv_coef[i] = new double[l];

	int j = 0;
	for(i=0;i<l;i++)
	{
		SData SVMData;
		for(int k=0;k<m;k++)
			fscanf(fp,"%lf",&sv_coef[k][i]);
		SVMData.m_fYt = (float)sv_coef[0][i];
		SVMData.m_fAlpha = (float)sv_coef[0][i];
		SVMData.m_mcsp_tau[0] = SVMData.m_fAlpha;
		SVMData.m_mcsp_tau[1] = -SVMData.m_fAlpha;


		double dTemp;
		while(1)
		{
			int c;
			do {
				c = getc(fp);
				if(c=='\n') goto out2;
			} while(isspace(c));
			ungetc(c,fp);
			fscanf(fp,"%d:%lf",&(SVMData.m_iIndex[j]),&dTemp);
			SVMData.m_iIndex[j]--;
			SVMData.m_fData[j++] = (float)dTemp;
		}
out2:
		SVMData.m_iDataLen = j;
		m_TrainHeader.Add(SVMData);
		j = 0;
	}

	if (elements)
		delete elements;
	for(i=0;i<m;i++)
		delete sv_coef[i];
	delete sv_coef;
	fclose(fp);
}


void COnlineSVM::Load(char* strInputFile)
{
	printf("\nLoad Input file. - %s\n", strInputFile);
	FILE *fp = fopen(strInputFile,"rb");
	if(fp==NULL) {
		printf("\nerror - input file don't exist.\n");
		return;
	}
	long pos = ftell(fp);

	int model_l = 0;
	while(1)
	{
		int c = fgetc(fp);
		switch(c)
		{
			case '\n':
				model_l++;
				break;
			case ':':
				break;
			case EOF:
				goto out0;
			default:
				;
		}
	}
out0:
	fseek(fp,pos,SEEK_SET);
	model_l++; // if last lane ends EOF instead of \n....

	int* elements = new int[model_l];
	for (int z=0; z<model_l; z++) elements[z] = 0;

	int model_index=0;
	elements[model_index] = 0;
	while(1)
	{
		int c = fgetc(fp);
		switch(c)
		{
			case '\n':
				model_index++;
				break;
			case ':':
				++elements[model_index];
				break;
			case EOF:
				goto out;
			default:
				;
		}
	}
out:
	fseek(fp,pos,SEEK_SET);
	if (elements[model_l-1] == 0)
		model_l--;

	int l = model_l;
	int j=0;
	for(int i=0;i<l;i++)
	{
		SData SVMData;
		double dTemp;
		fscanf(fp,"%lf",&(dTemp));
		SVMData.m_fYt = SVMData.m_fAlpha = (float)dTemp;
		SVMData.m_mcsp_tau[0] = SVMData.m_fAlpha;
		SVMData.m_mcsp_tau[1] = -SVMData.m_fAlpha;
		SVMData.m_iDataLen = elements[i];

		for (j = 0; j<elements[i]; j++) {
			fscanf(fp,"%d:%lf",&(SVMData.m_iIndex[j]), &dTemp);
			SVMData.m_fData[j] = (float)dTemp;
			SVMData.m_iIndex[j]--;
		}
		printf("m_ImgHeader.Add(&pChain)\n");
		m_ImgHeader.Add(SVMData);
	}
	fclose(fp);


}


float dot_prod( float dataA, float dataB )
{
	float c = 0;
	c = dataA * dataB;
	return( c );
}

float sub_dot_prod( float dataA, float dataB )
{
	float c = 0;
	c = (dataA - dataB) * (dataA - dataB);
	return( c );
}

float COnlineSVM::fDot(SData pTrain, SData pTest )
{
	float sum = 0;
	int iT = pTrain.m_iDataLen;
	int jT = pTest.m_iDataLen;
	int i=0, j=0;
	while (i < iT && j < jT) 
	{
		if (pTrain.m_iIndex[i] == pTest.m_iIndex[j]) {
			sum += pTrain.m_fData[i] * pTest.m_fData[j];
			i++;
			j++;
		}
		else {
			if (pTrain.m_iIndex[i] > pTest.m_iIndex[j])
				j++;
			else
				i++;
		}
			
	}
	return sum;
}

float COnlineSVM::Kernel( SData pTrain, SData pTest )
{
	switch(m_iKernel)
	{
		case LINEAR:
			return fDot(pTrain, pTest);
		case POLY:
			return pow(m_fGamma*fDot(pTrain, pTest)+m_fCoef0,m_iDegree);
		case RBF:
		{
			float sum = 0;
			int iT = pTrain.m_iDataLen;
			int jT = pTest.m_iDataLen;
			int i=0, j=0;
			while (i < iT && j < jT) 
			{
				if (pTrain.m_iIndex[i] == pTest.m_iIndex[j]) {
					float d = pTrain.m_fData[i] - pTrain.m_fData[i]; 
					sum += d*d;
					i++;
					j++;
				}
				else
				{
					if(pTrain.m_iIndex[i] > pTest.m_iIndex[j]) 
					{	
						sum += pTrain.m_fData[i] * pTrain.m_fData[i];
						j++;
					}
					else
					{
						sum += pTrain.m_fData[i] * pTrain.m_fData[i];
						i++;
					}
				}
			}

			while(i < iT)
			{
				sum += pTrain.m_fData[i] * pTrain.m_fData[i];
				i++;
			}

			while(j < jT)
			{
				sum += pTrain.m_fData[i] * pTrain.m_fData[i];
				j++;
			}
			
			return exp(-m_fGamma*sum);
		}
		case SIGMOID:
			return tanh(m_fGamma*fDot(pTrain, pTest)+m_fCoef0);
		default:
			return 0;	// Unreachable 
	}
		return 0;
}

int COnlineSVM::UpdateBinValue(SData pTrain)
{
	for (int j=0; j<pTrain.m_iDataLen; j++) {
		m_fSVM[pTrain.m_iIndex[j]] += (pTrain.m_fAlpha*pTrain.m_fData[j]);
	}
	return 1;
}

int COnlineSVM::CalcBinValue()
{
	int i;
	int iNumSupportVec = m_TrainHeader.m_iLen;
	memset(m_fSVM, 0, sizeof(float)*ConstiDataLen);

	for (i=0; i<iNumSupportVec; i++) 
	{
		SData* pTemp = m_TrainHeader.GetAt(i);
		if (pTemp) {
			for (int j=0; j<pTemp->m_iDataLen; j++) {
				m_fSVM[pTemp->m_iIndex[j]] += (pTemp->m_fAlpha*pTemp->m_fData[j]);
			}
		}
	}
	return 1;
}

float COnlineSVM::GetBinValue(int bin)
{
	if (bin >= ConstiDataLen)
		return -1.f;
	else {
		return m_fSVM[bin];
	}
}
