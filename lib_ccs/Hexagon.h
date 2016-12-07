// SegmentExtraction.h: interface for the DepthEstimation class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_HEXAGON_H__709DA504_44CB_4EA3_92D9_BC798E7BE787__INCLUDED_)
#define AFX_HEXAGON_H__709DA504_44CB_4EA3_92D9_BC798E7BE787__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>
#include <vector>
#include "opencv2/opencv.hpp"
#include "BasicStructures.h"
#include <time.h>

typedef unsigned char uchar;

/////////////////////////////////////////


class Hexagon  
{
public:
	 
	 Hexagon();
	 virtual ~Hexagon();

	 void get_data(int Width,int Height,std::vector< uchar *> ImArray,int index,int *Segno);

	 void clear_data();
	 //// Kmeans Oversegmetnation
	 void KmeansOverSeg(uchar * Im,uchar * Im_out,int frame_no);
	 void find_borders_Kmeans(int *CandidateList);
	 void find_borders_Kmeans2(int *CandidateList,int ind);
	 void find_borders_Kmeans3(int *CandidateList,int ind);
	 template <class T> void update_segments(T * Im,float* one_over_size);
	 void smooth_image(uchar * Im);
	 //// Segmentation Evaluation
	 void find_bleeding(uchar * Im,float* BleedSum);
	 void find_edgeness(uchar *Im);
	 void smooth_saliency(IplImage * Im);
	 template <class T> void initialize(T *Im, int step,float* one_over_val);
	 void transfer_data(int* labelMap,int segNo);

	 void find_border_Recall(uchar * Im,float* Border_Recall/*, bool *boundaryMap*/);
	 void find_areahist(int * labelMap,int* AreaHist,int segNo);
	
	 std::vector< uchar *> mSegImArray;
	 uchar * TempIm;
	 int mW,mH,mSegmentNo,mSegmentNoK,mImageNo,mIterNo;
	 int mOutput_Choice,mCompactness;
	 float total_time;
	 int count,sz,mTotalSegmentNo,mIndex;
	 node A,B;
	 Segnode AA;

	 int *mSegmentIndex,*mSegmentIndexK;
	 int *mSegNoList,*EdgeMap;
	 int *BorderMask;
	 int *mBorderList;
	 int* mSegChange;
	 int *mLabelChangers;
	 int mBorderCount;
	 int *R_mean,*G_mean,*B_mean,*R_var,*G_var,*B_var,*Y_mean,*X_mean,*Count_mean;
	 int *RGB_total,*XY_total,*RGB_total2;
	 int* mSQlist;
	 DynamicList *SegList, *EdgeList, *BorderList,*NeighborList,*ThickBorderList;
	 DynamicList *SegListK;
	 DynamicList *FinalSegList, *FinalEdgeList, *FinalBorderList,*FinalNeighborList,*FinalThickBorderList;
	 DynamicSegList *SegData, *SegDataK,*FinalSegData;
};
#endif