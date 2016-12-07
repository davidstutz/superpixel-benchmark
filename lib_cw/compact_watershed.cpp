/*
 * Compact Watershed
 * Copyright (C) 2014  Peer Neubert, peer.neubert@etit.tu-chemnitz.de
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * 
 * ====================================================================== 
 * 
 * This source code is based on the OpenCV library. This file is a modified 
 * version of a OpenCV source code file. All license conditipons for this part 
 * remain unchanged according to the following license:
 * 
 * ====================================================================== 
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
 * ====================================================================== 
 * 
 */

#include "compact_watershed.h"
#include <cxmisc.h>

/****************************************************************************************\
*                                Compact Watershed                                      *
\****************************************************************************************/


using namespace std;
using namespace cv;


namespace cws
{
  typedef struct CvWSNode
  {
      struct CvWSNode* next;
      int mask_ofs;
      int img_ofs;
      float compVal;
  }
  CvWSNode;

  typedef struct CvWSQueue
  {
      CvWSNode* first;
      CvWSNode* last;
  }
  CvWSQueue;

  static CvWSNode* icvAllocWSNodes( CvMemStorage* storage )
  {
      CvWSNode* n = 0;
      
      int i, count = (storage->block_size - sizeof(CvMemBlock))/sizeof(*n) - 1;

      n = (CvWSNode*)cvMemStorageAlloc( storage, count*sizeof(*n) );
      for( i = 0; i < count-1; i++ )
          n[i].next = n + i + 1;
      n[count-1].next = 0;

      return n;
  }


