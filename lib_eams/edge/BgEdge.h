/////////////////////////////////////////////////////////////////////////////
// Name:        BgEdge.h
// Purpose:     BgEdge class
// Author:      Bogdan Georgescu
// Modified by:
// Created:     06/22/2000
// Copyright:   (c) Bogdan Georgescu
// Version:     v0.1
/////////////////////////////////////////////////////////////////////////////

// 8-connected neighbour
static const int gNb8[8][2]=
{
   1, 1,
   1,-1,
   1, 0,
   0, 1,
   0,-1,
  -1,-1,
  -1, 0,
  -1, 1
};

class BgEdge
{
public:
   int* edge_;
   double* grad_;
   bool isGradSet_;
   bool isMarkSet_;
   unsigned char* mark_;
   int nPoints_;
   BgEdge* next_;

   BgEdge();
   ~BgEdge();
   void SetPoints(float*, int);
   void SetPoints(int*, int);
   void SetGradient(float*,float*,float*,int);
};