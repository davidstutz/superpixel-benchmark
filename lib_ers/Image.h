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

#ifndef _image_h_
#define _image_h_

#include <cmath>
#include <cstring>
#include <assert.h>
typedef unsigned char uchar;

/* use imRef to access Image data. */
#define imRef(im, x, y) (im->access[y][x])
  
/* use imPtr to get pointer to Image data. */
#define imPtr(im, x, y) &(im->access[y][x])

class RGBMap
{
public:
	RGBMap(uchar r,uchar g,uchar b): r_(r),g_(g),b_(b) {};
	RGBMap() {};
	uchar r_, g_, b_;
	inline RGBMap& operator=(const RGBMap &rhs);
	inline double operator-(const RGBMap &other) const;
};

double RGBMap::operator-(const RGBMap &other) const
{
	double diff = 0;
	diff += abs(1.0*r_ - other.r_);
	diff += abs(1.0*g_ - other.g_);
	diff += abs(1.0*b_ - other.b_);
	return diff;
    
//        double diff = 0;
//	diff += (r_ - other.r_)*(r_ - other.r_);
//	diff += (g_ - other.g_)*(g_ - other.g_);
//	diff += (b_ - other.b_)*(b_ - other.b_);
//	return std::sqrt(diff);
}

RGBMap& RGBMap::operator=(const RGBMap &rhs)
{
	r_ = rhs.r_, g_ = rhs.g_, b_ = rhs.b_;
	return (*this);
}


template <class T>
class Image 
{
	public:

	// constructor
	inline Image();

	/* create an Image */
	inline Image(const int width, const int height, const bool init = true);

	/* delete an Image */
	inline ~Image();

	/* release current image if any */
	inline void Release();

	inline void Resize(const int width,const int height, const bool init = true);

	

	/* init an Image */
	inline void Init(const T &val);

	/* copy an Image */
	inline Image<T> *Copy() const;

	/* get the width of an Image. */
	inline int width() const { return w; }

	/* get the height of an Image. */
	inline int height() const { return h; }

	// returning a reference to the parituclar location.
	inline T& Access(int x,int y) {return access[y][x];};


	/* Image data. */
	T *data;

	/* row pointers. */
	T **access;

	

private:
	int w, h;
};


template <class T>
Image<T>::Image()
{
	w = 0;
	h = 0;
	data = NULL;
	access = NULL;
}


template <class T>
Image<T>::Image(const int width, const int height, const bool init) 
{
	w = width;
	h = height;
	data = new T[w * h];  // allocate space for Image data
	access = new T*[h];   // allocate space for row pointers

	// initialize row pointers
	for (int i = 0; i < h; i++)
		access[i] = data + (i * w);  

	if (init)
		memset(data, 0, w * h * sizeof(T));
}

template <class T>
Image<T>::~Image() 
{
	Release();
}

template <class T>
void Image<T>::Release()
{
	if(data)
		delete [] data;
	if(access)
		delete [] access;

	h = 0;
	w = 0;
}


template <class T>
void Image<T>::Resize(const int width, const int height, const bool init) 
{
	Release();
	w = width;
	h = height;
	data = new T[w * h];  // allocate space for Image data
	access = new T*[h];   // allocate space for row pointers

	// initialize row pointers
	for (int i = 0; i < h; i++)
		access[i] = data + (i * w);  

	if (init)
		memset(data, 0, w * h * sizeof(T));
}

template <class T>
void Image<T>::Init(const T &val) 
{
	T *ptr = imPtr(this, 0, 0);
	T *end = imPtr(this, w-1, h-1);
	while (ptr <= end)
		*ptr++ = val;
}


template <class T>
Image<T> *Image<T>::Copy() const 
{
	Image<T> *im = new Image<T>(w, h, false);
	memcpy(im->data, data, w * h * sizeof(T));
	return im;
}

#endif
  