  void  cvWatershed( const CvArr* srcarr, CvArr* dstarr, float compValStep)
  {
      if(compValStep>5)
          printf("cws::cvWatershed Warning: Large compValStep values can cause seg faults");
      
      const int IN_QUEUE = -2;
      const int WSHED = -1;
      const int NQ = 1024;
      cv::Ptr<CvMemStorage> storage;
      
      CvMat sstub, *src;
      CvMat dstub, *dst;
      CvSize size;
      CvWSNode* free_node = 0, *node;
      CvWSQueue q[NQ];
      int active_queue;
      int i, j;
      int db, dg, dr;
      int* mask;
      uchar* img;
      int mstep, istep;
      int subs_tab[2*NQ+1];
      
      // MAX(a,b) = b + MAX(a-b,0)
      #define ws_max(a,b) ((b) + subs_tab[(a)-(b)+NQ])
      // MIN(a,b) = a - MAX(a-b,0)
      #define ws_min(a,b) ((a) - subs_tab[(a)-(b)+NQ])

      #define ws_push(idx,mofs,iofs,cV)  \
      {                               \
          if( !free_node )            \
              free_node = icvAllocWSNodes( storage );\
          node = free_node;           \
          free_node = free_node->next;\
          node->next = 0;             \
          node->mask_ofs = mofs;      \
          node->img_ofs = iofs;       \
          node->compVal = cV;    \
          if( q[idx].last )           \
              q[idx].last->next=node; \
          else                        \
              q[idx].first = node;    \
          q[idx].last = node;         \
      }

      #define ws_pop(idx,mofs,iofs,cV)   \
      {                               \
          node = q[idx].first;        \
          q[idx].first = node->next;  \
          if( !node->next )           \
              q[idx].last = 0;        \
          node->next = free_node;     \
          free_node = node;           \
          mofs = node->mask_ofs;      \
          iofs = node->img_ofs;       \
          cV = node->compVal;       \
      }

      #define c_diff(ptr1,ptr2,diff)      \
      {                                   \
          db = abs((ptr1)[0] - (ptr2)[0]);\
          dg = abs((ptr1)[1] - (ptr2)[1]);\
          dr = abs((ptr1)[2] - (ptr2)[2]);\
          diff = ws_max(db,dg);           \
          diff = ws_max(diff,dr);         \
          assert( 0 <= diff && diff <= 255 ); \
      }

      src = cvGetMat( srcarr, &sstub );
      dst = cvGetMat( dstarr, &dstub );

      if( CV_MAT_TYPE(src->type) != CV_8UC3 )
          CV_Error( CV_StsUnsupportedFormat, "Only 8-bit, 3-channel input images are supported" );

      if( CV_MAT_TYPE(dst->type) != CV_32SC1 )
          CV_Error( CV_StsUnsupportedFormat,
              "Only 32-bit, 1-channel output images are supported" );
      
      if( !CV_ARE_SIZES_EQ( src, dst ))
          CV_Error( CV_StsUnmatchedSizes, "The input and output images must have the same size" );

      size = cvGetMatSize(src);
      storage = cvCreateMemStorage();

      istep = src->step;
      img = src->data.ptr;
      mstep = dst->step / sizeof(mask[0]);
      mask = dst->data.i;

      memset( q, 0, NQ*sizeof(q[0]) );

      for( i = 0; i < NQ; i++ )
          subs_tab[i] = 0;
      for( i = NQ; i <= 2*NQ; i++ )
          subs_tab[i] = i - NQ;
      
      // draw a pixel-wide border of dummy "watershed" (i.e. boundary) pixels
      for( j = 0; j < size.width; j++ )
          mask[j] = mask[j + mstep*(size.height-1)] = WSHED;

      // initial phase: put all the neighbor pixels of each marker to the ordered queue -
      // determine the initial boundaries of the basins
      for( i = 1; i < size.height-1; i++ )
      {
          img += istep; mask += mstep;
          mask[0] = mask[size.width-1] = WSHED;

          for( j = 1; j < size.width-1; j++ )
          {
              int* m = mask + j;
              if( m[0] < 0 ) m[0] = 0;
              if( m[0] == 0 && (m[-1] > 0 || m[1] > 0 || m[-mstep] > 0 || m[mstep] > 0) )
              {
                  uchar* ptr = img + j*3;
                  int idx = 256, t;
                  if( m[-1] > 0 )
                      c_diff( ptr, ptr - 3, idx );
                  if( m[1] > 0 )
                  {
                      c_diff( ptr, ptr + 3, t );
                      idx = ws_min( idx, t );
                  }
                  if( m[-mstep] > 0 )
                  {
                      c_diff( ptr, ptr - istep, t );
                      idx = ws_min( idx, t );
                  }
                  if( m[mstep] > 0 )
                  {
                      c_diff( ptr, ptr + istep, t );
                      idx = ws_min( idx, t );
                  }
                  assert( 0 <= idx && idx <= NQ-1 );
                  ws_push( idx, i*mstep + j, i*istep + j*3, 0.0);
                  m[0] = IN_QUEUE;
              }
          }
      }

      // find the first non-empty queue
      for( i = 0; i < NQ; i++ )
          if( q[i].first )
              break;

      // if there is no markers, exit immediately
      if( i == NQ )
          return;

      active_queue = i;
      img = src->data.ptr;
      mask = dst->data.i;

      // recursively fill the basins
      for(;;)
      {
          int mofs, iofs;
          int lab = 0, t;
          int* m;
          uchar* ptr;
          
          // search for next queue
          if( q[active_queue].first == 0 )
          {
              for( i = active_queue+1; i < NQ; i++ )
                  if( q[i].first )
                      break;
              if( i == NQ )
                  break;
              active_queue = i;
          }

          // get next element of this queue
          float compVal;
          ws_pop( active_queue, mofs, iofs, compVal);

          m = mask + mofs; // pointer to element in mask
          ptr = img + iofs; // pointer to element in image
          
          // have a look at all neighbors, if they have different label, mark
          // as watershed and continue
          t = m[-1];
          if( t > 0 ) lab = t;
          t = m[1];
          if( t > 0 )
          {
              if( lab == 0 ) lab = t;
              else if( t != lab ) lab = WSHED;
          }
          t = m[-mstep];
          if( t > 0 )
          {
              if( lab == 0 ) lab = t;
              else if( t != lab ) lab = WSHED;
          }
          t = m[mstep];
          if( t > 0 )
          {
              if( lab == 0 ) lab = t;
              else if( t != lab ) lab = WSHED;
          }
          assert( lab != 0 );
          m[0] = lab;     
          
          if( lab == WSHED )
            continue;
          
          
          // have a look at all neighbors
          // 
          
          if( m[-1] == 0 )
          {
              c_diff( ptr, ptr - 3, t ); // store difference to this neighbor in t (current gradient)
              ws_push( int(round(t+compVal)), mofs - 1, iofs - 3, compVal+compValStep); // store in queue
              active_queue = ws_min( active_queue, t ); // check if queue of this element is prior to the current queue (and should be proceeded in the next iteration)
              m[-1] = IN_QUEUE; // mark in mask as in a queue
          }
          if( m[1] == 0 )
          {
              c_diff( ptr, ptr + 3, t );
              ws_push( int(round(t+compVal)), mofs + 1, iofs + 3, compVal+compValStep );
              active_queue = ws_min( active_queue, t );
              m[1] = IN_QUEUE;
          }
          if( m[-mstep] == 0 )
          {
              c_diff( ptr, ptr - istep, t );
              ws_push( int(round(t+compVal)), mofs - mstep, iofs - istep, compVal+compValStep );
              active_queue = ws_min( active_queue, t );
              m[-mstep] = IN_QUEUE;
          }
          if( m[mstep] == 0 )
          {
              c_diff( ptr, ptr + istep, t );
              ws_push( int(round(t+compVal)), mofs + mstep, iofs + istep, compVal+compValStep );
              active_queue = ws_min( active_queue, t );
              m[mstep] = IN_QUEUE;
          }
      }
  }


