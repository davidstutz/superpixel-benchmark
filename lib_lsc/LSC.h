#ifndef LSCSUPERPIXEL
#define LSCSUPERPIXEL

#include"Initialize.h"
#include"Seeds.h"
#include"DoSuperpixel.h"
#include"point.h"
#include"myrgb2lab.h"
#include "countSuperpixel.h"
using namespace std;

//LSC superpixel segmentation algorithm

void LSC(unsigned char* R,unsigned char* G,unsigned char* B,int nRows,int nCols,int StepY,int StepX,double ratio,int iterationNum,int thresholdCoef,int color_space,unsigned short* label)
{
        int RowNum=nRows/StepY;
        int ColNum=nCols/StepX;
        int superpixelnum=RowNum*ColNum;    
        //Setting Parameter
	float colorCoefficient=20;
	float distCoefficient=colorCoefficient*ratio;
	int seedNum=superpixelnum;
//	int iterationNum=20;
//	int thresholdCoef=4;

	unsigned char *L, *a, *b;
        
        if(color_space>0)
        {
                L=new unsigned char[nRows*nCols];
                a=new unsigned char[nRows*nCols];
                b=new unsigned char[nRows*nCols];

                myrgb2lab(R,G,B,L,a,b,nRows,nCols);
        }
        else
        {
                L=R;
                a=G;
                b=B;
        }

	//Produce Seeds
//	int ColNum,RowNum;
//        int StepY,StepX;
//	ColNum=sqrt(float(seedNum*nCols/nRows));
//	RowNum=seedNum/ColNum;
//	StepX=nRows/RowNum;
//	StepY=nCols/ColNum;
	point *seedArray=new point[seedNum];
	int newSeedNum=Seeds(nRows,nCols,RowNum,ColNum,StepY,StepX,seedNum,seedArray);

	//Initialization
	float **L1,**L2,**a1,**a2,**b1,**b2,**x1,**x2,**y1,**y2;
	double **W;
	L1=new float*[nRows];
	L2=new float*[nRows];
	a1=new float*[nRows];
	a2=new float*[nRows];
	b1=new float*[nRows];
	b2=new float*[nRows];
	x1=new float*[nRows];
	x2=new float*[nRows];
	y1=new float*[nRows];
	y2=new float*[nRows];
	W=new double*[nRows];
	for(int i=0;i<nRows;i++)
	{
		L1[i]=new float[nCols];
		L2[i]=new float[nCols];
		a1[i]=new float[nCols];
		a2[i]=new float[nCols];
		b1[i]=new float[nCols];
		b2[i]=new float[nCols];
		x1[i]=new float[nCols];
		x2[i]=new float[nCols];
		y1[i]=new float[nCols];
		y2[i]=new float[nCols];
		W[i]=new double[nCols];
	}
	Initialize(L,a,b,L1,L2,a1,a2,b1,b2,x1,x2,y1,y2,W,nRows,nCols,StepX,StepY,colorCoefficient,distCoefficient);
	delete [] L;
	delete [] a;
	delete [] b;


	//Produce Superpixel
	DoSuperpixel(L1,L2,a1,a2,b1,b2,x1,x2,y1,y2,W,label,seedArray,newSeedNum,nRows,nCols,StepX,StepY,iterationNum,thresholdCoef);
	delete []seedArray;
        
//        preEnforceConnectivity(label, nRows, nCols);
	countSuperpixel(label,nRows,nCols);

	//Clear Memory
	for(int i=0;i<nRows;i++)
	{
		delete [] L1[i];
		delete [] L2[i];
		delete [] a1[i];
		delete [] a2[i];
		delete [] b1[i];
		delete [] b2[i];
		delete [] x1[i];
		delete [] x2[i];
		delete [] y1[i];
		delete [] y2[i];
		delete [] W[i];

	}
	delete []L1;
	delete []L2;
	delete []a1;
	delete []a2;
	delete []b1;
	delete []b2;
	delete []x1;
	delete []x2;
	delete []y1;
	delete []y2;
	delete []W;
}

