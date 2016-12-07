#ifndef INITIALIZE
#define INITIALIZE

#include<cmath>
using namespace std;

//map pixels into ten dimensional feature space

const double PI=3.1415926;
void Initialize(
		unsigned char* L,
		unsigned char* a,
		unsigned char* b,
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
		int nRows,
		int nCols,
		int StepX,
		int StepY,
		float Color,
		float Distance
	)
{
	float thetaL,thetaa,thetab,thetax,thetay;
	for(int i=0;i<nRows;i++)
		for(int j=0;j<nCols;j++)
		{
			thetaL=((float)L[i*nCols+j]/(float)255)*PI/2;
			thetaa=((float)a[i*nCols+j]/(float)255)*PI/2;
			thetab=((float)b[i*nCols+j]/(float)255)*PI/2;
			thetax=((float)i/(float)StepX)*PI/2;
			thetay=((float)j/(float)StepY)*PI/2;
			L1[i][j]=Color*cos(thetaL);
			L2[i][j]=Color*sin(thetaL);
			a1[i][j]=Color*cos(thetaa)*2.55;
			a2[i][j]=Color*sin(thetaa)*2.55;
			b1[i][j]=Color*cos(thetab)*2.55;
			b2[i][j]=Color*sin(thetab)*2.55;
			x1[i][j]=Distance*cos(thetax);
			x2[i][j]=Distance*sin(thetax);
			y1[i][j]=Distance*cos(thetay);
			y2[i][j]=Distance*sin(thetay);
		}
	double sigmaL1=0,sigmaL2=0,sigmaa1=0,sigmaa2=0,sigmab1=0,sigmab2=0,sigmax1=0,sigmax2=0,sigmay1=0,sigmay2=0;
	double size=nRows*nCols;
	for(int i=0;i<nRows;i++)
		for(int j=0;j<nCols;j++)
		{
			sigmaL1+=L1[i][j];
			sigmaL2+=L2[i][j];
			sigmaa1+=a1[i][j];
			sigmaa2+=a2[i][j];
			sigmab1+=b1[i][j];
			sigmab2+=b2[i][j];
			sigmax1+=x1[i][j];
			sigmax2+=x2[i][j];
			sigmay1+=y1[i][j];
			sigmay2+=y2[i][j];
		}
	sigmaL1/=size;
	sigmaL2/=size;
	sigmaa1/=size;
	sigmaa2/=size;
	sigmab1/=size;
	sigmab2/=size;
	sigmax1/=size;
	sigmax2/=size;
	sigmay1/=size;
	sigmay2/=size;
	for(int i=0;i<nRows;i++)
		for(int j=0;j<nCols;j++)
		{
			W[i][j]=L1[i][j]*sigmaL1+
					L2[i][j]*sigmaL2+
					a1[i][j]*sigmaa1+
					a2[i][j]*sigmaa2+
					b1[i][j]*sigmab1+
					b2[i][j]*sigmab2+
					x1[i][j]*sigmax1+
					x2[i][j]*sigmax2+
					y1[i][j]*sigmay1+
					y2[i][j]*sigmay2;
			L1[i][j]/=W[i][j];
			L2[i][j]/=W[i][j];
			a1[i][j]/=W[i][j];
			a2[i][j]/=W[i][j];
			b1[i][j]/=W[i][j];
			b2[i][j]/=W[i][j];
			x1[i][j]/=W[i][j];
			x2[i][j]/=W[i][j];
			y1[i][j]/=W[i][j];
			y2[i][j]/=W[i][j];
		}
	return;
}
#endif