// SegmentExtraction.cpp: implementation of the SegmentExtraction class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <vector>
#include "Hexagon.h"
#include <algorithm>
#include <cmath> 
#include "opencv2/opencv.hpp"

using namespace std;

#define p3(Y,r,c,chan) (((uchar*)(Y->imageData + Y->widthStep*(r)))[(c)*3+(chan)])


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


Hexagon::Hexagon()
{
	SegList = new DynamicList[1];
	SegData = new DynamicSegList[1];
	BorderList = new DynamicList[1];
	EdgeList   = new DynamicList[1];
	ThickBorderList = new DynamicList[1];
}
Hexagon::~Hexagon()
{

}
void Hexagon::get_data(int Width,int Height, std::vector< uchar *> ImArray,int index,int *Segno)
{
	mW = Width;
	mH = Height;
	//mImageNo = ImArray.size();
	mIndex = index;
	mSegmentNoK = Segno[0];
	mIterNo = Segno[1];
	mOutput_Choice = 1; // always show boundary
	mCompactness = Segno[2];
	mSegmentIndexK = (int*)calloc((mH*mW),sizeof(int));
	BorderMask = (int*)calloc((mH*mW),sizeof(int));
	mSQlist = (int*)calloc((256),sizeof(int));

	mBorderList = (int*)calloc((2*mH*mW),sizeof(int));
	mBorderCount = 0;
}
void Hexagon::clear_data()
{
	free(mSegmentIndexK);
	free(BorderMask);
	free(mBorderList);
	free(mSQlist);
	//free(mLabelChangers);
	delete [] SegListK;
}

