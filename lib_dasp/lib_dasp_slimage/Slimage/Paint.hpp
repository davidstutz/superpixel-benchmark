/*
 * Paint.hpp
 *
 *  Created on: Feb 17, 2012
 *      Author: david
 */

#ifndef SLIMAGE_PAINT_HPP_
#define SLIMAGE_PAINT_HPP_
//----------------------------------------------------------------------------//
#include "Slimage.hpp"
#include <cmath>
//----------------------------------------------------------------------------//
namespace slimage {
//----------------------------------------------------------------------------//

template<typename T>
void PaintPoint(const slimage::Image<T>& img, int px, int py, const Pixel<T>& color, int size=1)
{
	if(px < 0 || int(img.width()) <= px || py < 0 || int(img.height()) <= py) {
		return;
	}
	if(size == 1) {
		img(px, py) = color;

	}
	else if(size == 2) {
		// paint a star
		//    X
		//   X X
		//    X
		if(1 <= px) {
			img(px-1, py) = color;
		}
		if(px + 1 < int(img.width())) {
			img(px+1, py) = color;
		}
		if(1 <= py) {
			img(px, py-1) = color;
		}
		if(py + 1 < int(img.height())) {
			img(px, py+1) = color;
		}
	}
	else {
		// paint a circle
		//    X
		//   X X
		//  X   X
		//   X X
		//    X
		if(1 <= px && 1 <= py) {
			img(px-1, py-1) = color;
		}
		if(1 <= px && py + 1 < int(img.height())) {
			img(px-1, py+1) = color;
		}
		if(px + 1 < int(img.width()) && 1 <= py) {
			img(px+1, py-1) = color;
		}
		if(px + 1 < int(img.width()) && py + 1 < int(img.height())) {
			img(px+1, py+1) = color;
		}
		if(2 <= px) {
			img(px-2, py) = color;
		}
		if(px + 2 < int(img.width())) {
			img(px+2, py) = color;
		}
		if(2 <= py) {
			img(px, py-2) = color;
		}
		if(py + 2 < int(img.height())) {
			img(px, py+2) = color;
		}
	}
}

/** Paints a line */
template<typename T>
void PaintLine(const slimage::Image<T>& img, int x0, int y0, int x1, int y1, const Pixel<T>& color)
{
//	assert(0 <= x0 && x0 < img.width());
//	assert(0 <= x1 && x1 < img.width());
//	assert(0 <= y0 && y0 < img.height());
//	assert(0 <= y1 && y1 < img.height());
	// taken from http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
	int Dx = x1 - x0;
	int Dy = y1 - y0;
	bool steep = (std::abs(Dy) >= std::abs(Dx));
	if (steep) {
	   std::swap(x0, y0);
	   std::swap(x1, y1);
	   // recompute Dx, Dy after swap
	   Dx = x1 - x0;
	   Dy = y1 - y0;
	}
	int xstep = 1;
	if (Dx < 0) {
	   xstep = -1;
	   Dx = -Dx;
	}
	int ystep = 1;
	if (Dy < 0) {
	   ystep = -1;
	   Dy = -Dy;
	}
	int TwoDy = 2*Dy;
	int TwoDyTwoDx = TwoDy - 2*Dx; // 2*Dy - 2*Dx
	int E = TwoDy - Dx; //2*Dy - Dx
	int y = y0;
	int xDraw, yDraw;
	for(int x = x0; x != x1; x += xstep) {
	   if (steep) {
		   xDraw = y;
		   yDraw = x;
	   } else {
		   xDraw = x;
		   yDraw = y;
	   }
	   // plot
	   if(img.isValidIndex(xDraw, yDraw)) {
		   img(xDraw, yDraw) = color;
	   }
	   // next
	   if (E > 0) {
		   E += TwoDyTwoDx; //E += 2*Dy - 2*Dx;
		   y = y + ystep;
	   } else {
		   E += TwoDy; //E += 2*Dy;
	   }
	}
}

template<typename T>
void PaintEllipse(const slimage::Image<T>& img, int cx, int cy, int ux, int uy, int vx, int vy, const Pixel<T>& color)
{
//	PaintLine(img, cx+ux+vx, cy+uy+vy, cx+ux-vx, cy+uy-vy, color);
//	PaintLine(img, cx+ux-vx, cy+uy-vy, cx-ux-vx, cy-uy-vy, color);
//	PaintLine(img, cx-ux-vx, cy-uy-vy, cx-ux+vx, cy-uy+vy, color);
//	PaintLine(img, cx-ux+vx, cy-uy+vy, cx+ux+vx, cy+uy+vy, color);
	const unsigned int N = 16;
	int last_x = cx + ux;
	int last_y = cy + uy;
	for(unsigned int i=1; i<=N; i++) {
		float phi = static_cast<float>(i) / static_cast<float>(N) * 2.0f * M_PI;
		float cp = std::cos(phi);
		float sp = std::sin(phi);
		int x = cx + static_cast<int>(cp*static_cast<float>(ux) + sp*static_cast<float>(vx));
		int y = cy + static_cast<int>(cp*static_cast<float>(uy) + sp*static_cast<float>(vy));
		PaintLine(img, last_x, last_y, x, y, color);
		last_x = x;
		last_y = y;
	}
}

template<typename T>
void FillEllipse(const slimage::Image<T>& img, int cx, int cy, int ux, int uy, int vx, int vy, const Pixel<T>& color)
{
	// FIXME implement filling!
	const unsigned int N = 16;
	int last_x = cx + ux;
	int last_y = cy + uy;
	for(unsigned int i=1; i<=N; i++) {
		float phi = static_cast<float>(i) / static_cast<float>(N) * 2.0f * M_PI;
		float cp = std::cos(phi);
		float sp = std::sin(phi);
		int x = cx + static_cast<int>(cp*static_cast<float>(ux) + sp*static_cast<float>(vx));
		int y = cy + static_cast<int>(cp*static_cast<float>(uy) + sp*static_cast<float>(vy));
		PaintLine(img, last_x, last_y, x, y, color);
		last_x = x;
		last_y = y;
	}
}

//----------------------------------------------------------------------------//
}
//----------------------------------------------------------------------------//
#endif
