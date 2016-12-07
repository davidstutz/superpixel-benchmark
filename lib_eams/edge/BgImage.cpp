/////////////////////////////////////////////////////////////////////////////
// Name:        BgImage.cpp
// Purpose:     BgImage class functions
// Author:      Bogdan Georgescu
// Modified by:
// Created:     06/22/2000
// Copyright:   (c) Bogdan Georgescu
// Version:     v0.1
/////////////////////////////////////////////////////////////////////////////

#include "BgImage.h"

BgImage::BgImage()
{
   hasIm_ = false;
   colorIm_ = false;
   im_ = 0;
}

BgImage::BgImage(int x,int y, bool colorIm)
{
   colorIm_ = colorIm;
   if (colorIm_ == false)
      im_ = new unsigned char[x*y];
   else
      im_ = new unsigned char[x*y*3];
   x_ = x;
   y_ = y;
   hasIm_ = true;
}

BgImage::~BgImage()
{
   CleanData();
}

void BgImage::CleanData()
{
   if (hasIm_)
   {
      delete [] im_;
      x_ = y_ = 0;
      hasIm_ = false;
      colorIm_ = false;
   }
}

void BgImage::SetImage(unsigned char* im, int x, int y, bool colorIm)
{
   CleanData();
   colorIm_ = colorIm;
   if (colorIm_ == false)
      im_ = new unsigned char[x*y];
   else
      im_ = new unsigned char[x*y*3];
   x_ = x;
   y_ = y;
   hasIm_ = true;

   int i;
   unsigned char *its, *itd;
   if (colorIm_ == false)
   {
      for (i=0, its=im, itd=im_; i<x*y; i++)
      {
         *(itd++) = *(its++);
      }
   }
   else
   {
      for (i=0, its=im, itd=im_; i<x*y*3; i++)
      {
         *(itd++) = *(its++);
      }
   }

}

void BgImage::SetImageFromRGB(unsigned char* im, int x, int y, bool colorIm)
{
   PrivateResize(x, y, colorIm);

   int i;
   unsigned char *its, *itd;
   if (colorIm_ == false)
   {
      for (i=0, its=im, itd=im_; i<x*y; i++, itd++, its+=3)
	  {
		 *itd = (int) (its[0]*RED_WEIGHT + its[1]*GREEN_WEIGHT + its[2]*BLUE_WEIGHT);
	  }
   }
   else
   {
      for (i=0, its=im, itd=im_; i<x*y*3; i++)
         *(itd++) = *(its++);
   }
}

void BgImage::SetSameImageFromRGB(unsigned char* im)
{
   int i;
   unsigned char *its, *itd;
   if (colorIm_ == false)
   {
      for (i=0, its=im, itd=im_; i<x_*y_; i++, itd++, its+=3)
		 *itd = (int) (its[0]*RED_WEIGHT + its[1]*GREEN_WEIGHT + its[2]*BLUE_WEIGHT);
   }
   else
   {
      for (i=0, its=im, itd=im_; i<x_*y_*3; i++)
         *(itd++) = *(its++);
   }
}

void BgImage::SetImage(short* im, int x, int y, bool colorIm)
{
   CleanData();
   colorIm_ = colorIm;
   if (colorIm_ == false)
      im_ = new unsigned char[x*y];
   else
      im_ = new unsigned char[x*y*3];
   x_ = x;
   y_ = y;
   hasIm_ = true;

   int i;
   unsigned char *itd;
   short* its;
   if (colorIm_ == false)
   {
      for (i=0, its=im, itd=im_; i<x*y; i++)
      {
         *(itd++) = (unsigned char) *(its++);
      }
   }
   else
   {
      for (i=0, its=im, itd=im_; i<x*y*3; i++)
      {
         *(itd++) = (unsigned char) *(its++);
      }
   }
}

void BgImage::GetImage(unsigned char* im)
{
   unsigned char *its, *itd;
   int i;
   if (colorIm_ == false)
   {
      for(i=0, its=im_, itd=im; i<x_*y_; i++)
         *(itd++) = *(its++);
   }
   else
   {
      for(i=0, its=im_, itd=im; i<x_*y_*3; i++)
         *(itd++) = *(its++);
   }
}

void BgImage::GetImageColor(unsigned char* im)
{
   unsigned char *its, *itd;
   int i;
   if (colorIm_ == false)
   {
      for(i=0, its=im_, itd=im; i<x_*y_; i++)
      {
         *(itd++) = *its;
         *(itd++) = *its;
         *(itd++) = *(its++);
      }
   }
   else
   {
      for(i=0, its=im_, itd=im; i<(x_*y_*3); i++)
         *(itd++) = *(its++);
   }
}

void BgImage::GetImageBW(unsigned char* im)
{
   unsigned char *its, *itd;
   int i;
   if (colorIm_ == false)
   {
      for(i=0, its=im_, itd=im; i<x_*y_; i++)
         *(itd++) = *(its++);
   }
   else
   {
      for (i=0, its=im_, itd=im; i<x_*y_; i++, itd++, its+=3)
		 *itd = (int) (its[0]*RED_WEIGHT + its[1]*GREEN_WEIGHT + its[2]*BLUE_WEIGHT);
   }
}

