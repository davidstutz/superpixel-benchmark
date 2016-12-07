#ifndef ENFORCECONNECTIVITY
#define ENFORCECONNECTIVITY

#include<queue>
#include<vector>
using namespace std;

//Enforce Connectivity by merging very small superpixels with their neighbors

void preEnforceConnectivity(unsigned short int* label, int nRows,int nCols)
{
	const int dx8[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
	const int dy8[8] = { 0, -1, -1, -1, 0, 1, 1,  1};
	int adj=0;
	int Bond=20;
	bool **mask=new bool* [nRows];
	for(int i=0;i<nRows;i++)
	{
		mask[i]=new bool[nCols];
		for(int j=0;j<nCols;j++)
			mask[i][j]=0;
	}
	vector<unsigned short> xLoc;
	vector<unsigned short> yLoc;
	for(int i=0;i<nRows;i++)
		for(int j=0;j<nCols;j++)
		{
			if(mask[i][j]==0)
			{
				int L=label[i*nCols+j];
				for(int k=0;k<8;k++)
				{
					int x=i+dx8[k];
					int y=j+dy8[k];
					if(x>=0&&x<=nRows-1&&y>=0&&y<=nCols-1)
					{
						if(mask[x][y]==1&&label[x*nCols+y]!=L)
							adj=label[x*nCols+y];break;
					}
				}
				mask[i][j]=1;
				xLoc.insert(xLoc.end(),i);
				yLoc.insert(yLoc.end(),j);
				int indexMarker=0;
				while(indexMarker<xLoc.size())
				{
					int x=xLoc[indexMarker];int y=yLoc[indexMarker];
					indexMarker++;
					int minX=(x-1<=0)?0:x-1;
					int maxX=(x+1>=nRows-1)?nRows-1:x+1;
					int minY=(y-1<=0)?0:y-1;
					int maxY=(y+1>=nCols-1)?nCols-1:y+1;
					for(int m=minX;m<=maxX;m++)
						for(int n=minY;n<=maxY;n++)
						{
							if(mask[m][n]==0&&label[m*nCols+n]==L)
							{
								mask[m][n]=1;
								xLoc.insert(xLoc.end(),m);
								yLoc.insert(yLoc.end(),n);
							}
						}
				}
				if(indexMarker<Bond)
				{
					for(int k=0;k<xLoc.size();k++)
					{
						int x=xLoc[k];int y=yLoc[k];
						label[x*nCols+y]=adj;
					}
				}
				xLoc.clear();
				yLoc.clear();
			}
		}
	for(int i=0;i<nRows;i++)
		delete [] mask[i];
	delete [] mask;
}

#endif