template <class T>
void Hexagon::initialize(T *Im, int step,float* one_over_val)
{
	int Rval,Gval,Bval,Rval2,Gval2,Bval2,limit,index;
	int segcounter = 0;
	int xsum,ysum,area,Xcenter,Ycenter;
	float scale;
	count = 0;
	int shift,p1[2],p2[2],p3[2],p4[2],p5[2],p6[2];

	int ColNo = mW/(3*step)+1;
	int RowNo = mH/(step)+1;

	int CentShiftX = 3*step;
	int CentShiftY = step;
	int Ssegment;
	//memset(EdgeMap,-1,ColNo*RowNo*sizeof(int));
	for(int j=0;j<RowNo;j++)
	{
		if(j%2==0){
			shift = 5*step/2;
			Ssegment = -step*3;}
		else{
			shift = step;
			Ssegment = 0;
		}
		for(int i=0;i<ColNo;i++)
		{
			Xcenter = i*CentShiftX + shift+Ssegment;
			Ycenter = j*CentShiftY;

			p1[0] = Xcenter - step/2;p1[1] = Ycenter - step;
			p2[0] = Xcenter+step/2;p2[1] = Ycenter - step;
			p3[0] = Xcenter-step;p3[1] = Ycenter;
			p4[0] = Xcenter+step;p4[1] = Ycenter;
			p5[0] = Xcenter-step/2;p5[1] = Ycenter + step;
			p6[0] = Xcenter+step/2;p6[1] = Ycenter + step; 
			area = 0;
			
			Rval = 0;Gval = 0;Bval = 0;Rval2 = 0;Gval2 = 0;Bval2 = 0;
			xsum = 0;ysum = 0;
			
			for(int jj=p1[1];jj<p5[1];jj++)
			{
				if(jj<0 || jj>=mH) continue;
				int Str,End;
				int h = jj-Ycenter;
				if(h <= 0){
					h = abs(h);
					Str = p3[0] + h/2;
					End = p4[0] - h/2;
				}
				else
				{
					Str = p3[0] + h/2;
					End = p4[0] - h/2;
				}

				
				for(int ii=Str;ii<=End;ii++)
				{
					if(ii<0 || ii>=mW) continue;

					Rval = Rval + Im[3*(ii + jj*mW)];
					Gval = Gval + Im[3*(ii + jj*mW)+1];
					Bval = Bval + Im[3*(ii + jj*mW)+2];
					
					Rval2 += mSQlist[Im[3*(ii + jj*mW)]];
					Gval2 += mSQlist[Im[3*(ii + jj*mW)+1]];
					Bval2 += mSQlist[Im[3*(ii + jj*mW)+2]];
					
					xsum += ii;
					ysum +=jj;
					
					mSegmentIndexK[ii+mW*jj]=segcounter;//segment indexes
					area++;
				}
			}

			RGB_total[3*segcounter] = Rval;
			RGB_total[3*segcounter+1] = Gval;
			RGB_total[3*segcounter+2] = Bval;
			RGB_total2[3*segcounter] = Rval2;
			RGB_total2[3*segcounter+1] = Gval2;
			RGB_total2[3*segcounter+2] = Bval2;

			XY_total[2*segcounter] = xsum;
			XY_total[2*segcounter+1] = ysum;

			scale = one_over_val[area];
			R_mean[segcounter] = (float)Rval*scale;
			G_mean[segcounter] = (float)Gval*scale;
			B_mean[segcounter] = (float)Bval*scale;
			R_var[segcounter] = abs(Rval2*scale - R_mean[segcounter]*R_mean[segcounter]);
			G_var[segcounter] = abs(Gval2*scale - G_mean[segcounter]*G_mean[segcounter]);
			B_var[segcounter] = abs(Bval2*scale - B_mean[segcounter]*B_mean[segcounter]);

			Count_mean[segcounter] = area;
			X_mean[segcounter] =(float)xsum*scale;
			Y_mean[segcounter] = (float)ysum*scale;

			segcounter++;
		}
	}
	mSegmentNoK = segcounter;
}
void Hexagon::KmeansOverSeg(uchar * Im,uchar * Im_out,int frame_no)
{
	clock_t start,end,FullS,FullE;
	clock_t start1,end1,start2,end2;
	start = clock();

	double dif1,dif2,dif3;
	int Varmin[3];
	int SegNo = mSegmentNoK;
	int step = sqrt(double(2*mW*mH/(sqrt(3.0)*3*mSegmentNoK)));
	if(step%2==1)
		step++;

	int ColNo = mW/(3*step)+1;
	int RowNo = mH/(step)+1;
	mSegmentNoK = ColNo*RowNo+1;
	
	//EdgeMap = (int*)calloc((ColNo*RowNo),sizeof(int));
	FullS = clock();
	//find_edgeness(Im);
	SegNo = mSegmentNoK;

	int Rval,Gval,Bval,count,sz,limit,iindex,Mindex;
	float cost,MinCost,cost1;
	float costP1,costP;
	float compactness,t_size;
	//int *BorderMask = new int[mW*mH];
	int *CandidateList = (int*)calloc((mW*mH*9),sizeof(int));
	Segnode *BB,*CC;

	R_mean = (int*)calloc((mSegmentNoK),sizeof(int));
	G_mean = (int*)calloc((mSegmentNoK),sizeof(int));
	B_mean = (int*)calloc((mSegmentNoK),sizeof(int));
	R_var = (int*)calloc((mSegmentNoK),sizeof(int));
	G_var = (int*)calloc((mSegmentNoK),sizeof(int));
	B_var = (int*)calloc((mSegmentNoK),sizeof(int));
	Count_mean = (int*)calloc((mSegmentNoK),sizeof(int));
	X_mean = (int*)calloc((mSegmentNoK),sizeof(int));
	Y_mean = (int*)calloc((mSegmentNoK*2),sizeof(int));
	RGB_total  = (int*)calloc((mSegmentNoK*3),sizeof(int));
	RGB_total2 = (int*)calloc((mSegmentNoK*3),sizeof(int));
	XY_total = (int*)calloc((mSegmentNoK*2),sizeof(int));
	mSegChange = (int*)calloc((mSegmentNoK),sizeof(int));
	//float* im_Lab = (float*)calloc((mW*mH*3),sizeof(float));
	//rgb2Lab(Im, im_Lab,mH,mW);

	//SegDataK = new DynamicSegList[SegNo];
	//SegListK = new DynamicList[SegNo];
	memset(mSegmentIndexK, -1, mW*mH*sizeof(int));


	float *one_over_size= (float*)calloc((100000),sizeof(float));
	for(int i=1;i<100000;i++)
		one_over_size[i] = 1/(float(i));

	
	for(int i=0;i<256;i++)
		mSQlist[i] = i*i;

	if(frame_no==0)
		initialize(Im,step,one_over_size);

	start = clock();

	//printf ("It took  %f seconds for Initialization\n", dif1/CLOCKS_PER_SEC);

	start1 = clock();
	find_borders_Kmeans(CandidateList);
	end1 = clock();
	dif1 = end1 - start1;
	//printf ("It took  %f seconds for Border\n", dif1/CLOCKS_PER_SEC);
	//compactness = 500000/(mW*mH);
	//compactness = 0.15*mCompactness*16/((float)step*100);
	compactness = 64*mCompactness/((float)step*step*100)/9;

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
		
		memset(mSegChange,0,mSegmentNoK*sizeof(int));
		for(int c=0;c<mBorderCount;c++)
		{
			int i = mBorderList[2*c];
			int j = mBorderList[2*c+1];


			int index = j*mW + i;
			int segInd = mSegmentIndexK[index];

			if(segInd<0) continue;
			//Calculate Min Var among Neighbors
			Varmin[0] = R_var[segInd];Varmin[1] = G_var[segInd];Varmin[2] = B_var[segInd];
			for(int ii=0;ii<CandidateList[index];ii++)
			{
				iindex = CandidateList[9*index+(ii+1)];
				if(R_var[iindex] < Varmin[0])
					Varmin[0] = R_var[iindex];;

				if(G_var[iindex] < Varmin[1])
					Varmin[1] = G_var[iindex];;

				if(B_var[iindex] < Varmin[2])
					Varmin[2] = B_var[iindex];
			}
			Varmin[0] = min(9000,max(16,Varmin[0]));
			Varmin[1] = min(9000,max(16,Varmin[1]));
			Varmin[2] = min(9000,max(16,Varmin[2]));

			int Xmean = X_mean[segInd];
			int Ymean = Y_mean[segInd];
			//Rval = im_Lab[3*(i+mW*j)];Gval = im_Lab[3*(i+mW*j)+1];Bval = im_Lab[3*(i+mW*j)+2];
			Rval = Im[3*index];Gval = Im[3*index+1];Bval = Im[3*index+2];	

			
			cost1 = (float)mSQlist[abs(R_mean[segInd] - Rval)]*one_over_size[Varmin[0]] + (float)mSQlist[abs(G_mean[segInd] - Gval)]*one_over_size[Varmin[1]] 
					+ (float)mSQlist[abs(B_mean[segInd] - Bval)]*one_over_size[Varmin[2]];
		/*	cost1 = (float)mSQlist[abs(R_mean[segInd] - Rval)]/(float)Varmin[0] + (float)mSQlist[abs(G_mean[segInd] - Gval)]/(float)Varmin[1]
					+ (float)mSQlist[abs(B_mean[segInd] - Bval)]/(float)Varmin[2];*/
			/*cost1 = abs(R_mean[segInd] - Rval) + abs(G_mean[segInd] - Gval) 
					+ abs(B_mean[segInd] - Bval);*/

			costP1 = (i - Xmean)*(i - Xmean) + (j - Ymean)*(j - Ymean);

			MinCost = cost1 + costP1*compactness;
			Mindex = segInd;

			
			for(int ii=0;ii<CandidateList[index];ii++)
			{
				//index = CandidateList[i + mW*j+(ii+1)*mW*mH];
				iindex = CandidateList[9*index+(ii+1)];
				Xmean = X_mean[iindex];
				Ymean = Y_mean[iindex];

				cost = (float)mSQlist[abs(R_mean[iindex] - Rval)]*one_over_size[Varmin[0]] + (float)mSQlist[abs(G_mean[iindex] - Gval)]*one_over_size[Varmin[1]] 
						+ (float)mSQlist[abs(B_mean[iindex] - Bval)]*one_over_size[Varmin[2]];
				/*cost1 = (float)mSQlist[abs(R_mean[iindex] - Rval)]/(float)Varmin[0] + (float)mSQlist[abs(G_mean[iindex] - Gval)]/(float)Varmin[1]
					+(float)mSQlist[abs(B_mean[iindex] - Bval)]/(float)Varmin[2];*/
				/*cost = abs(R_mean[iindex] - Rval) + abs(G_mean[iindex] - Gval)
						+ abs(B_mean[iindex] - Bval);*/

				costP = (i - Xmean)*(i - Xmean) + (j - Ymean)*(j - Ymean);
				
				cost = cost + costP*compactness;

				if(cost<MinCost)
				{
					MinCost = cost;
					Mindex = iindex;
				}
			}
			if(Mindex != segInd)
			{
				mSegChange[segInd]++;
				mSegChange[Mindex]++;
				if(Count_mean[segInd])
				{
					Count_mean[segInd] = Count_mean[segInd] - 1;
					RGB_total[3*segInd] -= Rval;
					RGB_total[3*segInd+1] -= Gval;
					RGB_total[3*segInd+2] -= Bval;
					RGB_total2[3*segInd] -= mSQlist[Rval];
					RGB_total2[3*segInd+1] -= mSQlist[Gval];
					RGB_total2[3*segInd+2] -= mSQlist[Bval];
					XY_total[2*segInd] -= i;
					XY_total[2*segInd+1] -= j;
				}

				Count_mean[Mindex] = Count_mean[Mindex] + 1;
				RGB_total[3*Mindex] += Rval;
				RGB_total[3*Mindex+1] += Gval;
				RGB_total[3*Mindex+2] += Bval;
				RGB_total2[3*Mindex] += mSQlist[Rval];
				RGB_total2[3*Mindex+1] += mSQlist[Gval];
				RGB_total2[3*Mindex+2] += mSQlist[Bval];

				XY_total[2*Mindex] += i;
				XY_total[2*Mindex+1] += j;
				mSegmentIndexK[i+mW*j]=Mindex;
			}

		}
		
		start1 = clock();
		update_segments(Im,one_over_size);
		end1 = clock();
		dif2 += end1 - start1;
		
		//if(iter<mIterNo-1)
		{
			start1 = clock();
				find_borders_Kmeans2(CandidateList,iter);
			end1 = clock();
			dif3 += end1 - start1;
		}

		//printf ("Number of Border Pixels = %d\n", mBorderCount);
	}

	end = clock();
	dif1 = end - start;
	//printf ("It took  %f seconds for Iteration\n", dif1/CLOCKS_PER_SEC);

	FullE = clock();
	dif1 = FullE - FullS;
	printf ("It took  %f seconds for All Computations \n", dif1/CLOCKS_PER_SEC);
	printf ("It took  %f seconds for ModelUpdate \n", dif2/(CLOCKS_PER_SEC));
	printf ("It took  %f seconds for BoundaryDetection \n", dif3/(CLOCKS_PER_SEC));

	total_time += dif1/CLOCKS_PER_SEC;
	/////////   Fill the output Image
	int dx8[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
	int dy8[8] = { 0, -1, -1, -1, 0, 1, 1,  1};

	sz = mW*mH;

	for(int i=0;i<mW*mH;i++)
		mSegmentIndexK[i] = max(0,mSegmentIndexK[i]);

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
	//for( int j = 0; j < mH; j++ )
	//{
	//	for( int k = 0; k < mW; k++ )
	//	{
	//		Im_out[3*mainindex] = Im[3*mainindex];
	//		Im_out[3*mainindex+1] = Im[3*mainindex+1];
	//		Im_out[3*mainindex+2] = Im[3*mainindex+2];
	//	}
	//	mainindex++;
	//}
	/*for(int c=0;c<mBorderCount;c++)
	{
		int i = mBorderList[2*c];
		int j = mBorderList[2*c+1];

		mainindex = i+j*mW;

		Im_out[3*mainindex] = 255;
		Im_out[3*mainindex+1] = 0;
		Im_out[3*mainindex+2] = 255;

	}*/
	//for(int i=0;i<mSegmentNoK;i++)
	//{
	//	mainindex = X_mean[i]+mW*Y_mean[i];
	//	Im_out[3*mainindex] = 0;
	//	Im_out[3*mainindex+1] = 0;
	//	Im_out[3*mainindex+2] = 255;
	//}
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
	//Segment_wrt_Variance(Im,Im_out);

	//SegListK = new DynamicList[mSegmentNoK];
	//for(int j=0;j<mH;j++)
	//{
	//	for(int i=0;i<mW;i++)
	//	{
	//		
	//		A.xL = i;A.yL = j;
	//		int index = mSegmentIndexK[i+mW*j];
	//		
	//		
	//		Im_out[3*(i + mW*j)] = R_mean[index];
	//		Im_out[3*(i + mW*j)+1] = G_mean[index];
	//		Im_out[3*(i + mW*j)+2] = B_mean[index];
	//		
	//		//SegListK[index].push_back(A);

	//	}
	//}

	free(X_mean);
	free(Y_mean);
	free(R_mean);
	free(G_mean);
	free(B_mean);
	free(R_var);
	free(G_var);
	free(B_var);
	free(XY_total);
	free(RGB_total);
	free(RGB_total2);
	free(one_over_size);
	free(CandidateList);
	free(Count_mean);
	//free(im_Lab);
}