void BgImage::GetImageR(unsigned char* im)
{
   unsigned char *its, *itd;
   int i;
   if (colorIm_ == false)
   {
      for(i=0, its=im_, itd=im; i<x_*y_; i++)
         *(itd++) = *(its++);
   }
   else
   {
      for(i=0, its=im_, itd=im; i<x_*y_; i++)
      {
         *(itd++) = *its;
         its += 3;
      }
   }
}

void BgImage::GetImageG(unsigned char* im)
{
   unsigned char *its, *itd;
   int i;
   if (colorIm_ == false)
   {
      for(i=0, its=im_, itd=im; i<x_*y_; i++)
         *(itd++) = *(its++);
   }
   else
   {
      for(i=0, its=im_+1, itd=im; i<x_*y_; i++)
      {
         *(itd++) = *its;
         its += 3;
      }
   }
}

void BgImage::GetImageB(unsigned char* im)
{
   unsigned char *its, *itd;
   int i;
   if (colorIm_ == false)
   {
      for(i=0, its=im_, itd=im; i<x_*y_; i++)
         *(itd++) = *(its++);
   }
   else
   {
      for(i=0, its=im_+2, itd=im; i<x_*y_; i++)
      {
         *(itd++) = *its;
         its += 3;
      }
   }
}

inline unsigned char BgImage::operator()(int r, int c) const
{
   assert(hasIm_ && (r >= 0) && (r < y_) && (c >= 0) && (c < x_));
   return im_[c+r*x_];
}

inline unsigned char& BgImage::operator()(int r, int c)
{
   assert(hasIm_ && (r >= 0) && (r < y_) && (c >= 0) && (c < x_));
   return im_[c+r*x_];
}

unsigned char BgImage::PixelValue(int r, int c)
{
   assert(hasIm_ && (r >= 0) && (r < y_) && (c >= 0) && (c < x_));
   return im_[c+r*x_];
}

inline unsigned char gBgImPt(BgImage* in_im, int in_r, int in_c)
{
   assert(in_im->hasIm_ && (in_r >= 0) && (in_r < in_im->y_) && (in_c >= 0) && (in_c < in_im->x_));
   return in_im->im_[in_c+in_r*in_im->x_];
}

bool BgImage::ValidCoord(int in_x, int in_y)
{
   return ((in_x>=0) && (in_x<x_) && (in_y>=0) && (in_y<y_));
}

int BgImage::ValidReturnBW(int in_x, int in_y, int& cval)
{
   if ((in_x>=0) && (in_x<x_) && (in_y>=0) && (in_y<y_)) 
   {
      cval = im_[in_x+in_y*x_];
      return 1;
   } else
   {
      cval = -1;
      return 0;
   }
}
int BgImage::ValidReturnCol(int in_x, int in_y, int& rval, int& gval, int& bval)
{
   if ((in_x>=0) && (in_x<x_) && (in_y>=0) && (in_y<y_)) 
   {
      rval = im_[in_x*3+0+in_y*(x_*3)];
      gval = im_[in_x*3+1+in_y*(x_*3)];
      bval = im_[in_x*3+2+in_y*(x_*3)];

      return 1;
   } else
   {
      rval = gval = bval = -1;
      return 0;
   }
}

int BgImage::ReturnCol(int in_x, int in_y, int& rval, int& gval, int& bval)
{
   rval = im_[in_x*3+0+in_y*(x_*3)];
   gval = im_[in_x*3+1+in_y*(x_*3)];
   bval = im_[in_x*3+2+in_y*(x_*3)];
   return 1;
}

bool BgImage::IsAllocated(void) const
{
   return hasIm_;
}

const BgImage& BgImage::operator=(const BgImage& im)
{
   if (this == &im)
      return *this;
   
   if (!im.IsAllocated())
   {
      CleanData();
      return *this;
   }
   
   PrivateCopyToThis(im);
   return *this;
}

void BgImage::PrivateCopyToThis(const BgImage& im)
{
   PrivateResize(im.x_, im.y_, im.colorIm_);
   int ncopy, i;
   ncopy = x_*y_;
   if (colorIm_ == true)
      ncopy *= 3;

   unsigned char *src;
   src = im.im_;
   for (i=0; i<ncopy; i++)
      im_[i] = src[i];
}

void BgImage::PrivateResize(int width, int height, bool color)
{
   if ((hasIm_ == false) || (width != x_) || (height != y_) || (color != colorIm_))
   {
      CleanData();
      x_ = width;
      y_ = height;
      colorIm_ = color;
      if (color == false)
         im_ = new unsigned char[x_*y_];
      else
         im_ = new unsigned char[x_*y_*3];
      hasIm_ = true;
   }
}

void BgImage::Resize(int width, int height, bool color)
{
   PrivateResize(width, height, color);
}