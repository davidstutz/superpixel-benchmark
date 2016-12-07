// SegmentExtraction.cpp: implementation of the SegmentExtraction class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <vector>
#include "SegmentExtraction.h"
#include <algorithm>
#include <cmath> 
#include "opencv2/opencv.hpp"

using namespace std;

#define p3(Y,r,c,chan) (((uchar*)(Y->imageData + Y->widthStep*(r)))[(c)*3+(chan)])


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


SegmentExtraction::SegmentExtraction()
{
	SegList = new DynamicList[1];
	SegData = new DynamicSegList[1];
	BorderList = new DynamicList[1];
	EdgeList   = new DynamicList[1];
	ThickBorderList = new DynamicList[1];
}
SegmentExtraction::~SegmentExtraction()
{

}

void SegmentExtraction::get_data(int Width,int Height, std::vector< uchar *> ImArray,int index,int *Segno)
{
	mW = Width;
	mH = Height;
	//mImageNo = ImArray.size();
	mIndex = index;
	step = Segno[0];
	mIterNo = Segno[1];
	mOutput_Choice = 1; // always show boundary
	mCompactness = Segno[2];
	mSegmentIndexK = (int*)calloc((mH*mW),sizeof(int));
	BorderMask = (int*)calloc((mH*mW),sizeof(int));
	mBorderList = (int*)calloc((2*mH*mW),sizeof(int));
	mBorderCount = 0;
}
void SegmentExtraction::KmeansOverSeg(uchar * Im,uchar * Im_out,int frame_no)
{
	clock_t start,end,FullS,FullE;
	clock_t start1,end1,start2,end2;
	start = clock();

	double dif1,dif2,dif3;
//	int SegNo = mSegmentNoK;
//	int step = sqrt(double(mW*mH/SegNo));
	int ColNo = mW/step;
	int RowNo = mH/step;
	int SegNo = ColNo*RowNo;
	mSegmentNoK = SegNo;

	//EdgeMap = (int*)calloc((ColNo*RowNo),sizeof(int));
	FullS = clock();
	//find_edgeness(Im);
	SegNo = mSegmentNoK;

	int Rval,Gval,Bval,count,sz,limit,Pindex;
	int cost,MinCost,index,Mindex,cost1;
	int costP1,costP;
	float compactness,t_size;
	//int *BorderMask = new int[mW*mH];
	int *CandidateList = (int*)calloc((mW*mH*9),sizeof(int));
	Segnode *BB,*CC;

	R_mean = (int*)calloc((mSegmentNoK),sizeof(int));
	G_mean = (int*)calloc((mSegmentNoK),sizeof(int));
	B_mean = (int*)calloc((mSegmentNoK),sizeof(int));
	Count_mean = (int*)calloc((mSegmentNoK),sizeof(int));
	X_mean = (int*)calloc((mSegmentNoK),sizeof(int));
	Y_mean = (int*)calloc((mSegmentNoK*2),sizeof(int));
	RGB_total = (int*)calloc((mSegmentNoK*3),sizeof(int));
	XY_total = (int*)calloc((mSegmentNoK*2),sizeof(int));
	mSegChange = (bool*)calloc((mSegmentNoK),sizeof(bool));
	//float* im_Lab = (float*)calloc((mW*mH*3),sizeof(float));
	//rgb2Lab(Im, im_Lab,mH,mW);

	float *one_over_size= (float*)calloc((100000),sizeof(float));
	for(int i=1;i<100000;i++)
		one_over_size[i] = 1/(float(i));

	//SegDataK = new DynamicSegList[SegNo];
	//SegListK = new DynamicList[SegNo];
	memset(mSegmentIndexK, -1, mW*mH*sizeof(int));
	
	start = clock();

	if(frame_no==0)
		initialize(Im,ColNo,RowNo,step,one_over_size);
	//initialize(im_Lab,ColNo,RowNo,step,one_over_size);
	end = clock();
	dif1 = end - start;
	//printf ("It took  %f seconds for Initialization\n", dif1/CLOCKS_PER_SEC);

	start = clock();
	find_borders_Kmeans(CandidateList);
	end = clock();
	dif1 = end - start;
//	printf ("It took  %f seconds for Border\n", dif1/CLOCKS_PER_SEC);
	//compactness = 500000/(mW*mH);
	compactness = 0.15*mCompactness*16/((float)step*100);
	//mIterNo = step*step/10;
	//mIterNo = 0;

	

	//for(int iter =0;iter<mIterNo;iter++)
	//{
	//	count = 0;

	//	for(int j=1;j<mH-1;j++)
	//	{
	//		for(int i=1;i<mW-1;i++)
	//		{
	//			if(BorderMask[i + j*mW] == 0) continue;

	//			int segInd = mSegmentIndexK[j*mW + i];
	//			int Xmean = X_mean[segInd];
	//			int Ymean = Y_mean[segInd];
	//			//Rval = im_Lab[3*(i+mW*j)];Gval = im_Lab[3*(i+mW*j)+1];Bval = im_Lab[3*(i+mW*j)+2];
	//			Rval = Im[3*(i+mW*j)];Gval = Im[3*(i+mW*j)+1];Bval = Im[3*(i+mW*j)+2];
	//			

	//			cost1 = abs(R_mean[segInd] - Rval) + abs(G_mean[segInd] - Gval) + abs(B_mean[segInd] - Bval);
	//			costP1 = (i - Xmean)*(i - Xmean) + (j - Ymean)*(j - Ymean);

	//			MinCost = cost1 + costP1*compactness;
	//			Mindex = segInd;

	//			for(int ii=0;ii<CandidateList[i + mW*j];ii++)
	//			{
	//				index = CandidateList[i + mW*j+(ii+1)*mW*mH];
	//				Xmean = X_mean[index];
	//				Ymean = Y_mean[index];

	//				cost = abs(R_mean[index] - Rval) + abs(G_mean[index] - Gval) + abs(B_mean[index] - Bval);
	//				costP = (i - Xmean)*(i - Xmean) + (j - Ymean)*(j - Ymean);

	//				cost = cost + costP*compactness;

	//				if(cost<MinCost)
	//				{
	//					MinCost = cost;
	//					Mindex = index;
	//				}
	//			}
	//			if(Mindex != segInd)
	//			{
	//				if(Count_mean[segInd])
	//				{
	//					Count_mean[segInd] = Count_mean[segInd] - 1;
	//					RGB_total[segInd] -= Rval;
	//					RGB_total[segInd+mSegmentNoK] -= Gval;
	//					RGB_total[segInd+2*mSegmentNoK] -= Bval;
	//					XY_total[segInd] -= i;
	//					XY_total[segInd+mSegmentNoK] -= j;
	//				}

	//				Count_mean[Mindex] = Count_mean[Mindex] + 1;
	//				RGB_total[Mindex] += Rval;
	//				RGB_total[Mindex+mSegmentNoK] += Gval;
	//				RGB_total[Mindex+2*mSegmentNoK] += Bval;
	//				XY_total[Mindex] += i;
	//				XY_total[Mindex+mSegmentNoK] += j;
	//				mSegmentIndexK[i+mW*j]=Mindex;
	//			}
	//			
	//		}
	//	}
	//	update_segments(Im,one_over_size);
	//	//if(iter<mIterNo-1)
	//		find_borders_Kmeans(CandidateList);
	//}

	dif2 = 0;
	dif3 = 0;
	// Visit only Boundary Pixels
	for(int iter =0;iter<mIterNo;iter++)
	{
		count = 0;
		
		memset(mSegChange,0,mSegmentNoK*sizeof(bool));
		for(int c=0;c<mBorderCount;c++)
		{
			int i = mBorderList[2*c];
			int j = mBorderList[2*c+1];

			Pindex = i+j*mW;
			int segInd = mSegmentIndexK[Pindex];

			int Xmean = X_mean[segInd];
			int Ymean = Y_mean[segInd];
			//Rval = im_Lab[3*(i+mW*j)];Gval = im_Lab[3*(i+mW*j)+1];Bval = im_Lab[3*(i+mW*j)+2];
			Rval = Im[3*Pindex];Gval = Im[3*Pindex+1];Bval = Im[3*Pindex+2];	

			cost1 = abs(R_mean[segInd] - Rval) + abs(G_mean[segInd] - Gval) + abs(B_mean[segInd] - Bval);
			costP1 = (i - Xmean)*(i - Xmean) + (j - Ymean)*(j - Ymean);

			MinCost = cost1 + costP1*compactness;
			Mindex = segInd;

			for(int ii=0;ii<CandidateList[Pindex];ii++)
			{
				//index = CandidateList[i + mW*j+(ii+1)*mW*mH];
				index = CandidateList[9*(Pindex)+(ii+1)];
				Xmean = X_mean[index];
				Ymean = Y_mean[index];

				cost = abs(R_mean[index] - Rval) + abs(G_mean[index] - Gval) + abs(B_mean[index] - Bval);
				costP = (i - Xmean)*(i - Xmean) + (j - Ymean)*(j - Ymean);
				

				cost = cost + costP*compactness;

				if(cost<MinCost)
				{
					MinCost = cost;
					Mindex = index;
				}
			}
			if(Mindex != segInd)
			{
				mSegChange[segInd] = true;
				mSegChange[Mindex] = true;
				if(Count_mean[segInd])
				{
					Count_mean[segInd] = Count_mean[segInd] - 1;
					RGB_total[segInd] -= Rval;
					RGB_total[segInd+mSegmentNoK] -= Gval;
					RGB_total[segInd+2*mSegmentNoK] -= Bval;
					XY_total[segInd] -= i;
					XY_total[segInd+mSegmentNoK] -= j;
				}

				Count_mean[Mindex] = Count_mean[Mindex] + 1;
				RGB_total[Mindex] += Rval;
				RGB_total[Mindex+mSegmentNoK] += Gval;
				RGB_total[Mindex+2*mSegmentNoK] += Bval;
				XY_total[Mindex] += i;
				XY_total[Mindex+mSegmentNoK] += j;
				mSegmentIndexK[i+mW*j]=Mindex;
			}

		}
		
		start1 = clock();
		update_segments(Im,one_over_size);
		end1 = clock();
		dif2 += end1 - start1;
		
		//if(iter<mIterNo-1)
		start1 = clock();
			find_borders_Kmeans2(CandidateList,iter);
		end1 = clock();
		dif3 += end1 - start1;

		//printf ("Number of Border Pixels = %d\n", mBorderCount);
	}

	end = clock();
	dif1 = end - start;
	//printf ("It took  %f seconds for Iteration\n", dif1/CLOCKS_PER_SEC);

	FullE = clock();
	dif1 = FullE - FullS;
//	printf ("It took  %f seconds for All Computations \n", dif1/CLOCKS_PER_SEC);
//	printf ("It took  %f seconds for ModelUpdate \n", dif2/(CLOCKS_PER_SEC));
//	printf ("It took  %f seconds for BoundaryDetection \n", dif3/(CLOCKS_PER_SEC));

	total_time += dif1/CLOCKS_PER_SEC;
	/////////   Fill the output Image
	int dx8[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
	int dy8[8] = { 0, -1, -1, -1, 0, 1, 1,  1};

	sz = mW*mH;

	vector<bool> istaken(sz, false);

	int mainindex(0);
	for( int j = 0; j < mH; j++ )
	{
		for( int k = 0; k < mW; k++ )
		{
			int np(0);
			for( int i = 0; i < 8; i++ )
			{
				int x = k + dx8[i];
				int y = j + dy8[i];

				if( (x >= 0 && x < mW) && (y >= 0 && y < mH) )
				{
					int index = y*mW + x;

					if( false == istaken[index] )//comment this to obtain internal contours
					{
						if( mSegmentIndexK[mainindex] != mSegmentIndexK[index] ) np++;
					}
				}
			}
			/*Im_out[3*mainindex] = R_mean[mSegmentIndexK[mainindex]];
			Im_out[3*mainindex+1] = G_mean[mSegmentIndexK[mainindex]];
			Im_out[3*mainindex+2] = B_mean[mSegmentIndexK[mainindex]];*/
			Im_out[3*mainindex] = Im[3*mainindex];
			Im_out[3*mainindex+1] = Im[3*mainindex+1];
			Im_out[3*mainindex+2] = Im[3*mainindex+2];
			if( np > 1 )//change to 2 or 3 for thinner lines
			{
				Im_out[3*mainindex] = 0;
				Im_out[3*mainindex+1] = 0;
				Im_out[3*mainindex+2] = 255;
				istaken[mainindex] = true;
			}
			mainindex++;
		}
	}

	/// Kutalm�� Format�
	//int SegID,Rvalue,Gvalue,Bvalue;
	//for( int j = 0; j < mH; j++ )
	//{
	//	for( int k = 0; k < mW; k++ )
	//	{
	//		mainindex = j*mW + k;
	//		SegID = mSegmentIndexK[mainindex];
	//		Rvalue = SegID % (255*255);
	//		Gvalue = Rvalue / (255);
	//		Rvalue = Rvalue % 255;
	//		Bvalue = SegID / (255*255);
	//		/*Im_out[3*mainindex] = R_mean[mSegmentIndexK[mainindex]];
	//		Im_out[3*mainindex+1] = G_mean[mSegmentIndexK[mainindex]];
	//		Im_out[3*mainindex+2] = B_mean[mSegmentIndexK[mainindex]];*/
	//		Im_out[3*mainindex+2] = Rvalue;
	//		Im_out[3*mainindex+1] = Gvalue;
	//		Im_out[3*mainindex] = Bvalue;
	//	}
	//}

	/*for(int iter=0;iter<2;iter++)
	{
		for(int j=1;j<mH-1;j++)
		{
			for(int i=1;i<mW-1;i++)
			{
				if(iter==0)
				{
					Im_out[3*(i + mW*j)] = Im[3*(i + mW*j)];
					Im_out[3*(i + mW*j)+1] = Im[3*(i + mW*j)+1];
					Im_out[3*(i + mW*j)+2] = Im[3*(i + mW*j)+2];

					if(mOutput_Choice==1)
					{
						if(BorderMask[i + j*mW]==1 && BorderMask[i + (j+1)*mW]==1)
						{
							Im_out[3*(i + mW*j)] = 0;
							Im_out[3*(i + mW*j)+1] = 0;
							Im_out[3*(i + mW*j)+2] = 255;
							continue;
						}
						else
							BorderMask[i + j*mW] = 0;
					}
				}
			}
		}
		for(int j=1;j<mH-1;j++)
		{
			for(int i=1;i<mW-1;i++)
			{
				Im_out[3*(i + mW*j)] = Im_out[3*(i + mW*j)];
				Im_out[3*(i + mW*j)+1] = Im_out[3*(i + mW*j)+1];
				Im_out[3*(i + mW*j)+2] = Im_out[3*(i + mW*j)+2];

				if(j==11 && i==11)
					i=i;

				if(Im_out[3*(i + mW*j)] == 0 && Im_out[3*(i + mW*j)+1] == 0 && Im_out[3*(i + mW*j)+2] ==255)
				{
					if(BorderMask[i + j*mW]==1 && BorderMask[i+1 + j*mW]==0)
					{
						Im_out[3*(i + mW*j)] = Im[3*(i + mW*j)];
						Im_out[3*(i + mW*j)+1] = Im[3*(i + mW*j)+1];
						Im_out[3*(i + mW*j)+2] = Im[3*(i + mW*j)+2];
						continue;
					}
						
				}
			}
		}
	}*/

	/*for(int i=0;i<mSegmentNoK;i++)
	{
		int XX = (int)X_mean[i];
		int YY = (int)Y_mean[i];
		
		Im_out[3*(XX + mW*YY)] = 0;
		Im_out[3*(XX + mW*YY)+1] = 255;
		Im_out[3*(XX + mW*YY)+2] = 255;
	}*/

	//Segment_wrt_Variance(Im,Im_out);

	SegListK = new DynamicList[SegNo];
	for(int j=0;j<mH;j++)
	{
		for(int i=0;i<mW;i++)
		{
			A.xL = i;A.yL = j;
			int index = mSegmentIndexK[i+mW*j];
			
			SegListK[index].push_back(A);

		}
	}

	int mm;
	//for(int i=0;i<SegNo;i++)
	//{
	//	AA = SegDataK[i].at(0);
	//	for(int j=0;j<SegListK[i].size();j++)
	//	{
	//		A = SegListK[i].at(j);

	//		//if(A.xL+A.yL<1700 || A.xL+A.yL>2900)
	//		//{
	//		//	Im_out[3*(A.xL + mW*A.yL)] = 0;
	//		//	Im_out[3*(A.xL + mW*A.yL)+1] = 0;
	//		//	Im_out[3*(A.xL + mW*A.yL)+2] = 0;
	//		//	continue;
	//		//}
	//		
	//		//if(CandidateList[A.xL + mW*A.yL].size() > 1 ) continue;
	//		/*Im_out[3*(A.xL + mW*A.yL)] = AA.ImeanR;
	//		Im_out[3*(A.xL + mW*A.yL)+1] = AA.ImeanG;
	//		Im_out[3*(A.xL + mW*A.yL)+2] = AA.ImeanB;*/
	//		if(mOutput_Choice==1)
	//		{
	//			Im_out[3*(A.xL + mW*A.yL)] = Im[3*(A.xL + mW*A.yL)];
	//			Im_out[3*(A.xL + mW*A.yL)+1] = Im[3*(A.xL + mW*A.yL)+1];
	//			Im_out[3*(A.xL + mW*A.yL)+2] = Im[3*(A.xL + mW*A.yL)+2];
	//			if(BorderMask[A.xL + A.yL*mW]==1)
	//			{
	//				Im_out[3*(A.xL + mW*A.yL)] = 0;
	//				Im_out[3*(A.xL + mW*A.yL)+1] = 0;
	//				Im_out[3*(A.xL + mW*A.yL)+2] = 255;
	//				mSegmentIndexK[A.xL+mW*A.yL] = i;
	//				continue;
	//			}
	//		}
	//		else
	//		{
	//			Im_out[3*(A.xL + mW*A.yL)] = AA.ImeanR;
	//			Im_out[3*(A.xL + mW*A.yL)+1] = AA.ImeanG;
	//			Im_out[3*(A.xL + mW*A.yL)+2] = AA.ImeanB;
	//		}
	//	}
	//}
	
	/*for(int i=1;i<mW-1;i++)
	{
		for(int j=1;j<mH-1;j++)
		{
			if((BorderMask[i + j*mW]==1) && (BorderMask[i+1 + j*mW]==1) && (BorderMask[i + (j+1)*mW]==1))
				continue;

			else 
			{
				Im_out[3*(i + mW*j)] = Im[3*(i + mW*j)];
				Im_out[3*(i+ mW*j)+1] = Im[3*(i + mW*j)+1];
				Im_out[3*(i+ mW*j)+2] = Im[3*(i + mW*j)+2];
			}
		}
	}*/
	/*int *checker = new int[mW*mH];
	int cc=0;
	for(int i=0;i<mW*mH;i++)
		checker[i] = BorderMask[i];

	for(int i=1;i<mW-1;i++)
	{
		for(int j=1;j<mH-1;j++)
		{	
			if(BorderMask[i+j*mW]==1)
			{
				for(int ii=-1;ii<1;ii++)
				{
					for(int jj=-1;jj<1;jj++)
					{
						if(BorderMask[i+ii+(j+jj)*mW]==0)
						{
							checker[i+j*mW]=0;
							break;
						}
					}
				}
			}

		}
	}

	for(int i=1;i<mW-1;i++)
	{
		for(int j=1;j<mH-1;j++)
		{
			if(checker[i + j*mW]==1)
			{
				Im_out[3*(i + mW*j)] = 0;
				Im_out[3*(i + mW*j)+1] = 0;
				Im_out[3*(i + mW*j)+2] = 255;
			}
		}
	}
	delete [] BorderMask;
	delete [] CandidateList;
	delete [] checker;*/
	//free(EdgeMap);
	free(X_mean);
	free(Y_mean);
	free(R_mean);
	free(G_mean);
	free(B_mean);
	free(XY_total);
	free(RGB_total);
	free(one_over_size);
	free(CandidateList);
	free(Count_mean);
	//free(im_Lab);
}

void SegmentExtraction::find_borders_Kmeans(int *CandidateList)
{
	int checkin,segInd,count;

	memset(BorderMask,0, mW*mH*sizeof(int));

	mBorderCount = 0;

	for(int j=1;j<mH-1;j++)
	{
		for(int i=1;i<mW-1;i++)
		{
			segInd = mSegmentIndexK[j*mW + i];

			count =0;
			for(int k=-1;k<2;k++)
			{
				for(int m =-1;m<2;m++)
				{
					checkin = mSegmentIndexK[(j+k)*mW + i+m];
					
					if(checkin==segInd) continue;
					
					BorderMask[i + j*mW] = 1;
					count +=1;
					
					//CandidateList[i + j*mW+count*mW*mH] = checkin;
					CandidateList[(i + j*mW)*9+count] = checkin;
				}
			}
			
			CandidateList[i + j*mW] = count;

			if(BorderMask[i + j*mW]){
				mBorderList[2*mBorderCount] = i;
				mBorderList[2*mBorderCount+1] = j;
				mBorderCount++;}
		}
	}
}
void SegmentExtraction::find_borders_Kmeans2(int *CandidateList,int ind)
{
	int checkin,segInd,count;

	memset(BorderMask,0, mW*mH*sizeof(int));

	mBorderCount = 0;
	//int listX[4];
	//int listY[4];
	//if(ind%2==0){
	//listX[0] = -1;listX[1] = 0;listX[2] = 1;listX[3] = -1;
	//listY[0] = -1;listY[1] = -1;listY[2] = -1;listY[3] = 0;
	//}
	//else{
	//listX[0] = 1;listX[1] = 0;listX[2] = -1;listX[3] = 1;
	//listY[0] = 1;listY[1] = 1;listY[2] = 1;listY[3] = 0;
	//}

	for(int j=1;j<mH-1;j++)
	{
		for(int i=1;i<mW-1;i++)
		{
			segInd = mSegmentIndexK[j*mW + i];

			if(mSegChange[segInd]==false) continue;

			count =0;
		/*	for(int k=0;k<4;k++)
			{
				int x = listX[k];
				int y = listY[k];
				checkin = mSegmentIndexK[(j+y)*mW + x+i];

				if(checkin==segInd) continue;

				BorderMask[i + j*mW] = 1;
				count +=1;

				CandidateList[(i + j*mW)*9+count] = checkin;
			}*/
			for(int k=-1;k<2;k++)
			{
				for(int m =-1;m<2;m++)
				{
					checkin = mSegmentIndexK[(j+k)*mW + i+m];
					
					if(checkin==segInd) continue;
					
					BorderMask[i + j*mW] = 1;
					count +=1;
					
					//CandidateList[i + j*mW+count*mW*mH] = checkin;
					CandidateList[(i + j*mW)*9+count] = checkin;
				}
			}
			
			CandidateList[i + j*mW] = count;

			if(BorderMask[i + j*mW]){
				mBorderList[2*mBorderCount] = i;
				mBorderList[2*mBorderCount+1] = j;
				mBorderCount++;}
		}
	}
}

template <class T>
void SegmentExtraction::update_segments(T * Im,float* one_over_size)
{
	int Rval,Gval,Bval;
	int Xval,Yval,index;
	float t_size;

	for(int i=0;i<mSegmentNoK;i++)
	{
		t_size = one_over_size[(int)Count_mean[i]];

		R_mean[i] = RGB_total[i]*t_size;
		G_mean[i] = RGB_total[i+mSegmentNoK]*t_size;
		B_mean[i] = RGB_total[i+2*mSegmentNoK]*t_size;
		X_mean[i] = XY_total[i]*t_size;
		Y_mean[i] = XY_total[i+mSegmentNoK]*t_size;
	}
}

void SegmentExtraction::smooth_image(uchar * Im)
{
	uchar* TempIm = new uchar[3*mW*mH];
	int Rval,Gval,Bval,count,Thres;
	Thres = 40;
	
	for(int i=0;i<3*mH*mW;i++)
		TempIm[i] = Im[i];

	for(int i=0;i<mW;i++)
	{
		for(int j=0;j<mH;j++)
		{
			Rval = 0;Gval = 0;Bval = 0;count = 0;
			for(int ii=-2;ii<3;ii++)
			{
				for(int jj=-2;jj<3;jj++)
				{
					if((i+ii)<0 || (i+ii)>=mW || (j+jj)<0 || (j+jj)>=mH) continue;
					if(abs(TempIm[3*(i+j*mW)] - TempIm[3*((i+ii)+(j+jj)*mW)])>Thres || abs(TempIm[3*(i+j*mW)+1] - TempIm[3*((i+ii)+(j+jj)*mW)+1])>Thres 
						|| abs(TempIm[3*(i+j*mW)+2] - TempIm[3*((i+ii)+(j+jj)*mW)+2])>Thres) continue;

					Rval = Rval + TempIm[3*((i+ii)+(j+jj)*mW)];
					Gval = Gval + TempIm[3*((i+ii)+(j+jj)*mW)+1];
					Bval = Bval + TempIm[3*((i+ii)+(j+jj)*mW)+2];
					count = count + 1;
				}
			}
			Im[3*(i+j*mW)]   = Rval/count;
			Im[3*(i+j*mW)+1] = Gval/count;
			Im[3*(i+j*mW)+2] = Bval/count;
		}
	}

	delete [] TempIm;
}

void SegmentExtraction::find_bleeding(uchar * Im,float* BleedSum)
{
	float totalBleed;
	int index,count,Tcount,count2;

	Tcount = 0;
	count2 = 0;
	
	//memset(image2,0,3*mW*mH*sizeof(uchar));

	int LargeSegNo = 0;
	for(int i=0;i<mW*mH;i++)
	{
		if(Im[3*i]>LargeSegNo)
			LargeSegNo = Im[3*i];
	}
	float Bleed = 0;

	int * seg_hist = (int*)calloc((LargeSegNo+1),sizeof(int));
	for(int i=0;i<mW*mH;i++)
		seg_hist[Im[3*i]] +=1;

	for(int iter=0;iter<=LargeSegNo;iter++)
	{
		for(int i=0;i<mSegmentNoK;i++)
		{
			count = 0;
			//memset(checklist,0,mSegmentNoK*sizeof(int));
			for(int j=0;j<SegListK[i].size();j++)
			{
				A = SegListK[i].at(j);
				if(Im[3*(A.xL + mW*A.yL)] == iter)
					count +=1;
			} 
			if(count > SegListK[i].size()/20)
			{
				Bleed += SegListK[i].size();
			}
		}
	}

	totalBleed = (float)abs(Bleed - mW*mH) / (float)(mW*mH);

	BleedSum[0] += totalBleed;
	free(seg_hist);

	//printf ("Total bleed %f\n", totalBleed);
	//return totalBleed;
}

//TODO
//template <class T> 
void SegmentExtraction::find_bleeding_2(UINT* labels, uchar * Im,float* BleedSum,int mW,int mH,int segNo)
{
	node A;
	float coutAll = 0;

	DynamicList* SegListK = new DynamicList[segNo+1];
	int* ClearList = (int*)calloc(segNo+1,sizeof(int));
	int mSegmentNoK = segNo;
	for(int j=0;j<mH;j++)
	{
		for(int i=0;i<mW;i++)
		{
			A.xL = i;A.yL = j;
			int index = labels[i+mW*j];
			
			if(index>segNo) continue;
			SegListK[index].push_back(A);

			coutAll++;
		}
	}
	int t_index,indMax,MaxArea;

	float totalBleed;
	int index,count,Tcount,count2;

	Tcount = 0;
	count2 = 0;
	
	//memset(image2,0,3*mW*mH*sizeof(uchar));

	int LargeSegNo = 0;
	for(int i=0;i<mW*mH;i++)
	{
		if(Im[3*i]>LargeSegNo)
			LargeSegNo = Im[3*i];
	}
	float Bleed = 0;

	int * seg_hist = (int*)calloc((LargeSegNo+1),sizeof(int));
	for(int i=0;i<mW*mH;i++)
		seg_hist[Im[3*i]] +=1;

	for(int iter=0;iter<=LargeSegNo;iter++)
	{
		for(int i=0;i<mSegmentNoK;i++)
		{
			count = 0;
			//memset(checklist,0,mSegmentNoK*sizeof(int));
			for(int j=0;j<SegListK[i].size();j++)
			{
				A = SegListK[i].at(j);
				if(Im[3*(A.xL + mW*A.yL)] == iter)
					count +=1;
			} 
			if(count > SegListK[i].size()/20)
			{
				Bleed += SegListK[i].size();
			}
		}
	}

	totalBleed = (float)abs(Bleed - mW*mH) / (float)(coutAll);

	BleedSum[0] += totalBleed;
	free(seg_hist);

	//printf ("Total bleed %f\n", totalBleed);
	//return totalBleed;
}

void SegmentExtraction::find_bleeding_External(IplImage * ImRef,IplImage* Im,float* BleedSum)
{
	node A;
	int mW = Im->width;
	int mH = Im->height;
	int* labelMap = (int*)calloc(mW*mH,sizeof(int));
	int segNo = 0;
	for(int j=0;j<mH;j++){
		for(int i=0;i<mW;i++){
			labelMap[i+j*mW] = 255*(int)p3(Im,j,i,1) + p3(Im,j,i,2);
			
			if(labelMap[i+j*mW] > segNo)
				segNo = labelMap[i+j*mW];
		}
	}
	for(int i=0;i<mW*mH;i++){
		if(labelMap[i] > segNo)
			segNo = labelMap[i];
	}
	DynamicList* SegListK = new DynamicList[segNo+1];
	int* ClearList = (int*)calloc(segNo+1,sizeof(int));
	int mSegmentNoK = segNo;
	for(int j=0;j<mH;j++)
	{
		for(int i=0;i<mW;i++)
		{
			A.xL = i;A.yL = j;
			int index = labelMap[i+mW*j];
			
			SegListK[index].push_back(A);
		}
	}
	int t_index,indMax,MaxArea;
	//for(int j=0;j<mH;j++)
	//{
	//	for(int i=0;i<mW;i++)
	//	{
	//		A.xL = i;A.yL = j;
	//		int index = labelMap[i+mW*j];
	//		
	//		if(SegListK[index].size() < 15){
	//			MaxArea = 0;
	//			ClearList[index] = 1;
	//			for(int ii=-1;ii<2;ii++){
	//				for(int jj=-1;jj<2;jj++){
	//					if(SegListK[labelMap[i+jj+mW*(j+ii)]].size() > MaxArea){
	//						MaxArea = SegListK[labelMap[i+jj+mW*(j+ii)]].size();
	//						indMax = labelMap[i+jj+mW*(j+ii)];
	//					}
	//				}
	//			}
	//			if(MaxArea >= 15){
	//				//labelMap[i+mW*j] = indMax;
	//				//SegListK[indMax].push_back(A);
	//			}
	//		}

	//	}
	//}

	float totalBleed;
	int index,count,Tcount,count2;

	Tcount = 0;
	count2 = 0;
	
	//memset(image2,0,3*mW*mH*sizeof(uchar));

	int LargeSegNo = 0;
	for(int i=0;i<mH;i++){
		for(int j=0;j<mW;j++){
		if(p3(ImRef,i,j,0)>LargeSegNo)
			LargeSegNo = p3(ImRef,i,j,0);
		}
	}
	float Bleed = 0;

	int * seg_hist = (int*)calloc((LargeSegNo+1),sizeof(int));
	for(int i=0;i<mH;i++){
		for(int j=0;j<mW;j++){
		seg_hist[p3(ImRef,i,j,0)] +=1;
		}
	}

	for(int iter=0;iter<=LargeSegNo;iter++)
	{
		for(int i=0;i<mSegmentNoK;i++)
		{
			//if(SegListK[i].size() < 25) continue;
			count = 0;
			//memset(checklist,0,mSegmentNoK*sizeof(int));
			for(int j=0;j<SegListK[i].size();j++)
			{
				A = SegListK[i].at(j);
				if(p3(ImRef,A.yL,A.xL,0) == iter)
					count +=1;
			} 
			if(count > SegListK[i].size()/20)
			{
				Bleed += SegListK[i].size();
			}
		}
	}

	totalBleed = (float)abs(Bleed - mW*mH) / (float)(mW*mH);

	//printf ("Bleeding %f\n", totalBleed);
	BleedSum[0] += totalBleed;
	BleedSum[1] += segNo;
	free(seg_hist);
	free(ClearList);
	free(labelMap);
	delete [] SegListK;
}

void SegmentExtraction::find_edgeness(uchar *Im)
{
	int SegNo = mSegmentNoK;
	int step = sqrt(double(mW*mH/SegNo));
	int ColNo = mW/step;
	int RowNo = mH/step;
	SegNo = ColNo*RowNo;
	mSegmentNoK = SegNo;
	
	int kernel[5][5][4];
	kernel[0][0][0]=-1;kernel[1][0][0]=-2;kernel[2][0][0]=0;kernel[3][0][0]=2;kernel[4][0][0]=1;
	kernel[0][1][0]=-1;kernel[1][1][0]=-2;kernel[2][1][0]=0;kernel[3][1][0]=2;kernel[4][1][0]=1;
	kernel[0][2][0]=-1;kernel[1][2][0]=-2;kernel[2][2][0]=0;kernel[3][2][0]=2;kernel[4][2][0]=1;
	kernel[0][3][0]=-1;kernel[1][3][0]=-2;kernel[2][3][0]=0;kernel[3][3][0]=2;kernel[4][3][0]=1;
	kernel[0][4][0]=-1;kernel[1][4][0]=-2;kernel[2][4][0]=0;kernel[3][4][0]=2;kernel[4][4][0]=1;

	kernel[0][0][1]=-1;kernel[1][0][1]=-1;kernel[2][0][1]=-2;kernel[3][0][1]=-2;kernel[4][0][1]=0;
	kernel[0][1][1]=-1;kernel[1][1][1]=-2;kernel[2][1][1]=-2;kernel[3][1][1]=0;kernel[4][1][1]=2;
	kernel[0][2][1]=-2;kernel[1][2][1]=-2;kernel[2][2][1]=0;kernel[3][2][1]=2;kernel[4][2][1]=2;
	kernel[0][3][1]=-2;kernel[1][3][1]=0;kernel[2][3][1]=2;kernel[3][3][1]=2;kernel[4][3][1]=1;
	kernel[0][4][1]=0;kernel[1][4][1]=2;kernel[2][4][1]=2;kernel[3][4][1]=1;kernel[4][4][1]=1;

	kernel[0][0][2]=-1;kernel[1][0][2]=-1;kernel[2][0][2]=-1;kernel[3][0][2]=-1;kernel[4][0][2]=-1;
	kernel[0][1][2]=-2;kernel[1][1][2]=-2;kernel[2][1][2]=-2;kernel[3][1][2]=-2;kernel[4][1][2]=-2;
	kernel[0][2][2]=0;kernel[1][2][2]=0;kernel[2][2][2]=0;kernel[3][2][2]=0;kernel[4][2][2]=0;
	kernel[0][3][2]=1;kernel[1][3][2]=1;kernel[2][3][2]=1;kernel[3][3][2]=1;kernel[4][3][2]=1;
	kernel[0][4][2]=2;kernel[1][4][2]=2;kernel[2][4][2]=2;kernel[3][4][2]=2;kernel[4][4][2]=2;

	kernel[0][0][3]=0;kernel[1][0][3]=-2;kernel[2][0][3]=-2;kernel[3][0][3]=-1;kernel[4][0][3]=-1;
	kernel[0][1][3]=2;kernel[1][1][3]=0;kernel[2][1][3]=-2;kernel[3][1][3]=-2;kernel[4][1][3]=-1;
	kernel[0][2][3]=2;kernel[1][2][3]=2;kernel[2][2][3]=0;kernel[3][2][3]=-2;kernel[4][2][3]=-2;
	kernel[0][3][3]=1;kernel[1][3][3]=2;kernel[2][3][3]=2;kernel[3][3][3]=0;kernel[4][3][3]=-2;
	kernel[0][4][3]=1;kernel[1][4][3]=1;kernel[2][4][3]=2;kernel[3][4][3]=2;kernel[4][4][3]=0;

	int sumR,sumG,sumB,Maxsum,val,x,y;
	int Threshold = 600;
	memset(EdgeMap,-1,ColNo*RowNo*sizeof(int));

	count = 0;
	for(int i=1;i<ColNo-1;i++)
	{
		for(int j=1;j<RowNo-1;j++)
		{
			x = i*step + step/2;
			y = j*step + step/2;
			Maxsum = 0;
			val = 0;
			for(int k =0;k<4;k++)
			{
				sumR = 0;sumG = 0;sumB = 0;
				for(int ii=-2;ii<3;ii++)
				{
					for(int jj=-2;jj<3;jj++)
					{
						sumR = sumR + kernel[ii+2][jj+2][k]*Im[3*(x+ii + (y+jj)*mW)];
						sumG = sumG + kernel[ii+2][jj+2][k]*Im[3*(x+ii + (y+jj)*mW)+1];
						sumB = sumB + kernel[ii+2][jj+2][k]*Im[3*(x+ii + (y+jj)*mW)+2];
					}
				}
				sumR = abs(sumR) + abs(sumG) + abs(sumB);
				
				if(sumR>Maxsum)
				{
					val = k;
					Maxsum = sumR;
				}
			}
			count += 1;
			if(Maxsum>Threshold)
			{
				EdgeMap[i+j*ColNo] = val;
				mSegmentNoK = mSegmentNoK+1;
				count += 1;
			}
			if(count==1000)
				count = 1000;
		}
	}
}
template <class T>
void SegmentExtraction::initialize(T *Im,int ColNo, int RowNo, int step,float* one_over_val)
{
	int Rval,Gval,Bval,limit,index;
	int segcounter = 0;
	int xsum,ysum;
	float scale;
	count = 0;

	//memset(EdgeMap,-1,ColNo*RowNo*sizeof(int));
	for(int j=0;j<RowNo;j++)
	{
		for(int i=0;i<ColNo;i++)
		{
			//if(EdgeMap[i+j*ColNo]==-1) //no boundary-edge problem
			{
				if(i<ColNo-1 && j<RowNo-1)
				{
					Rval = 0;Gval = 0;Bval = 0;
					xsum = 0;ysum = 0;
					for(int ii=step*i;ii<step*(i+1);ii++)
					{
						for(int jj=step*j;jj<step*(j+1);jj++)
						{
							Rval = Rval + Im[3*(ii + jj*mW)];
							Gval = Gval + Im[3*(ii + jj*mW)+1];
							Bval = Bval + Im[3*(ii + jj*mW)+2];
							xsum += ii;
							ysum +=jj;
							mSegmentIndexK[ii+mW*jj]=segcounter;//segment indexes
						}
					}
					RGB_total[segcounter] = Rval;
					RGB_total[segcounter+mSegmentNoK] = Gval;
					RGB_total[segcounter+2*mSegmentNoK] = Bval;
					XY_total[segcounter] = xsum;
					XY_total[segcounter+mSegmentNoK] = ysum;

					scale = one_over_val[step*step];
					R_mean[segcounter] = Rval*scale;
					G_mean[segcounter] = Gval*scale;
					B_mean[segcounter] = Bval*scale;
					Count_mean[segcounter] = (step*step);
					X_mean[segcounter] = i*step + step/2;
					Y_mean[segcounter] = j*step + step/2;
					segcounter +=1;
					continue;
				}
				else if((i==ColNo-1))
				{
					limit = step*(j+1);
					if(j==RowNo-1)
						limit = mH;

					Rval = 0;Gval = 0;Bval = 0;
					xsum = 0;ysum = 0;
					for(int ii=step*i;ii<mW;ii++)
					{
						for(int jj=step*j;jj<limit;jj++)
						{
							Rval = Rval + Im[3*(ii + jj*mW)];
							Gval = Gval + Im[3*(ii + jj*mW)+1];
							Bval = Bval + Im[3*(ii + jj*mW)+2];
							xsum += ii;
							ysum +=jj;
							mSegmentIndexK[ii+mW*jj]=segcounter;//segment indexes
						}
					}
					
					RGB_total[segcounter] = Rval;
					RGB_total[segcounter+mSegmentNoK] = Gval;
					RGB_total[segcounter+2*mSegmentNoK] = Bval;
					XY_total[segcounter] = xsum;
					XY_total[segcounter+mSegmentNoK] = ysum;

					scale = one_over_val[step*(mW-step*i)];
					R_mean[segcounter] = Rval*scale;
					G_mean[segcounter] = Gval*scale;
					B_mean[segcounter] = Bval*scale;
					Count_mean[segcounter] = (step*(mW-step*i));
					X_mean[segcounter] = i*step + step/2;
					Y_mean[segcounter] = j*step + step/2;

					segcounter +=1; 
					continue;
				}
				else
				{
					Rval = 0;Gval = 0;Bval = 0;
					xsum = 0;ysum = 0;
					for(int ii=step*i;ii<step*(i+1);ii++)
					{
						for(int jj=step*j;jj<mH;jj++)
						{
							Rval = Rval + Im[3*(ii + jj*mW)];
							Gval = Gval + Im[3*(ii + jj*mW)+1];
							Bval = Bval + Im[3*(ii + jj*mW)+2];
							xsum += ii;
							ysum +=jj;
							mSegmentIndexK[ii+mW*jj]=segcounter;//segment indexes
						}
					}

					RGB_total[segcounter] = Rval;
					RGB_total[segcounter+mSegmentNoK] = Gval;
					RGB_total[segcounter+2*mSegmentNoK] = Bval;
					XY_total[segcounter] = xsum;
					XY_total[segcounter+mSegmentNoK] = ysum;

					scale = one_over_val[step*(mH-step*j)];
					R_mean[segcounter] = Rval*scale;
					R_mean[segcounter] = Gval*scale;
					G_mean[segcounter] = Bval*scale;
					Count_mean[segcounter] = (step*(mH-step*j));
					X_mean[segcounter] = i*step + step/2;
					Y_mean[segcounter] = j*step + step/2;

					segcounter +=1;
					continue;
				}
				
			}
			//else
			//{
			//	if(EdgeMap[i+j*ColNo]==0)
			//	{
			//		///// right part
			//		//AA.Xmean = i*step + step/4;
			//		//AA.Ymean = j*step + step/2;
			//		Rval = 0;Gval = 0;Bval = 0;
			//		for(int ii=step*i;ii<(step*(i+1/2));ii++)
			//		{
			//			for(int jj=step*j;jj<step*(j+1);jj++)
			//			{
			//				Rval = Rval + Im[3*(ii + jj*mW)];
			//				Gval = Gval + Im[3*(ii + jj*mW)+1];
			//				Bval = Bval + Im[3*(ii + jj*mW)+2];
			//				//A.xL = ii;
			//				//A.yL = jj;
			//				//SegListK[segcounter].push_back(A);
			//				mSegmentIndexK[ii+mW*jj]=segcounter;//segment indexes
			//			}
			//		}
			//		/*AA.ImeanR = Rval/(step*step/2);
			//		AA.ImeanG = Gval/(step*step/2);
			//		AA.ImeanB = Bval/(step*step/2);
			//		SegDataK[segcounter].push_back(AA);*/

			//		RGB_total[segcounter] = Rval;
			//		RGB_total[segcounter+mSegmentNoK] = Gval;
			//		RGB_total[segcounter+2*mSegmentNoK] = Bval;

			//		RGB_mean[segcounter] = Rval/(step*step/2);
			//		RGB_mean[segcounter+mSegmentNoK] = Gval/(step*step/2);
			//		RGB_mean[segcounter+2*mSegmentNoK] = Bval/(step*step/2);
			//		RGB_mean[segcounter+3*mSegmentNoK] = (step*step/2);
			//		XY_mean[segcounter] = i*step + step/4;
			//		XY_mean[segcounter+mSegmentNoK] = j*step + step/2;
			//		segcounter +=1;

			//		///  left part 
			//		//AA.Xmean = i*step + 3*step/4;
			//		//AA.Ymean = j*step + step/2;
			//		Rval = 0;Gval = 0;Bval = 0;
			//		for(int ii=step*(i+1/2);ii<(step*(i+1));ii++)
			//		{
			//			for(int jj=step*j;jj<step*(j+1);jj++)
			//			{
			//				Rval = Rval + Im[3*(ii + jj*mW)];
			//				Gval = Gval + Im[3*(ii + jj*mW)+1];
			//				Bval = Bval + Im[3*(ii + jj*mW)+2];
			//				//A.xL = ii;
			//				//A.yL = jj;
			//				//SegListK[segcounter].push_back(A);
			//				mSegmentIndexK[ii+mW*jj]=segcounter;//segment indexes
			//			}
			//		}
			//	/*	AA.ImeanR = Rval/(step*step/2);
			//		AA.ImeanG = Gval/(step*step/2);
			//		AA.ImeanB = Bval/(step*step/2);
			//		SegDataK[segcounter].push_back(AA);*/

			//		RGB_total[segcounter] = Rval;
			//		RGB_total[segcounter+mSegmentNoK] = Gval;
			//		RGB_total[segcounter+2*mSegmentNoK] = Bval;

			//		RGB_mean[segcounter] = Rval/(step*step/2);
			//		RGB_mean[segcounter+mSegmentNoK] = Gval/(step*step/2);
			//		RGB_mean[segcounter+2*mSegmentNoK] = Bval/(step*step/2);
			//		RGB_mean[segcounter+3*mSegmentNoK] = (step*step/2);
			//		XY_mean[segcounter] = i*step + 3*step/4;
			//		XY_mean[segcounter+mSegmentNoK] = j*step + step/2;
			//		segcounter +=1;
			//	}

			//	else if(EdgeMap[i+j*ColNo]==1)
			//	{
			//		///// left upper part
			//		//AA.Xmean = i*step + step/4;
			//		//AA.Ymean = j*step + step/4;
			//		Rval = 0;Gval = 0;Bval = 0;
			//		for(int ii=step*i;ii<(step*(i+1));ii++)
			//		{
			//			for(int jj=step*j;jj<step*(j+1)+step*i-ii;jj++)
			//			{
			//				Rval = Rval + Im[3*(ii + jj*mW)];
			//				Gval = Gval + Im[3*(ii + jj*mW)+1];
			//				Bval = Bval + Im[3*(ii + jj*mW)+2];
			//				//A.xL = ii;
			//				//A.yL = jj;
			//				//SegListK[segcounter].push_back(A);
			//				mSegmentIndexK[ii+mW*jj]=segcounter;//segment indexes
			//			}
			//		}
			//	/*	AA.ImeanR = Rval/(step*step/2);
			//		AA.ImeanG = Gval/(step*step/2);
			//		AA.ImeanB = Bval/(step*step/2);
			//		SegDataK[segcounter].push_back(AA);*/

			//		RGB_total[segcounter] = Rval;
			//		RGB_total[segcounter+mSegmentNoK] = Gval;
			//		RGB_total[segcounter+2*mSegmentNoK] = Bval;

			//		RGB_mean[segcounter] = Rval/(step*step/2);
			//		RGB_mean[segcounter+mSegmentNoK] = Gval/(step*step/2);
			//		RGB_mean[segcounter+2*mSegmentNoK] = Bval/(step*step/2);
			//		RGB_mean[segcounter+3*mSegmentNoK] = (step*step/2);
			//		XY_mean[segcounter] = i*step + step/4;
			//		XY_mean[segcounter+mSegmentNoK] = j*step + step/4;
			//		segcounter +=1;
			//		///  right down part 
			//		//AA.Xmean = i*step + 3*step/4;
			//		//AA.Ymean = j*step + 3*step/4;
			//		Rval = 0;Gval = 0;Bval = 0;
			//		for(int ii=step*(i+1);ii>=(step*i);ii--)
			//		{
			//			for(int jj=step*j+step*(i+1)-ii;jj<step*(j+1);jj++)
			//			{
			//				Rval = Rval + Im[3*(ii + jj*mW)];
			//				Gval = Gval + Im[3*(ii + jj*mW)+1];
			//				Bval = Bval + Im[3*(ii + jj*mW)+2];
			//				//A.xL = ii;
			//				//A.yL = jj;
			//				//SegListK[segcounter].push_back(A);
			//				mSegmentIndexK[ii+mW*jj]=segcounter;//segment indexes
			//			}
			//		}
			//	/*	AA.ImeanR = Rval/(step*step/2);
			//		AA.ImeanG = Gval/(step*step/2);
			//		AA.ImeanB = Bval/(step*step/2);
			//		SegDataK[segcounter].push_back(AA);*/
			//		RGB_total[segcounter] = Rval;
			//		RGB_total[segcounter+mSegmentNoK] = Gval;
			//		RGB_total[segcounter+2*mSegmentNoK] = Bval;

			//		RGB_mean[segcounter] = Rval/(step*step/2);
			//		RGB_mean[segcounter+mSegmentNoK] = Gval/(step*step/2);
			//		RGB_mean[segcounter+2*mSegmentNoK] = Bval/(step*step/2);
			//		RGB_mean[segcounter+3*mSegmentNoK] = (step*step/2);
			//		XY_mean[segcounter] = i*step + 3*step/4;
			//		XY_mean[segcounter+mSegmentNoK] = j*step + 3*step/4;
			//		segcounter +=1;
			//	}

			//	else if(EdgeMap[i+j*ColNo]==2)
			//	{
			//		///// up part
			//		//AA.Xmean = i*step + step/2;
			//		//AA.Ymean = j*step + step/4;
			//		Rval = 0;Gval = 0;Bval = 0;
			//		for(int ii=step*i;ii<(step*(i+1));ii++)
			//		{
			//			for(int jj=step*j;jj<(step*(j+1/2));jj++)
			//			{
			//				Rval = Rval + Im[3*(ii + jj*mW)];
			//				Gval = Gval + Im[3*(ii + jj*mW)+1];
			//				Bval = Bval + Im[3*(ii + jj*mW)+2];
			//				//A.xL = ii;
			//				//A.yL = jj;
			//				//SegListK[segcounter].push_back(A);
			//				mSegmentIndexK[ii+mW*jj]=segcounter;//segment indexes
			//			}
			//		}
			///*		AA.ImeanR = Rval/(step*step/2);
			//		AA.ImeanG = Gval/(step*step/2);
			//		AA.ImeanB = Bval/(step*step/2);
			//		SegDataK[segcounter].push_back(AA);*/
			//		RGB_total[segcounter] = Rval;
			//		RGB_total[segcounter+mSegmentNoK] = Gval;
			//		RGB_total[segcounter+2*mSegmentNoK] = Bval;

			//		RGB_mean[segcounter] = Rval/(step*step/2);
			//		RGB_mean[segcounter+mSegmentNoK] = Gval/(step*step/2);
			//		RGB_mean[segcounter+2*mSegmentNoK] = Bval/(step*step/2);
			//		RGB_mean[segcounter+3*mSegmentNoK] = (step*step/2);
			//		XY_mean[segcounter] = i*step + step/2;
			//		XY_mean[segcounter+mSegmentNoK] = j*step + step/4;
			//		segcounter +=1;
			//		///  down part 
			//		//AA.Xmean = i*step + step/2;
			//		//AA.Ymean = j*step + 3*step/4;
			//		Rval = 0;Gval = 0;Bval = 0;
			//		for(int ii=step*i;ii<(step*(i+1));ii++)
			//		{
			//			for(int jj=(step*(j+1/2));jj<step*(j+1);jj++)
			//			{
			//				Rval = Rval + Im[3*(ii + jj*mW)];
			//				Gval = Gval + Im[3*(ii + jj*mW)+1];
			//				Bval = Bval + Im[3*(ii + jj*mW)+2];
			//				//A.xL = ii;
			//				//A.yL = jj;
			//				//SegListK[segcounter].push_back(A);
			//				mSegmentIndexK[ii + jj*mW]=segcounter;//segment indexes
			//			}
			//		}
			//	/*	AA.ImeanR = Rval/(step*step/2);
			//		AA.ImeanG = Gval/(step*step/2);
			//		AA.ImeanB = Bval/(step*step/2);
			//		SegDataK[segcounter].push_back(AA);*/
			//		RGB_total[segcounter] = Rval;
			//		RGB_total[segcounter+mSegmentNoK] = Gval;
			//		RGB_total[segcounter+2*mSegmentNoK] = Bval;

			//		RGB_mean[segcounter] = Rval/(step*step/2);
			//		RGB_mean[segcounter+mSegmentNoK] = Gval/(step*step/2);
			//		RGB_mean[segcounter+2*mSegmentNoK] = Bval/(step*step/2);
			//		RGB_mean[segcounter+3*mSegmentNoK] = (step*step/2);
			//		XY_mean[segcounter] = i*step + step/2;
			//		XY_mean[segcounter+mSegmentNoK] = j*step + 3*step/4;
			//		segcounter +=1;
			//	}

			//	else
			//	{
			//		///// right upper part
			//		//AA.Xmean = i*step + 3*step/4;
			//		//AA.Ymean = j*step + step/4;
			//		Rval = 0;Gval = 0;Bval = 0;
			//		for(int ii=step*i;ii<(step*(i+1));ii++)
			//		{
			//			for(int jj=step*j;jj<step*j+ii-step*i;jj++)
			//			{
			//				Rval = Rval + Im[3*(ii + jj*mW)];
			//				Gval = Gval + Im[3*(ii + jj*mW)+1];
			//				Bval = Bval + Im[3*(ii + jj*mW)+2];
			//				//A.xL = ii;
			//				//A.yL = jj;
			//				//SegListK[segcounter].push_back(A);
			//				mSegmentIndexK[ii+mW*jj]=segcounter;//segment indexes
			//			}
			//		}
			//	/*	AA.ImeanR = Rval/(step*step/2);
			//		AA.ImeanG = Gval/(step*step/2);
			//		AA.ImeanB = Bval/(step*step/2);
			//		SegDataK[segcounter].push_back(AA);*/
			//		RGB_total[segcounter] = Rval;
			//		RGB_total[segcounter+mSegmentNoK] = Gval;
			//		RGB_total[segcounter+2*mSegmentNoK] = Bval;

			//		RGB_mean[segcounter] = Rval/(step*step/2);
			//		RGB_mean[segcounter+mSegmentNoK] = Gval/(step*step/2);
			//		RGB_mean[segcounter+2*mSegmentNoK] = Bval/(step*step/2);
			//		RGB_mean[segcounter+3*mSegmentNoK] = (step*step/2);
			//		XY_mean[segcounter] = i*step + 3*step/4;
			//		XY_mean[segcounter+mSegmentNoK] = j*step + step/4;
			//		segcounter +=1;
			//		///  left down part 
			//		//AA.Xmean = i*step + step/4;
			//		//AA.Ymean = j*step + 3*step/4;
			//		Rval = 0;Gval = 0;Bval = 0;
			//		for(int ii=step*(i+1);ii>=(step*i);ii--)
			//		{
			//			for(int jj=step*j-step*(i)+ii;jj<step*(j+1);jj++)
			//			{
			//				Rval = Rval + Im[3*(ii + jj*mW)];
			//				Gval = Gval + Im[3*(ii + jj*mW)+1];
			//				Bval = Bval + Im[3*(ii + jj*mW)+2];
			//				//A.xL = ii;
			//				//A.yL = jj;
			//				//SegListK[segcounter].push_back(A);
			//				mSegmentIndexK[ii + jj*mW]=segcounter;//segment indexes
			//			}
			//		}
			//		/*AA.ImeanR = Rval/(step*step/2);
			//		AA.ImeanG = Gval/(step*step/2);
			//		AA.ImeanB = Bval/(step*step/2);
			//		SegDataK[segcounter].push_back(AA);*/
			//		RGB_total[segcounter] = Rval;
			//		RGB_total[segcounter+mSegmentNoK] = Gval;
			//		RGB_total[segcounter+2*mSegmentNoK] = Bval;

			//		RGB_mean[segcounter] = Rval/(step*step/2);
			//		RGB_mean[segcounter+mSegmentNoK] = Gval/(step*step/2);
			//		RGB_mean[segcounter+2*mSegmentNoK] = Bval/(step*step/2);
			//		RGB_mean[segcounter+3*mSegmentNoK] = (step*step/2);
			//		XY_mean[segcounter] = i*step + step/4;
			//		XY_mean[segcounter+mSegmentNoK] = j*step + 3*step/4;
			//		segcounter +=1;
			//	}
			//}
		}
	}
	mSegmentNoK = segcounter;
}
void SegmentExtraction::smooth_saliency(IplImage * Im)
{
	int sum = 0;
	
	for(int i=0;i<mSegmentNoK;i++)
	{
		AA = SegDataK[i].at(0);
		sum = 0;
		for(int j=0;j<SegListK[i].size();j++)
		{
			A = SegListK[i].at(j);

			sum += p3(Im,A.yL,A.xL,0);
		}
		sum = sum / SegListK[i].size();
		
		for(int j=0;j<SegListK[i].size();j++)
		{
			A = SegListK[i].at(j);

			p3(Im,A.yL,A.xL,0) = sum;
			p3(Im,A.yL,A.xL,1) = sum;
			p3(Im,A.yL,A.xL,2) = sum;
		}
	}	
}
void SegmentExtraction::transfer_data(int* labelMap,int segNo)
{
	SegListK = new DynamicList[segNo];
	mSegmentNoK = segNo;
	for(int j=0;j<mH;j++)
	{
		for(int i=0;i<mW;i++)
		{
			A.xL = i;A.yL = j;
			int index = labelMap[i+mW*j];
			
			SegListK[index].push_back(A);

		}
	}
}

void SegmentExtraction::find_border_Recall(uchar * Im,float* Border_Recall/*, bool *boundaryMap*/) {
	float boundaryRecall;
	int index,count,Tcount,count2;

	Tcount = 0;
	count2 = 0;
	
	//memset(image2,0,3*mW*mH*sizeof(uchar));

	int LargeSegNo = 0;
	for(int i=0;i<mW*mH;i++)
	{
		if(Im[3*i]>LargeSegNo)
			LargeSegNo = Im[3*i];
	}
	float Bleed = 0;
	int check,check_bound;
	float check_sum = 0;
	float bound_sum = 0;
	for(int j=2;j<mH-2;j++)
	{
		for(int i=2;i<mW-2;i++)
		{
			check = 0;
			check_bound = 0;
			for(int ii=-1;ii<2;ii++)
			{
				for(int jj=-1;jj<2;jj++)
				{
					if(abs(Im[3*(i+j*mW)]-Im[3*((i+ii)+(j+jj)*mW)]) > 0)
					{
						check = 1;
						break;
					}
				}
			}
			if(check)
			{
				check_sum +=1;
				for(int ii=-2;ii<3;ii++)
				{
					for(int jj=-2;jj<3;jj++)
					{
						if(BorderMask[i+ii+(j+jj)*mW])
						{
							check_bound = 1;
							break;
						}
					}
				}
			}
			if(check_bound)
				bound_sum +=1;
		}
	}

	boundaryRecall = bound_sum / check_sum;

	Border_Recall[0] += boundaryRecall;
	
	//delete [] SegListK;

	//printf ("Total bleed %f\n", totalBleed);
	//return totalBleed;
}
//template <class T> 
void SegmentExtraction::find_border_Recall_2(UINT* labelMap,uchar * Im,float* Border_Recall,int mW,int mH,int segNo)
{
	node A;
	float coutAll=0;
	DynamicList* SegListK = new DynamicList[segNo+1];
	int* ClearList = (int*)calloc(segNo+1,sizeof(int));
	int mSegmentNoK = segNo;
	for(int j=0;j<mH;j++)
	{
		for(int i=0;i<mW;i++)
		{
			A.xL = i;A.yL = j;
			int index = labelMap[i+mW*j];
			
						
			if(index>segNo) continue;
			SegListK[index].push_back(A);

			coutAll++;
		}
	}
	int t_index,indMax,MaxArea;

	int checkin,segInd,count;

	int* BorderMask = (int*)calloc(mW*mH,sizeof(int));
	
	for(int j=1;j<mH-1;j++)
	{
		for(int i=1;i<mW-1;i++)
		{
			segInd = labelMap[j*mW + i];

			for(int k=-1;k<2;k++)
			{
				for(int m =-1;m<2;m++)
				{
					checkin = labelMap[(j+k)*mW + i+m];
					
					if(checkin==segInd) continue;
					
					BorderMask[i + j*mW] = 1;
				}
			}
		}
	}

	/////
	float boundaryRecall;
	int index,Tcount,count2;

	Tcount = 0;
	count2 = 0;
	
	//memset(image2,0,3*mW*mH*sizeof(uchar));

	int LargeSegNo = 0;
	for(int i=0;i<mW*mH;i++)
	{
		if(Im[3*i]>LargeSegNo)
			LargeSegNo = Im[3*i];
	}
	float Bleed = 0;
	int check,check_bound;
	float check_sum = 0;
	float bound_sum = 0;
	for(int j=2;j<mH-2;j++)
	{
		for(int i=2;i<mW-2;i++)
		{
			check = 0;
			check_bound = 0;
			for(int ii=-1;ii<2;ii++)
			{
				for(int jj=-1;jj<2;jj++)
				{
					if(abs(Im[3*(i+j*mW)]-Im[3*((i+ii)+(j+jj)*mW)]) > 0)
					{
						check = 1;
						break;
					}
				}
			}
			if(check)
			{
				check_sum +=1;
				for(int ii=-2;ii<3;ii++)
				{
					for(int jj=-2;jj<3;jj++)
					{
						if(BorderMask[i+ii+(j+jj)*mW])
						{
							check_bound = 1;
							break;
						}
					}
				}
			}
			if(check_bound)
				bound_sum +=1;
		}
	}

	boundaryRecall = bound_sum / check_sum;

	Border_Recall[0] += boundaryRecall;
	
	//delete [] SegListK;

	//printf ("Total bleed %f\n", totalBleed);
	//return totalBleed;
	free(BorderMask);
	free(ClearList);
	delete [] SegListK;
	
}
void SegmentExtraction::find_border_Recall_External(uchar * Im,IplImage* Image,float* Border_Recall)
{
	node A;
	int mW = Image->width;
	int mH = Image->height;
	int* labelMap = (int*)calloc(mW*mH,sizeof(int));
	int segNo = 0;
	for(int j=0;j<mH;j++){
		for(int i=0;i<mW;i++){
			labelMap[i+j*mW] = 255*(int)p3(Image,j,i,1) + p3(Image,j,i,2);
			
			if(labelMap[i+j*mW] > segNo)
				segNo = labelMap[i+j*mW];
		}
	}
	for(int i=0;i<mW*mH;i++){
		if(labelMap[i] > segNo)
			segNo = labelMap[i];
	}
	DynamicList* SegListK = new DynamicList[segNo+1];
	int* ClearList = (int*)calloc(segNo+1,sizeof(int));
	int mSegmentNoK = segNo;
	for(int j=0;j<mH;j++)
	{
		for(int i=0;i<mW;i++)
		{
			A.xL = i;A.yL = j;
			int index = labelMap[i+mW*j];
			
			SegListK[index].push_back(A);
		}
	}
	int t_index,indMax,MaxArea;

	int checkin,segInd,count;

	int* BorderMask = (int*)calloc(mW*mH,sizeof(int));
	
	for(int j=1;j<mH-1;j++)
	{
		for(int i=1;i<mW-1;i++)
		{
			segInd = labelMap[j*mW + i];

			for(int k=-1;k<2;k++)
			{
				for(int m =-1;m<2;m++)
				{
					checkin = labelMap[(j+k)*mW + i+m];
					
					if(checkin==segInd) continue;
					
					BorderMask[i + j*mW] = 1;
				}
			}
		}
	}

	/////
	float boundaryRecall;
	int index,Tcount,count2;

	Tcount = 0;
	count2 = 0;
	
	//memset(image2,0,3*mW*mH*sizeof(uchar));

	int LargeSegNo = 0;
	for(int i=0;i<mW*mH;i++)
	{
		if(Im[3*i]>LargeSegNo)
			LargeSegNo = Im[3*i];
	}
	float Bleed = 0;
	int check,check_bound;
	float check_sum = 0;
	float bound_sum = 0;
	for(int j=2;j<mH-2;j++)
	{
		for(int i=2;i<mW-2;i++)
		{
			check = 0;
			check_bound = 0;
			for(int ii=-1;ii<2;ii++)
			{
				for(int jj=-1;jj<2;jj++)
				{
					if(abs(Im[3*(i+j*mW)]-Im[3*((i+ii)+(j+jj)*mW)]) > 0)
					{
						check = 1;
						break;
					}
				}
			}
			if(check)
			{
				check_sum +=1;
				for(int ii=-2;ii<3;ii++)
				{
					for(int jj=-2;jj<3;jj++)
					{
						if(BorderMask[i+ii+(j+jj)*mW])
						{
							check_bound = 1;
							break;
						}
					}
				}
			}
			if(check_bound)
				bound_sum +=1;
		}
	}

	boundaryRecall = bound_sum / check_sum;

	Border_Recall[0] += boundaryRecall;
	
	//delete [] SegListK;

	//printf ("Total bleed %f\n", totalBleed);
	//return totalBleed;
	free(labelMap);
	free(BorderMask);
	delete [] SegListK;
	
}

void SegmentExtraction::find_areahist_2(UINT * labelMap,int* AreaHist,int mW,int mH,int segNo)
{
	node A;

	segNo = 0;
	for(int i=0;i<mW*mH;i++){
		if(labelMap[i] > segNo)
			segNo = labelMap[i];}

	DynamicList* SegListK = new DynamicList[segNo+1];
	int* AreaList = (int*)calloc(segNo+1,sizeof(int));
	int mSegmentNoK = segNo;
	for(int j=0;j<mH;j++)
	{
		for(int i=0;i<mW;i++)
		{
			A.xL = i;A.yL = j;
			int index = labelMap[i+mW*j];
			
			SegListK[index].push_back(A);
			AreaList[index] = AreaList[index] + 1;
		}
	}
	int t_index,indMax,Area;

	for(int i=0;i<segNo+1;i++)
		AreaHist[AreaList[i]]++; 


	free(AreaList);
	delete [] SegListK;
}
void SegmentExtraction::find_areahist_3(int * labelMap,int* AreaHist,int mW,int mH,int segNo)
{
	node A;

	segNo = 0;
	for(int i=0;i<mW*mH;i++){
		if(labelMap[i] > segNo)
			segNo = labelMap[i];}

	DynamicList* SegListK = new DynamicList[segNo+1];
	int* AreaList = (int*)calloc(segNo+1,sizeof(int));
	int mSegmentNoK = segNo;
	for(int j=0;j<mH;j++)
	{
		for(int i=0;i<mW;i++)
		{
			A.xL = i;A.yL = j;
			int index = labelMap[i+mW*j];
			
			SegListK[index].push_back(A);
			AreaList[index] = AreaList[index] + 1;
		}
	}
	int t_index,indMax,Area;

	for(int i=0;i<segNo+1;i++)
		AreaHist[AreaList[i]]++; 


	free(AreaList);
	delete [] SegListK;
}
void SegmentExtraction::find_areahist_External(IplImage * ImRef,IplImage* Im,int* AreaHist)
{
	node A;
	int mW = Im->width;
	int mH = Im->height;
	int* labelMap = (int*)calloc(mW*mH,sizeof(int));
	int segNo = 0;
	for(int j=0;j<mH;j++){
		for(int i=0;i<mW;i++){
			labelMap[i+j*mW] = 255*(int)p3(Im,j,i,1) + p3(Im,j,i,2);
			
			if(labelMap[i+j*mW] > segNo)
				segNo = labelMap[i+j*mW];
		}
	}
	for(int i=0;i<mW*mH;i++){
		if(labelMap[i] > segNo)
			segNo = labelMap[i];
	}
	//DynamicList* SegListK = new DynamicList[segNo+1];
	int* AreaList = (int*)calloc(segNo+1,sizeof(int));
	int* BoundaryList = (int*)calloc(segNo+1,sizeof(int));
	int mSegmentNoK = segNo;
	for(int j=1;j<mH-1;j++)
	{
		for(int i=1;i<mW-1;i++)
		{
			A.xL = i;A.yL = j;
			int index = labelMap[i+mW*j];
			
			//SegListK[index].push_back(A);
			AreaList[index] = AreaList[index] + 1;

			for(int k=-1;k<2;k++)
			{
				for(int m =-1;m<2;m++)
				{
					int checkin = labelMap[(j+k)*mW + i+m];
					
					if(checkin==index) continue;
					
					BoundaryList[index] += 1;
					m=3;k=3;
				}
			}
		}
	}

	int t_index,indMax,Area;

	for(int i=0;i<segNo+1;i++)
	{
 		int val = (10*BoundaryList[i]*BoundaryList[i])/(AreaList[i]+1);
		val = min(9999,val);
		AreaHist[val]++; 
	}

	free(labelMap);
	free(AreaList);
	free(BoundaryList);
	//delete [] SegListK;
}

void SegmentExtraction::Segment_wrt_Variance(uchar * ImIn,uchar * Im)
{
	float *Variance_list  = (float*)calloc((mSegmentNoK),sizeof(float));
	float *Mean_list  = (float*)calloc((mSegmentNoK),sizeof(float));
	int *check_background = (int*)calloc((mSegmentNoK),sizeof(int));
	int *minmax_list	  = (int*)calloc((2*mSegmentNoK),sizeof(int));

	for(int i=0;i<mSegmentNoK;i++)
	{
		minmax_list[i] = 255;
		minmax_list[i+mSegmentNoK] = 0;
	}
	for(int j=0;j<mH;j++)
	{
		for(int i=0;i<mW;i++)
		{
			//float valR = abs((float)ImIn[3*i] - R_mean[mSegmentIndexK[i]]);
			//float valG = abs((float)ImIn[3*i+1] - G_mean[mSegmentIndexK[i]]);
			//float valB = abs((float)ImIn[3*i+2] - B_mean[mSegmentIndexK[i]]);
			if(BorderMask[i + j*mW]==1)
			{
				float valX = abs(i - X_mean[mSegmentIndexK[i+j*mW]]);
				float valY = abs(j - Y_mean[mSegmentIndexK[i+j*mW]]);
			
				Mean_list[mSegmentIndexK[i+j*mW]] += valX*valX + valY*valY;//(valR + valG + valB);
			}
			/*if(valR < minmax_list[mSegmentIndexK[i+j*mW]])
				minmax_list[mSegmentIndexK[i+j*mW]] = valR;

			if(valR > minmax_list[mSegmentIndexK[i+j*mW]+mSegmentNoK])
				minmax_list[mSegmentIndexK[i+j*mW]+mSegmentNoK] = valR;*/
		}
	}

	float total_sum = 0;
	float max_var = 0;
	float min_var = 10000;
	float mean_diff,tval;
	mean_diff = 0;
	for(int i=0;i<mSegmentNoK-3;i++)
	{
		if(Count_mean[i])
		{
			Mean_list[i] = ((Mean_list[i])/Count_mean[i]);//sqrt(Variance_list[i]/(3*(float)Count_mean[i]));
		}
	}

	for(int j=0;j<mH;j++)
	{
		for(int i=0;i<mW;i++)
		{
			//float valR = abs((float)ImIn[3*i] - R_mean[mSegmentIndexK[i]]);
			//float valG = abs((float)ImIn[3*i+1] - G_mean[mSegmentIndexK[i]]);
			//float valB = abs((float)ImIn[3*i+2] - B_mean[mSegmentIndexK[i]]);
			if(BorderMask[i + j*mW]==1)
			{
				float valX = abs(i - X_mean[mSegmentIndexK[i+j*mW]]);
				float valY = abs(j - Y_mean[mSegmentIndexK[i+j*mW]]);
			
				float vall = (valX*valX + valY*valY) - Mean_list[mSegmentIndexK[i+j*mW]];
				Variance_list[mSegmentIndexK[i+j*mW]] += vall*vall;//(valR + valG + valB);
			}
			/*if(valR < minmax_list[mSegmentIndexK[i+j*mW]])
				minmax_list[mSegmentIndexK[i+j*mW]] = valR;

			if(valR > minmax_list[mSegmentIndexK[i+j*mW]+mSegmentNoK])
				minmax_list[mSegmentIndexK[i+j*mW]+mSegmentNoK] = valR;*/
		}
	}

	for(int i=0;i<mSegmentNoK-3;i++)
	{
		if(Count_mean[i])
		{
			Variance_list[i] = (sqrt(Variance_list[i])/Count_mean[i]);//sqrt(Variance_list[i]/(3*(float)Count_mean[i]));
			total_sum += Variance_list[i];

			if(Variance_list[i] > max_var)
				max_var = Variance_list[i];

			if(Variance_list[i] < min_var)
				min_var = Variance_list[i];

			/*tval = minmax_list[i+mSegmentNoK] - minmax_list[i];
			mean_diff += tval;

			if(tval > max_var)
				max_var = tval;

			if(tval < min_var)
				min_var = tval;*/
		}
	}

	total_sum = total_sum / (float)(mSegmentNoK);
	mean_diff = mean_diff / (float)(mSegmentNoK);

	for(int i=0;i<mW*mH;i++)
	{
		//if(Variance_list[mSegmentIndexK[i]] < total_sum)
		//if((minmax_list[mSegmentIndexK[i]+mSegmentNoK] - minmax_list[mSegmentIndexK[i]]) < mean_diff*0.5)
		if(Variance_list[mSegmentIndexK[i]] < total_sum)
		{
			Im[3*i] = 0;
			Im[3*i+1] = 255;
			Im[3*i+2] = 0;
		}
		else
		{
			Im[3*i] = 0;
			Im[3*i+1] = 0;
			Im[3*i+2] = 0;
		}

		//tval = minmax_list[mSegmentIndexK[i]+mSegmentNoK] - minmax_list[mSegmentIndexK[i]];

		tval = Variance_list[mSegmentIndexK[i]];
		tval = 255*(tval-min_var)/(max_var-min_var);
		Im[3*i] = tval;
		Im[3*i+1] = tval;
		Im[3*i+2] = tval;
	}

	free(Variance_list);
	free(check_background);
	free(Mean_list);
}