void Hexagon::find_borders_Kmeans(int *CandidateList)
{
	int checkin,segInd,count,index;

	memset(BorderMask,0, mW*mH*sizeof(int));
	//int* checkInd = (int*)calloc((mSegmentNoK),sizeof(int));
	mBorderCount = 0;

	for(int j=1;j<mH-1;j++)
	{
		for(int i=1;i<mW-1;i++)
		{
			//memset(checkInd,0, mSegmentNoK*sizeof(int));
			segInd = mSegmentIndexK[j*mW + i];
			//checkInd[segInd] = 1;

			count =0;
			for(int k=-1;k<2;k++)
			{
				for(int m =-1;m<2;m++)
				{
					checkin = mSegmentIndexK[(j+k)*mW + i+m];
					
					//if(checkInd[checkin]) continue;
					if(checkin == segInd) continue;
					if(checkin < 0) continue;
					
					//checkInd[checkin] = 1;
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

	//free(checkInd);
}
void Hexagon::find_borders_Kmeans2(int *CandidateList,int ind)
{
	int checkin,segInd,count,index;

	memset(BorderMask,0, mW*mH*sizeof(int));
	//int* checkInd = (int*)calloc((mSegmentNoK),sizeof(int));
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
			index = j*mW + i;
			segInd = mSegmentIndexK[index];

			if(mSegChange[segInd]<8) continue;

			//memset(checkInd,0, mSegmentNoK*sizeof(int));
			count =0;
			//checkInd[segInd] = 1;

			for(int k=-1;k<2;k++)
			{
				for(int m =-1;m<2;m++)
				{
					checkin = mSegmentIndexK[(j+k)*mW + i+m];
					
					//if(checkInd[checkin]) continue;
					if(checkin ==segInd) continue;
					if(checkin < 0) continue;

					//checkInd[checkin] = 1;
					
					BorderMask[index] = 1;
					count +=1;
					
					//CandidateList[i + j*mW+count*mW*mH] = checkin;
					CandidateList[index*9+count] = checkin;
				}
			}
			
			CandidateList[index] = count;

			if(BorderMask[index]){
				mBorderList[2*mBorderCount] = i;
				mBorderList[2*mBorderCount+1] = j;
				mBorderCount++;}
		}
	}

	//free(checkInd);
}
void Hexagon::find_borders_Kmeans3(int *CandidateList,int ind)
{
	int checkin,segInd,count,index;

	memset(BorderMask,0, mW*mH*sizeof(int));
	int* checkInd = (int*)calloc((mW*mH),sizeof(int));
	int* Blist = (int*)calloc((2*mW*mH),sizeof(int));

	int BorderC = 0;

	for(int c=0;c<mBorderCount;c++)
	{
		int i = mBorderList[2*c];
		int j = mBorderList[2*c+1];

		index = j*mW + i;
		
		//checkInd[segInd] = 1;

			for(int k=-1;k<2;k++)
			{
				for(int m =-1;m<2;m++)
				{
					if((j+k)<0 || (j+k)>=mH) continue;
					if((i+m)<0 ||(i+m)>=mW) continue;

					int ind2 = (j+k)*mW + i+m;

					if(checkInd[ind2]) continue;
					checkInd[ind2] = 1;

					segInd = mSegmentIndexK[ind2];

					if(mSegChange[segInd]<4) continue;

					count =0;
					for(int kk=-1;kk<2;kk++)
					{
						for(int mm=-1;mm<2;mm++)
						{
							if((j+k+kk)<0 || (j+k+kk)>=mH) continue;
							if((i+m+mm)<0 ||(i+m+mm)>=mW) continue;

							int ind3 = (j+k+kk)*mW + i+m+mm;
							checkin = mSegmentIndexK[ind3];
							if(checkin ==segInd) continue;
							if(checkin < 0) continue;
							 
							BorderMask[ind2] = 1;
							count +=1;

							CandidateList[ind2*9+count] = checkin;
						}
					}

			CandidateList[ind2] = count;

			if(BorderMask[ind2]){
				Blist[2*BorderC] = i+m;
				Blist[2*BorderC+1] = j+k;
				BorderC++;}
				}
			}
	}

	mBorderCount = BorderC;
	for(int i=0;i<2*mBorderCount;i++)
		mBorderList[i] = Blist[i];

	free(Blist);
	free(checkInd);
}
void Hexagon::transfer_data(int* labelMap,int segNo)
{
	SegListK = new DynamicList[segNo];
	mSegmentNoK = segNo;
	for(int j=0;j<mH;j++)
	{
		for(int i=0;i<mW;i++)
		{
			A.xL = i;A.yL = j;
			int index = labelMap[i+mW*j];
			if(index<0) continue;
			SegListK[index].push_back(A);

		}
	}
}
template <class T>
void Hexagon::update_segments(T * Im,float* one_over_size)
{
	int Rval,Gval,Bval;
	int Xval,Yval,index;
	float t_size;

	for(int i=0;i<mSegmentNoK;i++)
	{
		if(Count_mean[i]>0)
			t_size = one_over_size[(int)Count_mean[i]];
		else
			t_size = 0;

		R_mean[i] = (float)RGB_total[3*i]*t_size;
		G_mean[i] = (float)RGB_total[3*i+1]*t_size;
		B_mean[i] = (float)RGB_total[3*i+2]*t_size;

		R_var[i] = abs(RGB_total2[3*i]*t_size - R_mean[i]*R_mean[i]);
		G_var[i] = abs(RGB_total2[3*i+1]*t_size - G_mean[i]*G_mean[i]);
		B_var[i] = abs(RGB_total2[3*i+2]*t_size - B_mean[i]*B_mean[i]);

		X_mean[i] = (float)XY_total[2*i]*t_size;
		Y_mean[i] = (float)XY_total[2*i+1]*t_size;
	}
}

void Hexagon::smooth_image(uchar * Im)
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

void Hexagon::find_bleeding(uchar * Im,float* BleedSum)
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

void Hexagon::find_border_Recall(uchar * Im,float* Border_Recall/*, bool *boundaryMap*/) {
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
void Hexagon::find_areahist(int * labelMap,int* AreaHist,int segNo)
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