#ifndef COUNTSUPERPIXEL
#define COUNTSUPERPIXEL

#include<iostream>
#include<vector>
#include<queue>
#include<algorithm>
using namespace std;

class Point
{
public:
	int x;
	int y;
	Point(int X=0,int Y=0)
	{
		x=X;
		y=Y;
	}
};

int countSuperpixel(unsigned short int* label,int nRows,int nCols)
{
	Point P;
	queue<Point> Q;
	int L;
	int labelNum=0;
	int x,y,maxX,minX,maxY,minY;
	bool** mask=new bool*[nRows];
	for(int i=0;i<nRows;i++)
	{
		mask[i]=new bool[nCols];
		for(int j=0;j<nCols;j++)
			mask[i][j]=false;
	}

	for(int i=0;i<nRows;i++)
		for(int j=0;j<nCols;j++)
		{
			if(mask[i][j]==false)
			{
				L=label[i*nCols+j];
				labelNum++;
				label[i*nCols+j]=labelNum;
				P.x=i;P.y=j;
				Q.push(P);
				mask[i][j]=true;
				while(!Q.empty())
				{
					P=Q.front();
					Q.pop();
					x=P.x;y=P.y;
					minX=(x-1<=0)?0:x-1;
					minY=(y-1<=0)?0:y-1;
					maxX=(x+1>=nRows-1)?nRows-1:x+1;
					maxY=(y+1>=nCols-1)?nCols-1:y+1;
					for(int m=minX;m<=maxX;m++)
						for(int n=minY;n<=maxY;n++)
						{
							if(label[m*nCols+n]==L&&mask[m][n]==false)
							{
								P.x=m;P.y=n;
								mask[m][n]=true;
								label[m*nCols+n]=labelNum;
								Q.push(P);
							}
						}
				}
			}
		}
	for(int i=0;i<nRows;i++)
		delete [] mask[i];
	delete [] mask;
	return labelNum;
}

#endif