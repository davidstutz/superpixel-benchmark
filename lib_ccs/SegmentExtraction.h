// SegmentExtraction.h: interface for the DepthEstimation class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_SEGMENTEXTRACTION_H__709DA504_44CB_4EA3_92D9_BC798E7BE787__INCLUDED_)
#define AFX_SEGMENTEXTRACTION_H__709DA504_44CB_4EA3_92D9_BC798E7BE787__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>
#include <vector>
#include "opencv2/opencv.hpp"
#include "BasicStructures.h"
#include <time.h>

typedef unsigned char uchar;
typedef unsigned int UINT;
/////////////////////////////////////////


class SegmentExtraction  
{
public:
	 
	 SegmentExtraction();
	 virtual ~SegmentExtraction();

	 void get_data(int Width,int Height,std::vector< uchar *> ImArray,int index,int *Segno);

	 void clear_data();
	 void update_segments_with_depth(uchar * DepthMap);
	 //// Kmeans Oversegmetnation
	 void KmeansOverSeg(uchar * Im,uchar * Im_out,int frame_no);
	 void find_borders_Kmeans(int *CandidateList);
	 void find_borders_Kmeans2(int *CandidateList,int ind);
	 template <class T> void update_segments(T * Im,float* one_over_size);
	 void smooth_image(uchar * Im);
	 //// Segmentation Evaluation
	 
	 void find_bleeding(uchar * Im,float* BleedSum);
	 //template <class T>  
	 void find_bleeding_2(UINT* labels, uchar * Im,float* BleedSum,int mW,int mH,int segNo);
	 void find_bleeding_External(IplImage * ImRef,IplImage* Im,float* BleedSum);

	 void find_edgeness(uchar *Im);
	 void smooth_saliency(IplImage * Im);
	 template <class T> void initialize(T *Im,int ColNo, int RowNo, int step,float* one_over_val);
	 void transfer_data(int* labelMap,int segNo);

	 void find_border_Recall(uchar * Im,float* Border_Recall/*, bool *boundaryMap*/);
	 //template <class T> 
	 void find_border_Recall_2(UINT* labelMap,uchar * Im,float* Border_Recall,int mW,int mH,int segNo);
	 void find_border_Recall_External(uchar * Im,IplImage* Image,float* Border_Recall);
	 
	 void find_areahist_2(UINT* labelMap,int* AreaHist,int mW,int mH,int segNo);
	 void find_areahist_3(int* labelMap,int* AreaHist,int mW,int mH,int segNo);
	 void find_areahist_External(IplImage * ImRef,IplImage* Im,int* AreaHist);

	 void Segment_wrt_Variance(uchar * ImIn,uchar * Im);

	 std::vector< uchar *> mSegImArray;
	 uchar * TempIm;
	 int mW,mH,mSegmentNo,step,mSegmentNoK,mImageNo,mIterNo;
	 int mOutput_Choice,mCompactness;
	 float total_time;
	 int count,sz,mTotalSegmentNo,mIndex;
	 node A,B;
	 Segnode AA;

	 int *mSegmentIndex,*mSegmentIndexK;
	 int *mSegNoList,*EdgeMap;
	 int *BorderMask;
	 int *mBorderList;
	 bool* mSegChange;
	 int *mLabelChangers;
	 int mBorderCount;
	 int *R_mean,*G_mean,*B_mean,*Y_mean,*X_mean,*Count_mean;
	 int *RGB_total,*XY_total;
	 DynamicList *SegList, *EdgeList, *BorderList,*NeighborList,*ThickBorderList;
	 DynamicList *SegListK;
	 DynamicList *FinalSegList, *FinalEdgeList, *FinalBorderList,*FinalNeighborList,*FinalThickBorderList;
	 DynamicSegList *SegData, *SegDataK,*FinalSegData;
};
#endif