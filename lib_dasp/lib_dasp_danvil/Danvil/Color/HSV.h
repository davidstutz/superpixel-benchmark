/*
 * HSV.h
 *
 * Functions to convert between RGB and HSV
 *
 * RGB is represented as [0..255]^3
 * HSV is represented as [0..255]^3 as well
 *
 *  Created on: 31.07.2009
 *      Author: David Weikersdorfer
 */

#ifndef DANVIL_COLOR_HSV_H_
#define DANVIL_COLOR_HSV_H_
//------------------------------------------------------------------------------
#include <Danvil/Types.h>
#include <Danvil/Tools/MoreMath.h>
#include <Danvil/Tools/Unit.h>
//------------------------------------------------------------------------------
namespace Danvil {
//------------------------------------------------------------------------------

inline
void convert_rgb_2_hsv(uchar r, uchar g, uchar b, uchar& h, uchar& s, uchar& v) {
	uchar v_min = MoreMath::Min(r,g,b);
	uchar v_max = MoreMath::Max(r,g,b);
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
	// saturation
	if( v_max == 0 ) {
		s = 0;
	} else {
		int si = (q<<8) / v_max;
		if( si == 256 ) {
			s = 255;
		} else {
			s = si;
		}
	}
	// lightness
	v = v_max;
}

inline
void convert_hsv_2_rgb(uchar h, uchar s, uchar v, uchar& r, uchar& g, uchar& b) {
	// look up hue range
	int hi = (int)h;
	int hi_int;
	int f;
	if(h < 43) { hi_int = 0; f = (hi*255) / 42; }
	else if(h < 85) { hi_int = 1; f = ((hi - 43)*255) / 43; }
	else if(h < 128) { hi_int = 2; f = ((hi - 85)*255) / 42; }
	else if(h < 171) { hi_int = 3; f = ((hi - 128)*255) / 43; }
	else if(h < 213) { hi_int = 4; f = ((hi - 171)*255) / 42; }
	else { hi_int = 5; f = ((h - 213)*255) / 43; }
	// helpers
	int si = (int)s;
	int vi = (int)v;
	int p = (vi * (255 - si)) >> 8;
	int q = (vi * (255*255 - si*f)) >> 16;
	int t = (vi * (255*255 - (255 - f)*si)) >> 16;
	// rgb
	switch(hi_int) {
	case 0: r = v; g = t; b = p; return; // -q
	case 1: r = q; g = v; b = p; return; // -t
	case 2: r = p; g = v; b = t; return; // -q
	case 3: r = p; g = q; b = v; return; // -t
	case 4: r = t; g = p; b = v; return; // -q
	case 5: r = v; g = p; b = q; return; // -t
	}
}

/** Converts RGB to HSV
 * r,g,b \in [0,1] -> h,s,v \in [0,1]
 */
inline
void convert_rgb_2_hsv(float r, float g, float b, float& h, float& s, float& v) {
	float max = std::max(r, std::max(g, b));
	float min = std::min(r, std::min(g, b));
	v = max;
	float delta = max - min;
	if(delta == 0) {
		s = 0.0f;
		h = 0.0f;
		return;
	}
	s = delta / max; // delta > 0 => max > 0
	if( r == max )
		// between yellow & magenta
		h = ( g - b ) / delta;
	else if( g == max )
		// between cyan & yellow
		h = 2 + ( b - r ) / delta;
	else
		// between magenta & cyan
		h = 4 + ( r - g ) / delta;
	// degrees
	if( h < 0 )
		h += 6;
	h /= 6.0f;
}

inline
void convert_hsv_2_rgb(float h, float s, float v, float& r, float& g, float& b) {
	double hi;
	float f;
	// clamp h to [0|360[
	//h = fmod(h / 360.0f, h) * 360.0f;
	// compute h = 6 * hi + f;
	f = std::modf(h*6.0f, &hi);
	// helpers
	float p = v * (1 - s);
	float q = v * (1 - s*f);
	float t = v * (1 - (1-f)*s);
	// rgb
	int hi_int = hi;
	switch(hi_int % 6) {
	case 0: r = v; g = t; b = p; return;
	case 1: r = q; g = v; b = p; return;
	case 2: r = p; g = v; b = t; return;
	case 3: r = p; g = q; b = v; return;
	case 4: r = t; g = p; b = v; return;
	case 5: r = v; g = p; b = q; return;
	}
}

//------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------
#endif /* DANVIL_COLOR_HSV_H_ */
