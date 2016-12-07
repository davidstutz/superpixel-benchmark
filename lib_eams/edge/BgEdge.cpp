/////////////////////////////////////////////////////////////////////////////
// Name:        BgEdge.cpp
// Purpose:     BGEdge class functions
// Author:      Bogdan Georgescu
// Modified by:
// Created:     06/22/2000
// Copyright:   (c) Bogdan Georgescu
// Version:     v0.1
/////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include "BgEdge.h"
#define PI 3.1415926535

BgEdge::BgEdge()
{
   nPoints_ = 0;
   edge_ = 0;
   next_ = 0;
   isGradSet_ = false;
   grad_ = 0;
   isMarkSet_ = false;
   mark_ = 0;
}

BgEdge::~BgEdge()
{
   if (nPoints_>0) delete [] edge_;
   if (isGradSet_) delete [] grad_;
   if (isMarkSet_) delete [] mark_;
}

void BgEdge::SetPoints(float* points, int npoints)
{
   if (nPoints_>0) delete [] edge_;
   nPoints_ = npoints;
   edge_=new int[npoints*2];

   float *its;
   int *itd;
   itd=edge_;
   its=points;
   for (int i=0; i<2*npoints; i++)
      *(itd++) = (int) *(its++);
}

void BgEdge::SetPoints(int* points, int npoints)
{
   if (nPoints_>0) delete [] edge_;
   nPoints_ = npoints;
   edge_=new int[npoints*2];

   int *its;
   int *itd;
   itd=edge_;
   its=points;
   for (int i=0; i<2*npoints; i++)
      *(itd++) = *(its++);
}

void BgEdge::SetGradient(float* grx, float* gry, float* mark, int ncol)
{
   if (isGradSet_ && nPoints_>0) delete [] grad_;

   grad_ = new double[nPoints_];
   isGradSet_ = true;

   double alpha, gx, gy;
   int* it;
   it=edge_;
   int x, y, i, j, nv;

   for (i=0; i<nPoints_;i++)
   {
      x = *(it++);
      y = *(it++);
      gx = *(grx+y*ncol+x);
      gy = *(gry+y*ncol+x);
      nv=1;
      for (j=0;j<8;j++)
      {
         if (*(mark+(y+gNb8[j][1])*ncol+gNb8[j][0]+x) == 1)
         {
            gx += *(grx+(y+gNb8[j][1])*ncol+gNb8[j][0]+x);
            gy += *(gry+(y+gNb8[j][1])*ncol+gNb8[j][0]+x);
         }
      }
      alpha = atan2(gy,gx);
      alpha = (alpha<0) ? alpha+PI : alpha;
      *(grad_+i)=alpha;
   }
}
