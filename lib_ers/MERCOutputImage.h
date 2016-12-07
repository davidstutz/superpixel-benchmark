/*
Copyright 2011, Ming-Yu Liu

All Rights Reserved 

Permission to use, copy, modify, and distribute this software and 
its documentation for any non-commercial purpose is hereby granted 
without fee, provided that the above copyright notice appear in 
all copies and that both that copyright notice and this permission 
notice appear in supporting documentation, and that the name of 
the author not be used in advertising or publicity pertaining to 
distribution of the software without specific, written prior 
permission. 

THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
ANY PARTICULAR PURPOSE. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR 
ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN 
AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING 
OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 
*/
#ifndef _m_erc_output_image_h_
#define _m_erc_output_image_h_

#include "MERCOutput.h"
#include "Image.h"
#include "ImageIO.h"

using namespace std;

class MERCOutputImage: public MERCOutput
{
public:
	static void StoreSegmentationMap(vector<int> &label,int width,int height,const char *filename);


};

void MERCOutputImage::StoreSegmentationMap(vector<int> &label,int width,int height,const char *filename)
{
	Image<RGBMap> outputImage(width,height);

	RGBMap *color = new RGBMap[width*height];

	for(int i=0; i<width*height; i++)
	{
		color[i].r_ = (uchar)(rand()&255);
		color[i].g_ = (uchar)(rand()&255);
		color[i].b_ = (uchar)(rand()&255);
	}

	for(int y=0;y<height;y++)
	{
		for(int x=0;x<width;x++)
		{
			outputImage.Access(x,y) = color[ label[y*width+x] ];
		}
	}

	ImageIO::SavePPM(&outputImage,filename);

	delete [] color;
}


#endif