/////////////////////////////////////////////////////////////////////////////
// Name:        BgImage.h
// Purpose:     BgImage class
// Author:      Bogdan Georgescu
// Modified by:
// Created:     06/22/2000
// Copyright:   (c) Bogdan Georgescu
// Version:     v0.1
/////////////////////////////////////////////////////////////////////////////

#ifndef _BG_IMAGE_H
#define _BG_IMAGE_H

#include <assert.h>

#define RED_WEIGHT 0.299
#define GREEN_WEIGHT 0.587
#define BLUE_WEIGHT 0.114

class BgImage
{
public:
   int x_;
   int y_;
   unsigned char* im_;
   bool hasIm_;
   bool colorIm_; // false - bw image
                  // true  - color RGB image

   BgImage();
   BgImage(int, int, bool colorIm = false);
   ~BgImage();

   void SetImage(unsigned char*, int, int, bool colorIm = false);
   void SetImageFromRGB(unsigned char*, int, int, bool colorIm = false);
   void SetSameImageFromRGB(unsigned char*);
   void SetImage(short*, int, int, bool colorIm = false);
   void GetImage(unsigned char*);
   void GetImageBW(unsigned char*);
   void GetImageColor(unsigned char*);
   void GetImageR(unsigned char*);
   void GetImageG(unsigned char*);
   void GetImageB(unsigned char*);
   bool ValidCoord(int, int);
   int ValidReturnBW(int in_x, int in_y, int& cval);
   int ValidReturnCol(int in_x, int in_y, int& rval, int& gval, int& bval);
   int ReturnCol(int in_x, int in_y, int& rval, int& gval, int& bval);

   inline int GetWidth() {return x_;};
   inline int GetHeight() {return y_;};


   const BgImage& operator=(const BgImage& im);
   bool IsAllocated(void) const;

   void CleanData();
   inline unsigned char operator()(int, int) const;
   inline unsigned char& operator()(int, int);
   unsigned char PixelValue(int, int);

   void Resize(int width, int height, bool color);

private:
   void PrivateCopyToThis(const BgImage& im);
   void PrivateResize(int width, int height, bool color);


};
extern inline unsigned char gBgImPt(BgImage*, int, int);

#endif