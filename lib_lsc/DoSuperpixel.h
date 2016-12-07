#ifndef DOSUPERPIXEL
#define DOSUPERPIXEL

#include<vector>
#include"preEnforceConnectivity.h"
#include<algorithm>
#include"EnforceConnectivity.h"
#include"point.h"
using namespace std;


//Perform weighted kmeans iteratively in the ten dimensional feature space.

void DoSuperpixel(
		float** L1,
		float** L2,
		float** a1,
		float** a2,
		float** b1,
		float** b2,
		float** x1,
		float** x2,
		float** y1,
		float** y2,
		double** W,
		unsigned short int* label,
		point* seedArray,
		int seedNum,
		int nRows,
		int nCols,
		int StepX,
		int StepY,
		int iterationNum,
		int thresholdCoef
	)
{
	//Pre-treatment
	double** dist=new double*[nRows];
	for(int i=0;i<nRows;i++)
		dist[i]=new double[nCols];
	double* centerL1=new double[seedNum];
	double* centerL2=new double[seedNum];
	double* centera1=new double[seedNum];
	double* centera2=new double[seedNum];
	double* centerb1=new double[seedNum];
	double* centerb2=new double[seedNum];
	double* centerx1=new double[seedNum];
	double* centerx2=new double[seedNum];
	double* centery1=new double[seedNum];
	double* centery2=new double[seedNum];
	double* WSum=new double[seedNum];
	int* clusterSize=new int[seedNum];



//Initialization
	for(int i=0;i<seedNum;i++)
	{
		centerL1[i]=0;
		centerL2[i]=0;
		centera1[i]=0;
		centera2[i]=0;
		centerb1[i]=0;
		centerb2[i]=0;
		centerx1[i]=0;
		centerx2[i]=0;
		centery1[i]=0;
		centery2[i]=0;
		int x=seedArray[i].x;
		int y=seedArray[i].y;
		int minX=(x-StepX/4<=0)?0:x-StepX/4;
		int minY=(y-StepY/4<=0)?0:y-StepY/4;
		int maxX=(x+StepX/4>=nRows-1)?nRows-1:x+StepX/4;
		int maxY=(y+StepY/4>=nCols-1)?nCols-1:y+StepY/4;
		int Count=0;
		for(int j=minX;j<=maxX;j++)
			for(int k=minY;k<=maxY;k++)
			{
				Count++;
				centerL1[i]+=L1[j][k];
				centerL2[i]+=L2[j][k];
				centera1[i]+=a1[j][k];
				centera2[i]+=a2[j][k];
				centerb1[i]+=b1[j][k];
				centerb2[i]+=b2[j][k];
				centerx1[i]+=x1[j][k];
				centerx2[i]+=x2[j][k];
				centery1[i]+=y1[j][k];
				centery2[i]+=y2[j][k];
			}
		centerL1[i]/=Count;
		centerL2[i]/=Count;
		centera1[i]/=Count;
		centera2[i]/=Count;
		centerb1[i]/=Count;
		centerb2[i]/=Count;
		centerx1[i]/=Count;
		centerx2[i]/=Count;
		centery1[i]/=Count;
		centery2[i]/=Count;
	}



	//K-means
	for(int iteration=0;iteration<=iterationNum;iteration++)
	{
		for(int i=0;i<nRows;i++)
			for(int j=0;j<nCols;j++)
				dist[i][j]=DBL_MAX;




		int minX,minY,maxX,maxY;double D;
		for(int i=0;i<seedNum;i++)
		{
			int x=seedArray[i].x;
			int y=seedArray[i].y;
			minX=(x-(StepX)<=0)?0:x-StepX;
			minY=(y-(StepY)<=0)?0:y-StepY;
			maxX=(x+(StepX)>=nRows-1)?nRows-1:x+StepX;
			maxY=(y+(StepY)>=nCols-1)?nCols-1:y+StepY;
			for(int m=minX;m<=maxX;m++)
				for(int n=minY;n<=maxY;n++)
				{
					D=  (L1[m][n]-centerL1[i])*(L1[m][n]-centerL1[i])+
						(L2[m][n]-centerL2[i])*(L2[m][n]-centerL2[i])+
						(a1[m][n]-centera1[i])*(a1[m][n]-centera1[i])+
						(a2[m][n]-centera2[i])*(a2[m][n]-centera2[i])+
						(b1[m][n]-centerb1[i])*(b1[m][n]-centerb1[i])+
						(b2[m][n]-centerb2[i])*(b2[m][n]-centerb2[i])+
						(x1[m][n]-centerx1[i])*(x1[m][n]-centerx1[i])+
						(x2[m][n]-centerx2[i])*(x2[m][n]-centerx2[i])+
						(y1[m][n]-centery1[i])*(y1[m][n]-centery1[i])+
						(y2[m][n]-centery2[i])*(y2[m][n]-centery2[i]);
					if(D<dist[m][n])
					{
						label[m*nCols+n]=i;
						dist[m][n]=D;
					}
				}
		}



		for(int i=0;i<seedNum;i++)
		{
			centerL1[i]=0;
			centerL2[i]=0;
			centera1[i]=0;
			centera2[i]=0;
			centerb1[i]=0;
			centerb2[i]=0;
			centerx1[i]=0;
			centerx2[i]=0;
			centery1[i]=0;
			centery2[i]=0;
			WSum[i]=0;
			clusterSize[i]=0;
			seedArray[i].x=0;
			seedArray[i].y=0;
		}



		for(int i=0;i<nRows;i++)
		{
			for(int j=0;j<nCols;j++)
			{
				int L=label[i*nCols+j];
                                if (L >= seedNum) std::cout << L << " " << seedNum << std::endl;
				double Weight=W[i][j];
				centerL1[L]+=Weight*L1[i][j];
				centerL2[L]+=Weight*L2[i][j];
				centera1[L]+=Weight*a1[i][j];
				centera2[L]+=Weight*a2[i][j];
				centerb1[L]+=Weight*b1[i][j];
				centerb2[L]+=Weight*b2[i][j];
				centerx1[L]+=Weight*x1[i][j];
				centerx2[L]+=Weight*x2[i][j];
				centery1[L]+=Weight*y1[i][j];
				centery2[L]+=Weight*y2[i][j];
				clusterSize[L]++;
				WSum[L]+=Weight;
				seedArray[L].x+=i;
				seedArray[L].y+=j;
			}
		}
		for(int i=0;i<seedNum;i++)
		{
			WSum[i]=(WSum[i]==0)?1:WSum[i];
			clusterSize[i]=(clusterSize[i]==0)?1:clusterSize[i];
		}
		for(int i=0;i<seedNum;i++)
		{
			centerL1[i]/=WSum[i];
			centerL2[i]/=WSum[i];
			centera1[i]/=WSum[i];
			centera2[i]/=WSum[i];
			centerb1[i]/=WSum[i];
			centerb2[i]/=WSum[i];
			centerx1[i]/=WSum[i];
			centerx2[i]/=WSum[i];
			centery1[i]/=WSum[i];
			centery2[i]/=WSum[i];
			seedArray[i].x/=clusterSize[i];
			seedArray[i].y/=clusterSize[i];
		}
	}



	//EnforceConnection
	int threshold=(nRows*nCols)/(seedNum*thresholdCoef);
	preEnforceConnectivity(label,nRows,nCols);
	EnforceConnectivity(L1,L2,a1,a2,b1,b2,x1,x2,y1,y2,W,label,threshold,nRows,nCols);



	//Clear Memory
	delete []centerL1;
	delete []centerL2;
	delete []centera1;
	delete []centera2;
	delete []centerb1;
	delete []centerb2;
	delete []centerx1;
	delete []centerx2;
	delete []centery1;
	delete []centery2;
	delete []WSum;
	delete []clusterSize;
	for(int i=0;i<nRows;i++)
		delete [] dist[i];
	delete []dist;
	return;
}

#endif
