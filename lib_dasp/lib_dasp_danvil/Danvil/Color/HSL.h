/*
 * HSL.h
 *
 * Functions to convert between RGB and HSL
 *
 * RGB is represented as [0..255]^3
 * HSL is represented as [0..255]^3 as well
 *
 *  Created on: 31.07.2009
 *      Author: David Weikersdorfer
 */

#ifndef DANVIL_COLOR_HSL_H_
#define DANVIL_COLOR_HSL_H_
//------------------------------------------------------------------------------
#include <Danvil/Types.h>
#include <Danvil/Tools/MoreMath.h>
//------------------------------------------------------------------------------
namespace Danvil {
//------------------------------------------------------------------------------

inline
void convert_rgb_2_hsl(uchar r, uchar g, uchar b, uchar& h, uchar& s, uchar& l) {
	uchar v_min = MoreMath::Min(r,g,b);
	uchar v_max = MoreMath::Max(r,g,b);
	uint p = v_max + v_min;
	uint q = v_max - v_min;
	// hue
	if( q == 0 ) {
		h = 0;
	} else if( v_max == r ) {
		if( g >= b ) {
			h = ((g-b) << 8) / (6*q);
		} else {
			int hi = 256 - ((b-g) << 8) / (6*q);
			if( hi == 256 ) {
				h = 255;
			} else {
				h = hi;
			}
		}
	} else if( v_max == g ) {
		if( b >= r ) {
			h = 85 + ((b-r) << 8) / (6*q);
		} else {
			h = 85 - ((r-b) << 8) / (6*q);
		}
	} else /*if( v_max == b )*/ {
		if( r >= g ) {
			h = 171 + ((r-g) << 8) / (6*q);
		} else {
			h = 171 - ((g-r) << 8) / (6*q);
		}
	}
	// lightness
	l = (uchar)(p >> 1);
	// saturation
	if( q == 0 ) {
		s = 0;
	} else if( (p >> 8) == 0 ) { // p <= 255
		int si = (q << 8) / p; // = 255 * (max-min)/(max+min)
		if( si == 256) {
			s = 255;
		} else {
			s = si;
		}
	} else {
		s = (q << 8) / (512-p); // = 255 * (max-min)/(2-(max+min))
	}
}

inline
float convert_hsl_2_rgb_helper(float t, float p, float q) {
	if(t < 1) {
		return p + t*(q - p);
	} else if(t < 3) {
		return q;
	} else if(t < 4) {
		return p + (4 - t)*(q - p);
	} else {
		return p;
	}
}

inline
void convert_hsl_2_rgb(float h, float s, float l, float& r, float& g, float& b) {
	float q = (l < 0.5f) ? (l*(1.0f + s)) : (l*(1.0f - s) + s);
	float p = 2*l - q;
	h *= 6.0f; // map to range [0,6]
	float tr = h + 2;
	if(tr > 6) { tr -= 6; }
	float tg = h;
	float tb = h - 2;
	if(tb < 0) { tb += 6; }
	r = convert_hsl_2_rgb_helper(tr, p, q);
	g = convert_hsl_2_rgb_helper(tg, p, q);
	b = convert_hsl_2_rgb_helper(tb, p, q);
}

inline
void convert_hsl_2_rgb(uchar h, uchar s, uchar l, uchar& r, uchar& g, uchar& b) {
	float rf, gf, bf;
	convert_hsl_2_rgb(
			((float)h)/256.0f,
			((float)s)/256.0f,
			((float)l)/256.0f,
			rf, gf, bf);
	r = rf * 255.0f;
	g = gf * 255.0f;
	b = bf * 255.0f;
}

//------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------
#endif /* DANVIL_COLOR_HSL_H_ */
