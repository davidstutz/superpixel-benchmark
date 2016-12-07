


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

void rgb2Lab(uchar* src, int * dst,int mH,int mW)
{
	
	float var_R,var_G,var_B,var_X,var_Y,var_Z;

	for (int j=0;j<mH;j++)
	{
		for (int i=0;i<mW;i++)
		{
			if(i==61 && j==628)
				j=628;
			var_R = (float)src[3*(i+mW*j)+2]/(float)255;
			var_G = (float)src[3*(i+mW*j)+1]/(float)255;
			var_B = (float)src[3*(i+mW*j)]/(float)255;
			

			if ( var_R > 0.04045 )
			{
				var_R = ( ( var_R + 0.055 ) / 1.055 );
				var_R = pow(var_R,float(2.4));
			}
			else
			{
				var_R = var_R / 12.92;
			}

			if ( var_G > 0.04045 )
			{
				var_G = ( ( var_G + 0.055 ) / 1.055 );
				var_G = pow(var_G,float(2.4));
			}
			else
			{
				var_G = var_G / 12.92;
			}

			if ( var_B > 0.04045 )
			{
				var_B = ( ( var_B + 0.055 ) / 1.055 );
				var_B = pow(var_B,float(2.4));
			}
			else
			{
				var_B = var_B / 12.92;
			}
			
			var_R = var_R * 100;
			var_G = var_G * 100;
			var_B = var_B * 100;

			var_X = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
			var_Y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
			var_Z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;
        
			var_X = var_X / 95.047;         
			var_Y = var_Y / 100;          
			var_Z = var_Z / 108.883;          

			if ( var_X > 0.008856 )
			{
				var_X = pow(var_X ,float(0.3333));
			}
			else
			{
				var_X = ( 7.787 * var_X ) + 0.13793;
			}

			if ( var_Y > 0.008856 )
			{
				var_Y = pow(var_Y ,float(0.3333));
			}
			else
			{
				var_Y = ( 7.787 * var_Y ) + 0.13793;
			}

			if ( var_Z > 0.008856 )
			{
				var_Z = pow(var_Z ,float(0.3333));
			}
			else
			{
				var_Z = ( 7.787 * var_Z ) + 0.13793;
			}

			dst[3*(i+mW*j)] = 2.5*( 116 * var_Y ) - 16;
			dst[3*(i+mW*j)+1] = 10*500 * ( var_X - var_Y );
			dst[3*(i+mW*j)+2] = 10*200 * ( var_Y - var_Z );
		}
	}

	//float max_val1 = 0;
	//float max_val2 = 0;
	//float min_val1 = 100;
	//float min_val2 = 100;

	//for(int i=0;i<mW*mH;i++)
	//{
	//	if(dst[3*i+1] > max_val1)
	//		max_val1 = dst[3*i+1];

	//	if(dst[3*i+1] < min_val1)
	//		min_val1 = dst[3*i+1];

	//	if(dst[3*i+2] > max_val2)
	//		max_val2 = dst[3*i+2];

	//	if(dst[3*i+2] < min_val2)
	//		min_val2 = dst[3*i+2];
	//}

	//for(int i=0;i<mW*mH;i++)
	//{
	//	dst[3*i+1] = 255*(dst[3*i+1]-min_val1)/(max_val1 - min_val1);
	//	dst[3*i+2] = 255*(dst[3*i+2]-min_val2)/(max_val2 - min_val2);
	//	dst[3*i]   = 2.5*dst[3*i];
	//}
}

