
#ifndef FRIENDFUNCTIONS_H
#define FRIENDFUNCTIONS_H

#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
//#include <atlstr.h>
#include "opencv2/opencv.hpp"


using namespace std;
typedef unsigned char uchar;
#define p3f(Y,r,c,chan) (float)(((uchar*)(Y->imageData + Y->widthStep*(r)))[(c)*3+(chan)])
#define p3(Y,r,c,chan) (((uchar*)(Y->imageData + Y->widthStep*(r)))[(c)*3+(chan)])
#define pM(Y,r,c) ((double*)(Y->data.ptr+ Y->step*r))[c]

uchar* Ipl2uchar(IplImage* img)
{
	uchar* tempIm = new uchar[3*img->height*img->width];
	for(int i=0;i<img->height;i++)
		for(int j=0;j<img->width;j++)
			for(int k=0;k<3;k++)
				tempIm[3*(i*img->width+j)+k] = p3(img,i,j,k);

	return tempIm;
}
void Ipl2uchar_Nomem(IplImage* img,uchar *target)
{
	for(int i=0;i<img->height;i++)
		for(int j=0;j<img->width;j++)
			for(int k=0;k<3;k++)
				target[3*(i*img->width+j)+k] = p3(img,i,j,k);

}
void Mat2uchar_Nomem(cv::Mat* img,uchar *target)
{
        for(int i=0;i<img->rows;i++)
		for(int j=0;j<img->cols;j++)
			for(int k=0;k<3;k++)
				target[3*(i*img->cols+j)+k] = img->at<cv::Vec3b>(i, j)[k];
}
void uchar2Ipl(uchar *tempIm,IplImage* img)
{
	for(int i=0;i<img->height;i++)
		for(int j=0;j<img->width;j++)
			for(int k=0;k<3;k++)
				 p3(img,i,j,k) = tempIm[3*(i*img->width+j)+k];
}
void uchar2Mat(uchar *tempIm,cv::Mat* img)
{
        for(int i=0;i<img->rows;i++)
		for(int j=0;j<img->cols;j++)
                        img->at<int>(i, j) = tempIm[3*(i*img->cols+j)+0] 
                                + 256*tempIm[3*(i*img->cols+j)+1] + 256*256*tempIm[3*(i*img->cols+j)+2];
}
void int2Mat(int *tempIm,cv::Mat* img)
{
        for(int i=0;i<img->rows;i++)
		for(int j=0;j<img->cols;j++)
                        img->at<int>(i, j) = tempIm[i*img->cols+j];
}
#endif