  void compact_watershed( InputArray _src, InputOutputArray markers , float compValStep)
  {
      Mat src = _src.getMat();
      CvMat c_src = _src.getMat(), c_markers = markers.getMat();
      cws::cvWatershed( &c_src, &c_markers,compValStep );
  }
} // namespace cws

void compact_watershed(Mat& img, Mat& B, float n, float compValStep, Mat& seeds)
{
  Mat markers = Mat::zeros(img.rows, img.cols, CV_32SC1);
  if( seeds.empty() )
  {
    // distribute initial markers
    float ny = sqrt( (n*img.rows) / img.cols);
    float nx = n/ny;

    float dx = img.cols / nx;
    float dy = img.rows / ny;
    
    int labelIdx=1;
    for(float i=dy/2; i<markers.rows; i+=dy)
    {
      for(float j=dx/2; j<markers.cols; j+=dx)
      {
        markers.at<int>(floor(i),floor(j)) = labelIdx;
        labelIdx++;
      }
    }
  }
  else
  {
    // use given seeds
    int labelIdx=1;
    for(int i=0; i<seeds.cols; i++)
    {
      //cout << "set "<<round(seeds.at<float>(0,i))<< " "<<round(seeds.at<float>(1,i))<<" to "<<labelIdx<<endl;
      markers.at<int>(round(seeds.at<float>(0,i)),round(seeds.at<float>(1,i))) = labelIdx;
      labelIdx++;      
    }
  }
  
  // run compact watershed
  cws::compact_watershed( img, markers, compValStep);
  
  // create boundary map
  B = markers<0;
  
  // extend boundary map to image borders
  for(int i=0; i<B.cols; i++)
  {
    if(B.at<uchar>(1,i))
      B.at<uchar>(0,i) = 255;      
    else
      B.at<uchar>(0,i) = 0;      
    if(B.at<uchar>(B.rows-2,i))
      B.at<uchar>(B.rows-1,i) = 255;
    else
      B.at<uchar>(B.rows-1,i) = 0;
  }
  
  for(int i=0; i<B.rows; i++)
  {
    if(B.at<uchar>(i,1))
      B.at<uchar>(i,0) = 255;   
    else
      B.at<uchar>(i,0) = 0;     
    if(B.at<uchar>(i, B.cols-2))
      B.at<uchar>(i, B.cols-1) = 255;
    else
      B.at<uchar>(i, B.cols-1) = 0;
  }
}

void compact_watershed(Mat& img, Mat& B, float dy, float dx, float compValStep, Mat& seeds)
{
  Mat markers = Mat::zeros(img.rows, img.cols, CV_32SC1);
  if( seeds.empty() )
  {    
    int labelIdx=1;
    for(float i=dy/2; i<markers.rows; i+=dy)
    {
      for(float j=dx/2; j<markers.cols; j+=dx)
      {
        markers.at<int>(floor(i),floor(j)) = labelIdx;
        labelIdx++;
      }
    }
  }
  else
  {
    // use given seeds
    int labelIdx=1;
    for(int i=0; i<seeds.cols; i++)
    {
      //cout << "set "<<round(seeds.at<float>(0,i))<< " "<<round(seeds.at<float>(1,i))<<" to "<<labelIdx<<endl;
      markers.at<int>(round(seeds.at<float>(0,i)),round(seeds.at<float>(1,i))) = labelIdx;
      labelIdx++;      
    }
  }
  
  // run compact watershed
  cws::compact_watershed( img, markers, compValStep);
  
  // create boundary map
  B = markers<0;
  
  // extend boundary map to image borders
  for(int i=0; i<B.cols; i++)
  {
    if(B.at<uchar>(1,i))
      B.at<uchar>(0,i) = 255;      
    else
      B.at<uchar>(0,i) = 0;      
    if(B.at<uchar>(B.rows-2,i))
      B.at<uchar>(B.rows-1,i) = 255;
    else
      B.at<uchar>(B.rows-1,i) = 0;
  }
  
  for(int i=0; i<B.rows; i++)
  {
    if(B.at<uchar>(i,1))
      B.at<uchar>(i,0) = 255;   
    else
      B.at<uchar>(i,0) = 0;     
    if(B.at<uchar>(i, B.cols-2))
      B.at<uchar>(i, B.cols-1) = 255;
    else
      B.at<uchar>(i, B.cols-1) = 0;
  }
}


