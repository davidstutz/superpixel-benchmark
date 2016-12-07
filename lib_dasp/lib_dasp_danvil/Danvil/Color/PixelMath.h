/*
 * PixelMath.h
 *
 *  Created on: Nov 17, 2010
 *      Author: david
 */

#ifndef DANVIL_COLOR_PIXELMATH_H_
#define DANVIL_COLOR_PIXELMATH_H_
#ifndef DANVIL_COLOR_H_
	#error "Do not include this file directly. Use Danvil/Color.h instead!"
#endif
//---------------------------------------------------------------------------
namespace Danvil {
//---------------------------------------------------------------------------
namespace PixelMath
{

	//---------------------------------------------------------------------------

	template<typename L>
	L red_from_value(uint32_t v) {
		return (L)(v & 0xFF);
	}

	template<typename L>
	L green_from_value(uint32_t v) {
		return (L)((v >> 8) & 0xFF);
	}

	template<typename L>
	L blue_from_value(uint32_t v) {
		return (L)((v >> 16) & 0xFF);
	}

	template<typename L>
	L alpha_from_value(uint32_t v) {
		return (L)(v >> 24);
	}

	/** Creates a new RGBA/BGRA color with changed alpha value */
	inline
	uint32_t set_alpha(const uint32_t& color_xxxa, ui08 alpha) {
		return (color_xxxa & 0x00FFFFFF) + (alpha << 24);
	}

	///** Changes the alpha value of a RGBA/BGRA color value */
	//inline
	//void set_alpha_inplace(uint32_t& color_xxxa, ui08 alpha) {
	//	color_xxxa = set_alpha(color_xxxa, alpha);
	//}

	template<typename L>
	void rgb_from_value(uint32_t v, L& r, L& g, L& b) {
		r = L(v & 0xFF);
		v >>= 8;
		g = L(v & 0xFF);
		v >>= 8;
		b = L(v & 0xFF);
	}

	template<typename L>
	void rgba_from_value(uint32_t v, L& r, L& g, L& b, L& a) {
		r = L(v & 0xFF);
		v >>= 8;
		g = L(v & 0xFF);
		v >>= 8;
		b = L(v & 0xFF);
		v >>= 8;
		a = L(v);
	}

	inline uint32_t rgb_to_int32(unsigned char r, unsigned char g, unsigned char b) {
		return (uint32_t)(r) + ((uint32_t)(g) << 8) + ((uint32_t)(b) << 16) + ((uint32_t)(255) << 24);
	}

	inline uint32_t rgba_to_int32(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
		return (uint32_t)(r) + ((uint32_t)(g) << 8) + ((uint32_t)(b) << 16) + ((uint32_t)(a) << 24);
	}

	template<typename L>
	void bgra_from_value(uint32_t v, L& r, L& g, L& b, L& a) {
		b = L(v & 0x000000FF);
		v >>= 8;
		g = L(v & 0x000000FF);
		v >>= 8;
		r = L(v & 0x000000FF);
		v >>= 8;
		a = L(v);
	}

	inline
	void rgba_from_value_noshift(uint32_t v, uint32_t& r, uint32_t& g, uint32_t& b, uint32_t& a) {
		r = v & 0x000000FF;
		g = v & 0x0000FF00;
		b = v & 0x00FF0000;
		a = v & 0xFF000000;
	}

	inline
	void rgb_from_value_noshift(uint32_t v, uint32_t& r, uint32_t& g, uint32_t& b) {
		r = v & 0x000000FF;
		g = v & 0x0000FF00;
		b = v & 0x00FF0000;
	}

	//---------------------------------------------------------------------------

	inline
	unsigned int DotRgb(unsigned char r1, unsigned char g1, unsigned char b1, unsigned char r2, unsigned char g2, unsigned char b2) {
		// TODO enhance performance
		int d1 = int(r1) - int(r2);
		int d2 = int(g1) - int(g2);
		int d3 = int(b1) - int(b2);
		return (unsigned int)(d1 * d1 + d2 * d2 + d3 * d3);
	}

	inline
	unsigned int DotRgb(uint32_t a, uint32_t b) {
		// TODO enhance performance
		int r1, g1, b1;
		rgb_from_value(a, r1, g1, b1);
		int r2, g2, b2;
		rgb_from_value(b, r2, g2, b2);
		int d1 = r1 - r2;
		int d2 = g1 - g2;
		int d3 = b1 - b2;
		return (unsigned int)(d1 * d1 + d2 * d2 + d3 * d3);
	}

	//---------------------------------------------------------------------------

}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