void LSC(unsigned char* R,unsigned char* G,unsigned char* B,int nRows,int nCols,int StepY,int StepX,double ratio,unsigned short* label)
{
        int RowNum=nRows/StepY;
        int ColNum=nCols/StepX;
        int superpixelnum=ColNum*RowNum;    
        //Setting Parameter
	float colorCoefficient=20;
	float distCoefficient=colorCoefficient*ratio;
	int seedNum=superpixelnum;
	int iterationNum=20;
	int thresholdCoef=4;

	unsigned char *L, *a, *b;
	L=new unsigned char[nRows*nCols];
	a=new unsigned char[nRows*nCols];
	b=new unsigned char[nRows*nCols];

	myrgb2lab(R,G,B,L,a,b,nRows,nCols);


	//Produce Seeds
//	int ColNum,RowNum;
//        int StepY,StepX;
//	ColNum=sqrt(float(seedNum*nCols/nRows));
//	RowNum=seedNum/ColNum;
//	StepX=nRows/RowNum;
//	StepY=nCols/ColNum;
	point *seedArray=new point[seedNum];
	int newSeedNum=Seeds(nRows,nCols,RowNum,ColNum,StepX,StepY,seedNum,seedArray);


	//Initialization
	float **L1,**L2,**a1,**a2,**b1,**b2,**x1,**x2,**y1,**y2;
	double **W;
	L1=new float*[nRows];
	L2=new float*[nRows];
	a1=new float*[nRows];
	a2=new float*[nRows];
	b1=new float*[nRows];
	b2=new float*[nRows];
	x1=new float*[nRows];
	x2=new float*[nRows];
	y1=new float*[nRows];
	y2=new float*[nRows];
	W=new double*[nRows];
	for(int i=0;i<nRows;i++)
	{
		L1[i]=new float[nCols];
		L2[i]=new float[nCols];
		a1[i]=new float[nCols];
		a2[i]=new float[nCols];
		b1[i]=new float[nCols];
		b2[i]=new float[nCols];
		x1[i]=new float[nCols];
		x2[i]=new float[nCols];
		y1[i]=new float[nCols];
		y2[i]=new float[nCols];
		W[i]=new double[nCols];
	}
	Initialize(L,a,b,L1,L2,a1,a2,b1,b2,x1,x2,y1,y2,W,nRows,nCols,StepX,StepY,colorCoefficient,distCoefficient);
	delete [] L;
	delete [] a;
	delete [] b;


	//Produce Superpixel
	DoSuperpixel(L1,L2,a1,a2,b1,b2,x1,x2,y1,y2,W,label,seedArray,newSeedNum,nRows,nCols,StepX,StepY,iterationNum,thresholdCoef);
	delete []seedArray;

	countSuperpixel(label,nRows,nCols);

	//Clear Memory
	for(int i=0;i<nRows;i++)
	{
		delete [] L1[i];
		delete [] L2[i];
		delete [] a1[i];
		delete [] a2[i];
		delete [] b1[i];
		delete [] b2[i];
		delete [] x1[i];
		delete [] x2[i];
		delete [] y1[i];
		delete [] y2[i];
		delete [] W[i];

	}
	delete []L1;
	delete []L2;
	delete []a1;
	delete []a2;
	delete []b1;
	delete []b2;
	delete []x1;
	delete []x2;
	delete []y1;
	delete []y2;
	delete []W;
}

void LSC(unsigned char* R,unsigned char* G,unsigned char* B,int nRows,int nCols,int superpixelnum,double ratio,unsigned short* label)
{
	//Setting Parameter
	float colorCoefficient=20;
	float distCoefficient=colorCoefficient*ratio;
	int seedNum=superpixelnum;
	int iterationNum=20;
	int thresholdCoef=4;

	unsigned char *L, *a, *b;
	L=new unsigned char[nRows*nCols];
	a=new unsigned char[nRows*nCols];
	b=new unsigned char[nRows*nCols];

	myrgb2lab(R,G,B,L,a,b,nRows,nCols);


	//Produce Seeds
	int ColNum,RowNum,StepY,StepX;
	ColNum=sqrt(float(seedNum*nCols/nRows));
	RowNum=seedNum/ColNum;
	StepX=nRows/RowNum;
	StepY=nCols/ColNum;
	point *seedArray=new point[seedNum];
	int newSeedNum=Seeds(nRows,nCols,RowNum,ColNum,StepX,StepY,seedNum,seedArray);


	//Initialization
	float **L1,**L2,**a1,**a2,**b1,**b2,**x1,**x2,**y1,**y2;
	double **W;
	L1=new float*[nRows];
	L2=new float*[nRows];
	a1=new float*[nRows];
	a2=new float*[nRows];
	b1=new float*[nRows];
	b2=new float*[nRows];
	x1=new float*[nRows];
	x2=new float*[nRows];
	y1=new float*[nRows];
	y2=new float*[nRows];
	W=new double*[nRows];
	for(int i=0;i<nRows;i++)
	{
		L1[i]=new float[nCols];
		L2[i]=new float[nCols];
		a1[i]=new float[nCols];
		a2[i]=new float[nCols];
		b1[i]=new float[nCols];
		b2[i]=new float[nCols];
		x1[i]=new float[nCols];
		x2[i]=new float[nCols];
		y1[i]=new float[nCols];
		y2[i]=new float[nCols];
		W[i]=new double[nCols];
	}
	Initialize(L,a,b,L1,L2,a1,a2,b1,b2,x1,x2,y1,y2,W,nRows,nCols,StepX,StepY,colorCoefficient,distCoefficient);
	delete [] L;
	delete [] a;
	delete [] b;


	//Produce Superpixel
	DoSuperpixel(L1,L2,a1,a2,b1,b2,x1,x2,y1,y2,W,label,seedArray,newSeedNum,nRows,nCols,StepX,StepY,iterationNum,thresholdCoef);
	delete []seedArray;

	countSuperpixel(label,nRows,nCols);

	//Clear Memory
	for(int i=0;i<nRows;i++)
	{
		delete [] L1[i];
		delete [] L2[i];
		delete [] a1[i];
		delete [] a2[i];
		delete [] b1[i];
		delete [] b2[i];
		delete [] x1[i];
		delete [] x2[i];
		delete [] y1[i];
		delete [] y2[i];
		delete [] W[i];

	}
	delete []L1;
	delete []L2;
	delete []a1;
	delete []a2;
	delete []b1;
	delete []b2;
	delete []x1;
	delete []x2;
	delete []y1;
	delete []y2;
	delete []W;
}

#endif