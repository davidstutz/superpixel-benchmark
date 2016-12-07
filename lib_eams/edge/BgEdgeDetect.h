/////////////////////////////////////////////////////////////////////////////
// Name:        BgEdgeDetect.h
// Purpose:     BgEdgeDetect class
// Author:      Bogdan Georgescu
// Modified by:
// Created:     06/22/2000
// Copyright:   (c) Bogdan Georgescu
// Version:     v0.1
/////////////////////////////////////////////////////////////////////////////

#define PI 3.1415926535
#define ZERO_TRESH 0.0000000001

// default values for edge detection
#define CONF_NMX 0.5
#define RANK_NMX 0.5
#define CONF_H 0.96
#define RANK_H 0.93
#define CONF_L 0.91
#define RANK_L 0.99
#define NMIN 5
#define KERNEL_SIZE 2

#define HYST_LOW_CUT 0.0
#define MAX_CUSTT 30
#define MAX_FILTS 31
#define NO_ANGLES 361

#define ALF_TRESH PI/4

static const int gNb[8][2]=
{
   1, 0,
   1, 1,
   1,-1,
   0, 1,
   0,-1,
  -1, 0,
  -1, 1,
  -1,-1
};

static const double gAlpha[8][2]=
{
     PI/2,   PI/2,
     PI/4,   PI/4,
   3*PI/4, 3*PI/4,
        0,   PI,
        0,   PI,
     PI/2,   PI/2,
   3*PI/4, 3*PI/4,
     PI/4,   PI/4
};

// main class, edge detection
class BgEdgeDetect
{
public:

   // main function for edge detection
   // cim input image
   // cel edge list (will be filled with pixels on edges)
   // nmxr, nmxc threshold for non-maxima-suppresion rank, confidence
   // rh, ch, threshold for hyst. high; rank, confidence
   // rl, cl, threshold for hyst. low; rank, confidence
   // nMin, min number of pixels on an edge
   // nmxType, hystTypeHigh, hystTypeLow, type of nmx curve, hyst. high curve, hyst low curve
   //  in (FC_ELLIPSE, FC_VERT_LINE, FC_HORIZ_LINE, FC_LINE, FC_SQUARE_BOX, FC_CUSTOM)

   void DoEdgeDetect(BgImage* cim, BgEdgeList* cel, double nmxr, double nmxc,
                     double rh, double ch, double rl, double cl,
                     int nMin, int nmxType, int hystTypeHigh, int hystTypeLow);

   // computes confidence map and rank information of sepcified image
   void ComputeEdgeInfo(BgImage*, float*, float*);
//   void ComputeConfidenceMap1(BgImage*, float*);
   // if have permanent data, call this function (compute only last two steps is same kernel size)
   void DoRecompute(BgEdgeList*, double, double, double, double, double, double, int, int, int, int);

   BgEdgeDetect(int filtDim);
   ~BgEdgeDetect();

   void SaveNmxValues();

   float EllipseEval(float, float);
   float EllipseComp(float, float, float, float);
   float LineEval(float, float);
   float LineComp(float, float, float, float);
   float VerticalLineEval(float, float);
   float VerticalLineComp(float, float, float, float);
   float HorizontalLineEval(float, float);
   float HorizontalLineComp(float, float, float, float);
   float SquareEval(float, float);
   float SquareComp(float, float, float, float);
   float CustomRegionEval(float, float);
   float CustomRegionComp(float, float, float, float);

   void SetCustomHigh(int*, int*, int, int, int);
   void SetCustomLow(int*, int*, int, int, int);
   void SetCustomHigh(double*, double*, int);
   void SetCustomLow(double*, double*, int);

   void IsGood(void);
   void GetPixels(int*, int*, int*, double, double, double, double);
   void GetNmxPixels(int*, int*, int*, double, double, double, double);
   
   double smofil_[MAX_FILTS];
   double diffil_[MAX_FILTS];
   double wdx_[MAX_FILTS*MAX_FILTS];
   double wdy_[MAX_FILTS*MAX_FILTS];
   double mN_[MAX_FILTS][MAX_FILTS];
   double mQ_[MAX_FILTS][MAX_FILTS];
   double* lookTable_[NO_ANGLES];

   int WW_;
   int WL_;
   float confTr_;
   float rankTr_;

   float* custx_;
   float* custy_;
   float* tcustx_;
   float* tcusty_;
   int ncust_;

   float* hcustx_;
   float* hcusty_;
   int nhcust_;
   float* lcustx_;
   float* lcusty_;
   int nlcust_;   

   int x_;
   int y_;
   float* permConf_;
   float* permRank_;
   float* permNmxRank_;
   float* permNmxConf_;
   bool havePerm_;

protected:

   void GenerateMaskAngle(double*, double);
   void CreateFilters(void);	
   void CreateLookTable(void);
   void DeleteLookTable(void);
   void GaussFilter(BgImage*, float*, double, int);
   void GaussDiffFilter(BgImage*, float*, float*, float*);
   void Strength(float*, float*, float*);
   void NewNonMaxSupress(float*, float*, float*, float*, float*, float* ,
   float (BgEdgeDetect::*compf)(float, float, float, float));
   void StrConfEstim(float*, float*, float*, float (BgEdgeDetect::*evalf)(float, float));
   void CompRanks(float*, float*);
   void NewHysteresisTr(float*, float*, BgEdgeList*, int, float*, float*);
   void NewEdgeFollow(int, int);
   void SubspaceEstim(float*, float*, float*, float*);

   float* te_;
   float* tm_;
   double low_;
   float* tc_;
   float* tl_;
   int npt_;

   float* grx_;
   float* gry_;
   float* permGx_;
   float* permGy_; 
};
