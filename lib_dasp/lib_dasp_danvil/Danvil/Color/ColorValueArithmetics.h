/*
 * ColorValueArithmetics.h
 *
 *  Created on: 15.12.2009
 *     Changed: 15.12.2009
 *      Author: david
 */

#ifndef DANVIL_COLOR_COLOR_VALUE_ARITHMETICS_H_
#define DANVIL_COLOR_COLOR_VALUE_ARITHMETICS_H_
#ifndef DANVIL_COLOR_H_
	#error "Do not include this file directly. Use Danvil/Color.h instead!"
#endif
//---------------------------------------------------------------------------
#include <Danvil/Types.h>
#include <Danvil/Tools/MoreMath.h>
//---------------------------------------------------------------------------
namespace Danvil {
//---------------------------------------------------------------------------

/** Color Arithmetics for floating point types */
template<typename K>
class ColorValueArithmetics
{
public:
	static K Min() { return 0; }
	static K Max() { return 1; }
	static K Half() { return Max()/2; }
	static bool IsZero(K a) { return a == 0; }
	static K Mult(K a, K b) { return a * b;}
	static K Add(K a, K b) { return Min(a + b, Max()); }
	static K Sub(K a, K b) { return Max(a - b, Min()); }
};

/** Color Arithmetics for 8-bit unsigned char type */
template<>
class ColorValueArithmetics<uchar>
{
public:
	static uchar Min() { return 0; }
	static uchar Max() { return 255; }
	static uchar Half() { return 128; }
	static bool IsZero(uchar a) { return a == 0; }
	static uchar Mult(uchar a, uchar b) {
		// multiplicate in 16 bit and reshift to 8 bit accuracy
		return static_cast<uchar>(
				(static_cast<unsigned int>(a) * static_cast<unsigned int>(b)) >> 8
		);
	}
	static uchar Add(uchar a, uchar b) {
		// handle overflow by adding in 16 bit space and clamping to 8-bit
		return static_cast<uchar>(
				MoreMath::Min(
						static_cast<int>(a) + static_cast<int>(b),
						static_cast<int>(Max())
				)
		);
	}

	static uchar Sub(uchar a, uchar b) {
		// handle the case when the result would be negative and return 0 in this case
		return (a < b) ? 0 : (a - b);
	}
};

/** Color Arithmetics for float type */
template<>
class ColorValueArithmetics<float>
{
public:
	static float Min() { return 0.0f; }
	static float Max() { return 1.0f; }
	static float Half() { return 0.5f; }
	static bool IsZero(float a) { return a < 1e-3f; }
	static float Mult(float a, float b) { return a * b;}
	static float Add(float a, float b) { return MoreMath::Min(a + b, Max()); }
	static float Sub(float a, float b) { return MoreMath::Max(a - b, Min()); }
};

/** Color Arithmetics for bool type */
template<>
class ColorValueArithmetics<bool>
{
public:
	static bool Min() { return false; }
	static bool Max() { return true; }
	static bool Half() { return false; }
	static bool IsZero(bool a) { return !a; }
	static bool Mult(bool a, bool b) {
		return a && b;
	}
	static bool Add(bool a, bool b) {
		return a || b;
	}
	static bool Sub(bool a, bool b) {
		return a && !b;
	}
};

//---------------------------------------------------------------------------

template<typename A, typename B>
void ColorValueConversion(const A& a, B& b) {
	b = (B)a;
}

template<typename A>
void ColorValueConversion(const A& a, A& b) {
	b = a;
}

template<> inline
void ColorValueConversion(const float& a, uchar& b) {
	b = uchar(a * 255.0f);
}

template<> inline
void ColorValueConversion(const uchar& a, float& b) {
	b = float(a) / 255.0f;
}

template<> inline
void ColorValueConversion(const float& a, bool& b) {
	b = (a >= 0.5f);
}

template<> inline
void ColorValueConversion(const bool& a, float& b) {
	b = (a ? 1.0f : 0.0f);
}

template<> inline
void ColorValueConversion(const uchar& a, bool& b) {
	b = (a >= 128);
}

template<> inline
void ColorValueConversion(const bool& a, uchar& b) {
	b = (a ? 0 : 255);
}

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
