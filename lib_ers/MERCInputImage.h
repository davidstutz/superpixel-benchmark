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
#ifndef _m_erc_input_image_h_
#define _m_erc_input_image_h_

#include "MERCInput.h"
#include "Image.h"
#include <cmath>

using namespace std;

template <class T>
class MERCInputImage: public MERCInput
{
public:
	void ReadImage(Image<T> *inputImage, int conn8=1);	

	int width_;
	int height_;
};

template <class T>
void MERCInputImage<T>::ReadImage(Image<T> *inputImage, int conn8)
{
	width_ = inputImage->width();
	height_ = inputImage->height();
	nNodes_ = width_*height_;
	
	//cout<<width_<<", "<<height_<<endl;
	if(conn8==1)	
		edges_ = new Edge [width_*height_*4];
	else
		edges_ = new Edge [width_*height_*2];
	
	int num = 0;
	for (int y = 0; y < height_; y++)
	{
		for (int x = 0; x < width_; x++) 
		{
			if (x < width_-1) 
			{
				edges_[num].a_ = y * width_ + x;
				edges_[num].b_ = y * width_ + (x+1);
				//edges_[num].w_ = DiffGrayImage(image, x, y, x+1, y);
				edges_[num].w_ = abs(1.0*(inputImage->Access(x,y) - inputImage->Access(x+1,y)));
				num++;
			}

			if (y < height_-1) 
			{
				edges_[num].a_ = y * width_ + x;
				edges_[num].b_ = (y+1) * width_ + x;
				//edges_[num].w_ = DiffGrayImage(image, x, y, x, y+1);
				edges_[num].w_ = abs(1.0*(inputImage->Access(x,y) - inputImage->Access(x,y+1)));
				num++;
			}

			if(conn8==1)
			{
				if ((x < width_-1) && (y < height_-1)) 
				{
					edges_[num].a_ = y * width_ + x;
					edges_[num].b_ = (y+1) * width_ + (x+1);
					//edges_[num].w_ = std::sqrt(2.0)*DiffGrayImage(image, x, y, x+1, y+1);
					edges_[num].w_ = sqrt(2.0)*abs(1.0*(inputImage->Access(x,y) - inputImage->Access(x+1,y+1)));
					num++;
				}

				if ((x < width_-1) && (y > 0)) 
				{
					edges_[num].a_ = y * width_ + x;
					edges_[num].b_ = (y-1) * width_ + (x+1);
					//edges_[num].w_ = std::sqrt(2.0)*DiffGrayImage(image, x, y, x+1, y-1);
					edges_[num].w_ = sqrt(2.0)*abs(1.0*(inputImage->Access(x,y) - inputImage->Access(x+1,y-1)));
					num++;
				}
			}
		}
	}
	nEdges_ = num;
}

#endif