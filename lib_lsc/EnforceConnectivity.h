#include<iostream>
#include<queue>
#include<vector>
#include<algorithm>
#include<float.h>
using namespace std;

class Superpixel
{
public:
	int Label;
	int Size;
	vector<int> Neighbor;
	Superpixel(int L=0,int S=0):Label(L),Size(S){}
	vector<int> xLoc;
	vector<int> yLoc;
	friend bool operator==(Superpixel& S,int L);
	friend bool operator==(int L,Superpixel& S);
};

bool operator==(Superpixel& S,int L)
{
	return S.Label==L;
}

bool operator==(int L,Superpixel& S)
{
	return S.Label==L;
}


void EnforceConnectivity(
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
		int threshold,
		int nRows,
		int nCols
	)
{
	unsigned char** mask=new unsigned char*[nRows];
	for(int i=0;i<nRows;i++)
	{
		mask[i]=new unsigned char[nCols];
		for(int j=0;j<nCols;j++)
			mask[i][j]=0;
	}

	vector<unsigned short>strayX;
	vector<unsigned short>strayY;
	vector<unsigned short>Size;
	queue<unsigned short> xLoc;
	queue<unsigned short> yLoc;
	vector<double>centerL1;
	vector<double>centerL2;
	vector<double>centera1;
	vector<double>centera2;
	vector<double>centerb1;
	vector<double>centerb2;
	vector<double>centerx1;
	vector<double>centerx2;
	vector<double>centery1;
	vector<double>centery2;
	vector<double>centerW;
	int sLabel=-1;
	int L;
	for(int i=0;i<nRows;i++)
		for(int j=0;j<nCols;j++)
		{
			if(mask[i][j]==0)
			{
				sLabel++;
				int Count=1;
				centerL1.insert(centerL1.end(),0);
				centerL2.insert(centerL2.end(),0);
				centera1.insert(centera1.end(),0);
				centera2.insert(centera2.end(),0);
				centerb1.insert(centerb1.end(),0);
				centerb2.insert(centerb2.end(),0);
				centerx1.insert(centerx1.end(),0);
				centerx2.insert(centerx2.end(),0);
				centery1.insert(centery1.end(),0);
				centery2.insert(centery2.end(),0);
				centerW.insert(centerW.end(),0);
				strayX.insert(strayX.end(),i);
				strayY.insert(strayY.end(),j);
				double Weight=W[i][j];
				centerL1[sLabel]+=L1[i][j]*Weight;
				centerL2[sLabel]+=L2[i][j]*Weight;
				centera1[sLabel]+=a1[i][j]*Weight;
				centera2[sLabel]+=a2[i][j]*Weight;
				centerb1[sLabel]+=b1[i][j]*Weight;
				centerb2[sLabel]+=b2[i][j]*Weight;
				centerx1[sLabel]+=x1[i][j]*Weight;
				centerx2[sLabel]+=x2[i][j]*Weight;
				centery1[sLabel]+=y1[i][j]*Weight;
				centery2[sLabel]+=y2[i][j]*Weight;
				centerW[sLabel]+=W[i][j];
				L=label[i*nCols+j];
				label[i*nCols+j]=sLabel;
				mask[i][j]=1;
				xLoc.push(i);yLoc.push(j);
				while(!xLoc.empty())
				{
					int x=xLoc.front();xLoc.pop();
					int y=yLoc.front();yLoc.pop();
					int minX=(x-1<=0)?0:x-1;
					int maxX=(x+1>=nRows-1)?nRows-1:x+1;
					int minY=(y-1<=0)?0:y-1;
					int maxY=(y+1>=nCols-1)?nCols-1:y+1;
					for(int m=minX;m<=maxX;m++)
						for(int n=minY;n<=maxY;n++)
						{
							if(mask[m][n]==0&&label[m*nCols+n]==L)
							{
								Count++;
								xLoc.push(m);
								yLoc.push(n);
								mask[m][n]=1;
								label[m*nCols+n]=sLabel;
								Weight=W[m][n];
								centerL1[sLabel]+=L1[m][n]*Weight;
								centerL2[sLabel]+=L2[m][n]*Weight;
								centera1[sLabel]+=a1[m][n]*Weight;
								centera2[sLabel]+=a2[m][n]*Weight;
								centerb1[sLabel]+=b1[m][n]*Weight;
								centerb2[sLabel]+=b2[m][n]*Weight;
								centerx1[sLabel]+=x1[m][n]*Weight;
								centerx2[sLabel]+=x2[m][n]*Weight;
								centery1[sLabel]+=y1[m][n]*Weight;
								centery2[sLabel]+=y2[m][n]*Weight;
								centerW[sLabel]+=W[m][n];
							}
						}
				}
				Size.insert(Size.end(),Count);
				centerL1[sLabel]/=centerW[sLabel];
				centerL2[sLabel]/=centerW[sLabel];
				centera1[sLabel]/=centerW[sLabel];
				centera2[sLabel]/=centerW[sLabel];
				centerb1[sLabel]/=centerW[sLabel];
				centerb2[sLabel]/=centerW[sLabel];
				centerx1[sLabel]/=centerW[sLabel];
				centerx2[sLabel]/=centerW[sLabel];
				centery1[sLabel]/=centerW[sLabel];
				centery2[sLabel]/=centerW[sLabel];
			}
		}

	sLabel=sLabel+1;
//	int Count=0;
	



	vector<int>::iterator Pointer;
	vector<Superpixel> Sarray;
	for(int i=0;i<sLabel;i++)
	{
		if(Size[i]<threshold)
		{
			int x=strayX[i];int y=strayY[i];
			L=label[x*nCols+y];
			mask[x][y]=0;
			int indexMark=0;
			Superpixel S(L,Size[i]);
			S.xLoc.insert(S.xLoc.end(),x);
			S.yLoc.insert(S.yLoc.end(),y);
			while(indexMark<S.xLoc.size())
			{
				x=S.xLoc[indexMark];y=S.yLoc[indexMark];
				indexMark++;
				int minX=(x-1<=0)?0:x-1;
				int maxX=(x+1>=nRows-1)?nRows-1:x+1;
				int minY=(y-1<=0)?0:y-1;
				int maxY=(y+1>=nCols-1)?nCols-1:y+1;
				for(int m=minX;m<=maxX;m++)
					for(int n=minY;n<=maxY;n++)
					{
						if(mask[m][n]==1&&label[m*nCols+n]==L)
						{
							mask[m][n]=0;
							S.xLoc.insert(S.xLoc.end(),m);
							S.yLoc.insert(S.yLoc.end(),n);
						}
						else if(label[m*nCols+n]!=L)
						{
							int NewLabel=label[m*nCols+n];
							Pointer=find(S.Neighbor.begin(),S.Neighbor.end(),NewLabel);
							if(Pointer==S.Neighbor.end())
							{
								S.Neighbor.insert(S.Neighbor.begin(),NewLabel);
							}
						}
					}

			}
			Sarray.insert(Sarray.end(),S);
		}
	}

	vector<Superpixel>::iterator S;
	vector<int>::iterator I;
	vector<int>::iterator I2;
	S=Sarray.begin();
	while(S!=Sarray.end())
	{
		double MinDist=DBL_MAX;
		int Label1=(*S).Label;
		int Label2=-1;
		for(I=(*S).Neighbor.begin();I!=(*S).Neighbor.end();I++)
		{
			double D=(centerL1[Label1]-centerL1[*I])*(centerL1[Label1]-centerL1[*I])+
				(centerL2[Label1]-centerL2[*I])*(centerL2[Label1]-centerL2[*I])+
				(centera1[Label1]-centera1[*I])*(centera1[Label1]-centera1[*I])+
				(centera2[Label1]-centera2[*I])*(centera2[Label1]-centera2[*I])+
				(centerb1[Label1]-centerb1[*I])*(centerb1[Label1]-centerb1[*I])+
				(centerb2[Label1]-centerb2[*I])*(centerb2[Label1]-centerb2[*I])+
				(centerx1[Label1]-centerx1[*I])*(centerx1[Label1]-centerx1[*I])+
				(centerx2[Label1]-centerx2[*I])*(centerx2[Label1]-centerx2[*I])+
				(centery1[Label1]-centery1[*I])*(centery1[Label1]-centery1[*I])+
				(centery2[Label1]-centery2[*I])*(centery2[Label1]-centery2[*I]);
			if(D<MinDist)
			{
				MinDist=D;
				Label2=(*I);
			}
		}
		double W1=centerW[Label1];
		double W2=centerW[Label2];
		double W=W1+W2;
		centerL1[Label2]=(W2*centerL1[Label2]+W1*centerL1[Label1])/W;
		centerL2[Label2]=(W2*centerL2[Label2]+W1*centerL2[Label1])/W;
		centera1[Label2]=(W2*centera1[Label2]+W1*centera1[Label1])/W;
		centera2[Label2]=(W2*centera2[Label2]+W1*centera2[Label1])/W;
		centerb1[Label2]=(W2*centerb1[Label2]+W1*centerb1[Label1])/W;
		centerb2[Label2]=(W2*centerb2[Label2]+W1*centerb2[Label1])/W;
		centerx1[Label2]=(W2*centerx1[Label2]+W1*centerx1[Label1])/W;
		centerx2[Label2]=(W2*centerx2[Label2]+W1*centerx2[Label1])/W;
		centery1[Label2]=(W2*centery1[Label2]+W1*centery1[Label1])/W;
		centery2[Label2]=(W2*centery2[Label2]+W1*centery2[Label1])/W;
		centerW[Label2]=W;
		for(int i=0;i<(*S).xLoc.size();i++)
		{
			int x=(*S).xLoc[i];int y=(*S).yLoc[i];
			label[x*nCols+y]=Label2;
		}
		vector<Superpixel>::iterator Stmp;
		Stmp=find(Sarray.begin(),Sarray.end(),Label2);
		if(Stmp!=Sarray.end())
		{
			Size[Label2]=Size[Label1]+Size[Label2];
			if(Size[Label2]>=threshold)
			{
				Sarray.erase(Stmp);
				Sarray.erase(S);
			}
			else
			{
				(*Stmp).xLoc.insert((*Stmp).xLoc.end(),(*S).xLoc.begin(),(*S).xLoc.end());
				(*Stmp).yLoc.insert((*Stmp).yLoc.end(),(*S).yLoc.begin(),(*S).yLoc.end());
				(*Stmp).Neighbor.insert((*Stmp).Neighbor.end(),(*S).Neighbor.begin(),(*S).Neighbor.end());
				sort((*Stmp).Neighbor.begin(),(*Stmp).Neighbor.end());
				I=unique((*Stmp).Neighbor.begin(),(*Stmp).Neighbor.end());
				(*Stmp).Neighbor.erase(I,(*Stmp).Neighbor.end());
				I=find((*Stmp).Neighbor.begin(),(*Stmp).Neighbor.end(),Label1);
				(*Stmp).Neighbor.erase(I);
				I=find((*Stmp).Neighbor.begin(),(*Stmp).Neighbor.end(),Label2);
				(*Stmp).Neighbor.erase(I);
				Sarray.erase(S);
			}
		}
		else
		{
			Sarray.erase(S);
		}
		for(int i=0;i<Sarray.size();i++)
		{
			I=find(Sarray[i].Neighbor.begin(),Sarray[i].Neighbor.end(),Label1);
			I2=find(Sarray[i].Neighbor.begin(),Sarray[i].Neighbor.end(),Label2);
			if(I!=Sarray[i].Neighbor.end()&&I2!=Sarray[i].Neighbor.end())
				Sarray[i].Neighbor.erase(I);
			else if(I!=Sarray[i].Neighbor.end()&&I2==Sarray[i].Neighbor.end())
				(*I)=Label2;
		}
		S=Sarray.begin();
	}
	for(int i=0;i<nRows;i++)
		delete [] mask[i];
	delete [] mask;
	return;
}

