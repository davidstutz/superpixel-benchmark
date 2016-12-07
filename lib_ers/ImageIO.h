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

#ifndef _image_io_h_
#define _image_io_h_

#include <cstdlib>
#include <climits>
#include <cstring>
#include <fstream>
#include "Image.h"


#define BUF_SIZE 256


class pnm_error
{ 

};

class ImageIO
{
public:
	inline static Image<uchar> *LoadPBM(const char *name);	
	inline static Image<uchar> *LoadPGM(const char *name);
	inline static Image<RGBMap> *LoadPPM(const char *name);
	template <class T>inline  void LoadImage(Image<T> **im, const char *name);

	inline static void SavePBM(Image<uchar> *im, const char *name);
	inline static void SavePGM(Image<uchar> *im, const char *name);
	inline static void SavePPM(Image<RGBMap> *im, const char *name);
	template <class T>inline  void SaveImage(Image<T> *im, const char *name);

private:


	inline static void read_packed(unsigned char *data, int size, std::ifstream &f);
	inline static void write_packed(unsigned char *data, int size, std::ofstream &f);
	inline static void pnm_read(std::ifstream &file, char *buf);
};



void ImageIO::read_packed(unsigned char *data, int size, std::ifstream &f)
{
	unsigned char c = 0;
	int bitshift = -1;
	for (int pos = 0; pos < size; pos++) 
	{
		if (bitshift == -1) 
		{
			c = f.get();
			bitshift = 7;
		}
		data[pos] = (c >> bitshift) & 1;
		bitshift--;
	}
}

void ImageIO::write_packed(unsigned char *data, int size, std::ofstream &f)
{
	unsigned char c = 0;

	int bitshift = 7;
	for (int pos = 0; pos < size; pos++) 
	{
		c = c | (data[pos] << bitshift);
		bitshift--;
		if ((bitshift == -1) || (pos == size-1)) 
		{
			f.put(c);
			bitshift = 7;
			c = 0;
		}
	}
}

/* read PNM field, skipping comments */ 
void ImageIO::pnm_read(std::ifstream &file, char *buf) 
{
	char doc[BUF_SIZE];
	char c;

	file >> c;
	while (c == '#') 
	{
		file.getline(doc, BUF_SIZE);
		file >> c;
	}
	file.putback(c);
	file.width(BUF_SIZE);
	file >> buf;
	file.ignore();
}

Image<uchar> *ImageIO::LoadPBM(const char *name) 
{
	char buf[BUF_SIZE];

	/* read header */
	std::ifstream file(name, std::ios::in | std::ios::binary);
	pnm_read(file, buf);
	if (strncmp(buf, "P4", 2))
		throw pnm_error();

	pnm_read(file, buf);
	int width = atoi(buf);
	pnm_read(file, buf);
	int height = atoi(buf);

	/* read data */
	Image<uchar> *im = new Image<uchar>(width, height);
	for (int i = 0; i < height; i++)
		read_packed(imPtr(im, 0, i), width, file);

	return im;
}

void ImageIO::SavePBM(Image<uchar> *im, const char *name) 
{
	int width = im->width();
	int height = im->height();
	std::ofstream file(name, std::ios::out | std::ios::binary);
	file << "P4\n" << width << " " << height << "\n";
	for (int i = 0; i < height; i++)
		write_packed(imPtr(im, 0, i), width, file);
}

Image<uchar> *ImageIO::LoadPGM(const char *name) 
{
	char buf[BUF_SIZE];

	/* read header */
	std::ifstream file(name, std::ios::in | std::ios::binary);
	pnm_read(file, buf);
	if (strncmp(buf, "P5", 2))
		return NULL;

	pnm_read(file, buf);
	int width = atoi(buf);
	pnm_read(file, buf);
	int height = atoi(buf);

	pnm_read(file, buf);
	if (atoi(buf) > UCHAR_MAX)
		return NULL;

	/* read data */
	Image<uchar> *im = new Image<uchar>(width, height);
	file.read((char *)imPtr(im, 0, 0), width * height * sizeof(uchar));

	return im;
}

void ImageIO::SavePGM(Image<uchar> *im, const char *name)
{
	int width = im->width();
	int height = im->height();
	std::ofstream file(name, std::ios::out | std::ios::binary);

	file << "P5\n" << width << " " << height << "\n" << UCHAR_MAX << "\n";
	file.write((char *)imPtr(im, 0, 0), width * height * sizeof(uchar));
}

Image<RGBMap> *ImageIO::LoadPPM(const char *name) 
{
	char buf[BUF_SIZE];

	/* read header */
	std::ifstream file(name, std::ios::in | std::ios::binary);
	pnm_read(file, buf);
	if (strncmp(buf, "P6", 2))
		throw pnm_error();

	pnm_read(file, buf);
	int width = atoi(buf);
	pnm_read(file, buf);
	int height = atoi(buf);

	pnm_read(file, buf);
	if (atoi(buf) > UCHAR_MAX)
		throw pnm_error();

	/* read data */
	Image<RGBMap> *im = new Image<RGBMap>(width, height);
	file.read((char *)imPtr(im, 0, 0), width * height * sizeof(RGBMap));

	return im;
}

void ImageIO::SavePPM(Image<RGBMap> *im, const char *name) 
{
	int width = im->width();
	int height = im->height();
	std::ofstream file(name, std::ios::out | std::ios::binary);

	file << "P6\n" << width << " " << height << "\n" << UCHAR_MAX << "\n";
	file.write((char *)imPtr(im, 0, 0), width * height * sizeof(RGBMap));
}
template <class T>
void ImageIO::LoadImage(Image<T> **im, const char *name) 
{
	char buf[BUF_SIZE];

	/* read header */
	std::ifstream file(name, std::ios::in | std::ios::binary);
	pnm_read(file, buf);
	if (strncmp(buf, "VLIB", 9))
		throw pnm_error();

	pnm_read(file, buf);
	int width = atoi(buf);
	pnm_read(file, buf);
	int height = atoi(buf);

	/* read data */
	*im = new Image<T>(width, height);
	file.read((char *)imPtr((*im), 0, 0), width * height * sizeof(T));
}

template <class T>
void ImageIO::SaveImage(Image<T> *im, const char *name) 
{
	int width = im->width();
	int height = im->height();
	std::ofstream file(name, std::ios::out | std::ios::binary);

	file << "VLIB\n" << width << " " << height << "\n";
	file.write((char *)imPtr(im, 0, 0), width * height * sizeof(T));
}

#endif
