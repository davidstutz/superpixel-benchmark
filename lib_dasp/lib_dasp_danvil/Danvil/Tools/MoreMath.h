/*
 * MoreMath.h
 *
 *  Created on: Nov 15, 2010
 *      Author: david
 */

#ifndef DANVIL_MOREMATH_H_
#define DANVIL_MOREMATH_H_
//---------------------------------------------------------------------------
#include <Danvil/Types.h>
#include "Constants.h"
#include <cmath>
#include <cstdlib> // for abs(int)
#include <algorithm> // for std::swap
//---------------------------------------------------------------------------
namespace Danvil {
//---------------------------------------------------------------------------
namespace MoreMath
{
	//---------------------------------------------------------------------------

	/** Sign of a number
	 * @return -1, if x is negative; +1, if x is positive; and 0, if x is 0
	 */
	template<typename K>
	int Sign(const K& x) {
		return x < 0 ? -1 : (x > 0 ? +1 : 0);
	}

	//---------------------------------------------------------------------------

	/** Square of x
	 * @return x * x
	 */
	template<typename K>
	K Square(K x) {
		return x * x;
	}

	/** Square of difference of a and b
	 * @return (a - b) * (a - b)
	 */
	template<typename K>
	K SquareDiff(K a, K b) {
		return Square(a - b);
	}

	// disable warning about strict aliasing for function FastInverseSqrt
	// TODO disable warning for MSVC
	// FIXME without push and pop this has the effect, that strict-aliasing warnings are removed for all following code!
#if defined(__GNUC__)
//	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
	/** Computes a good approximation for the inverse square root using a tricky method
	 * See http://en.wikipedia.org/wiki/Fast_inverse_square_root
	 * @return an good approximation for 1 / sqrt(x)
	 */
	inline
	float FastInverseSqrt(float x) {
		uint32_t i = *((uint32_t *)&x);			// evil floating point bit level hacking
		i = 0x5f3759df - (i >> 1);				// use a magic number
		float s = *((float *)&i);				// get back guess
		return s * (1.5f - 0.5f * x * s * s);	// one newton iteration
	}
#if defined(__GNUC__)
//	#pragma GCC diagnostic pop
#endif

	//---------------------------------------------------------------------------

	/** Swaps a and b using XOR
	 * See http://www-graphics.stanford.edu/~seander/bithacks.html#SwappingValuesXOR
	 */
	template<typename T>
	void SwapXOR(T& a, T& b) {
		// FIXME restrict do basic types
		// normal: T t = a; a = b; b = t;
		// faster (?):
		(a == b) || ((a ^= b), (b ^= a), (a ^= b));
	}

	//---------------------------------------------------------------------------

	/** Rounds a floating number to the nearest integer value s.t. 0.5 will be rounded to 1
	 * All reals {a + x | x \in [-0.5|+0.5[} are rounded to a for any integer a
	 */
	template<typename K>
	K RoundHalfUp(K x) {
		return std::floor(x + 0.5);
	}

	/** Rounds a floating number to the nearest integer value s.t. 0.5 will be rounded to 0 */
	template<typename K>
	K RoundHalfDown(K x) {
		return std::ceil(x - 0.5);
	}

	/** Same as RoundHalfUp */
	template<typename K>
	K Round(K x) {
		return RoundHalfUp(x);
	}

	/** Same as Round, but returns a integer
	 * Attention beware of overflows!
	 */
	template<typename K>
	int RoundToInt(K x) {
		return static_cast<int>(Round(x));
	}

	/** Same as Round, but returns a long integer
	 * Attention beware of overflows!
	 */
	template<typename K>
	long RoundToLong(K x) {
		return static_cast<long>(Round(x));
	}

	/** Rounds a floating point number to n digits after the decimal point */
	template<typename K>
	K Round(K t, int n) {
		K base = PowN(K(10), n);
		return Round(t * base) / base;
	}

	/** Ermittelt Rest und Ergebnis bei ganzzahliger Teilung von x / n.
	 * Berechnet a und 0 <= b < n, so dass
	 *    x = b*n + a
	 * Dabei sind a und b eindeutig bestimmt.
	 */
	template<typename K>
	void Split(K x, K n, K& a, K& b) {
		a = x % n;
		b = x / n;
	}

	/** Interpolates between a and b
	 * Returns value c which is p% like b and and (1-p)% like a
	 */
	template<typename K, typename T>
	T Interpolate(K t, const T& a, const T& b) {
		return (1 - t)*a + t*b;
	}

	//---------------------------------------------------------------------------

	/** Computes sin and cos at once together (which is faster than computing it individually) */
	template<typename K>
	void SinCos(K angle, K& sin_value, K& cos_value) {
		// FIXME use assembler or rely on optimization?
		sin_value = std::sin(angle);
		cos_value = std::cos(angle);
	}

	template<typename K>
	K ProtectedAcos(K x, bool* ok=0) {
		if(x >= K(1)) {
			if(ok) *ok = false;
			return K(0);
		}
		else if(x <= -K(1)) {
			if(ok) *ok = false;
			return K(Danvil::C_PI);
		}
		else {
			if(ok) *ok = true;
			return std::acos(x);
		}
	}

	//---------------------------------------------------------------------------

	/** x to the power of 2 with 1 multiplication */
	template<typename K>
	K Pow2(K x) {
		return x * x;
	}

	/** x to the power of 3 with 2 multiplications */
	template<typename K>
	K Pow3(K x) {
		//return x * pow_2(x);
		return x * x * x;
	}

	/** x to the power of 4 with 2 multiplications */
	template<typename K>
	K Pow4(K x) {
		//return pow_2(pow_2(x));
		K x2 = x * x;
		return x2 * x2;
	}

	/** x to the power of 5 with 3 multiplications */
	template<typename K>
	K Pow5(K x) {
		//return x * pow_4(x);
		K x2 = x * x;
		return x * x2 * x2;
	}

	/** x to the power of 6 with 3 multiplications */
	template<typename K>
	K Pow6(K x) {
		//return pow_3(pow_2(x));
		K x2 = x * x;
		return x2 * x2 * x2;
	}

	/** x to the power of 7 with 4 multiplications */
	template<typename K>
	K Pow7(K x) {
		//return x * pow_6(x);
		K x2 = x * x;
		return x * x2 * x2 * x2;
	}

	/** x to the power of 8 with 3 multiplications */
	template<typename K>
	K Pow8(K x) {
		//return pow_2(pow_4(x));
		K x2 = x * x;
		K x4 = x2 * x2;
		return x4 * x4;
	}

	/** x to the power of 9 with 4 multiplications */
	template<typename K>
	K Pow9(K x) {
		//return pow_2(pow_4(x));
		K x2 = x * x;
		K x4 = x2 * x2;
		return x * x4 * x4;
	}

	/** x to the power of n with n an integer
	 * Uses an explicit method for n < 10, and std::pow for n >= 10
	 * If exponent is a constant and smaller than 10, the compiler will probably optimize out this call.
	 */
	template<typename K>
	K PowN(K x, int exponent) {
		switch(exponent) {
		case 0: return K(1);
		case 1: return x;
		case 2: return Pow2(x);
		case 3: return Pow3(x);
		case 4: return Pow4(x);
		case 5: return Pow5(x);
		case 6: return Pow6(x);
		case 7: return Pow7(x);
		case 8: return Pow8(x);
		case 9: return Pow9(x);
		default:
			return std::pow(x, exponent);
		}
	}

	template<typename K, unsigned int N>
	K PowN(K x) {
		switch(N) {
		case 0: return K(1);
		case 1: return x;
		case 2: return Pow2(x);
		case 3: return Pow3(x);
		case 4: return Pow4(x);
		case 5: return Pow5(x);
		case 6: return Pow6(x);
		case 7: return Pow7(x);
		case 8: return Pow8(x);
		case 9: return Pow9(x);
		default:
			return std::pow(x, N);
		}
	}

	//---------------------------------------------------------------------------

	/** Computes a*a + b*b */
	template<typename K>
	K Dot(K a, K b) {
		return Square(a) + Square(b);
	}

	/** Computes a*a + b*b + c*c */
	template<typename K>
	K Dot(K a, K b, K c) {
		return Square(a) + Square(b) + Square(c);
	}

	/** Computes the dot product (a_1 - b_1)^2 + (a_2 - b_2)^2 */
	template<typename K>
	K DotDiff(K a1, K a2, K b1, K b2) {
		return SquareDiff(a1, b1) + SquareDiff(a2, b2);
	}

	/** Computes the dot product (a_1 - b_1)^2 + (a_2 - b_2)^2 + (a_3 - b_3)^2 */
	template<typename K>
	K DotDiff(K a1, K a2, K a3, K b1, K b2, K b3) {
		return SquareDiff(a1, b1) + SquareDiff(a2, b2) + SquareDiff(a3, b3);
	}

	/** Computes a*a + b*b + c*c */
	template<typename K>
	K Length(K a, K b, K c) {
		return std::sqrt(Dot(a, b, c));
	}

	//---------------------------------------------------------------------------

	/** Returns true if value 'x' lies in the closed interval [min,max] */
	template<typename K>
	bool InInterval(K x, K min, K max) {
		return min <= x && x <= max;
	}

	/** Clamps x to the inveral [min|max]
	 * If min > max the return value is min!
	 */
	template<typename K>
	void ClampInplace(K& x, K min, K max) {
		if( x < min ) {
			x = min;
		}
		else if( x > max ) {
			x = max;
		}
	}

	inline float Fmod(float a, float b) { return std::fmod(a, b); }
	inline double Fmod(double a, double b) { return std::fmod(a, b); }
	inline long double Fmod(long double a, long double b) { return std::fmod(a, b); }
	inline unsigned short Fmod(unsigned short a, unsigned short b) { return a % b; }
	inline unsigned int Fmod(unsigned int a, unsigned int b) { return a % b; }
	inline long unsigned int Fmod(long unsigned int a, long unsigned int b) { return a % b; }

	/** Clamps x to the inveral [min|max]
	 * If min > max the return value is min!
	 */
	template<typename K>
	K Clamp(K x, K min, K max) {
		return (x < min) ? min : (x > max ? max : x);
	}

	/** Computes 0 <= a < delta and n \in Z s.t. x = delta * n + a */
	template<typename K>
	K Wrap(K x, K delta) {
		// TODO assert(delta > K(0));
		x = Fmod(x, delta);
		return (x < 0) ? (x + delta) : x;
	}

	inline unsigned short Wrap(unsigned short x, unsigned short delta) {
		return Fmod(x, delta);
	}

	inline unsigned int Wrap(unsigned int x, unsigned int delta) {
		return Fmod(x, delta);
	}

	inline long unsigned int Wrap(long unsigned int x, long unsigned int delta) {
		return Fmod(x, delta);
	}

	template<typename K>
	K Wrap(K x, K min, K max) {
		return Wrap(x - min, max - min) + min;
	}

	template<typename K>
	void WrapInplace(K& x, K min, K max) {
		x = Wrap(x, min, max);
	}

	//	/** Computes min <= a <= max such that x = n*(max-min) + a, n \in Integers
	//	 * If min > max the return value is min!
	//	 */
	//	template<typename K>
	//	K wrapped(K x, K min, K max) {
	//		if(min >= max) {
	//			return min;
	//		}
	//		K d = max - min;
	//		while(x > max) {
	//			x -= d;
	//		}
	//		while(x < min) {
	//			x += d;
	//		}
	//		return x;
	//	}
	//
	//	template<typename K>
	//	void wrap(K& x, K min, K max) {
	//		x = wrapped(x, min, max);
	//	}
	//
	//	template<typename K>
	//	void wrap(K& x, K delta) {
	//		if(delta <= 0) {
	//			return;
	//		}
	//		while(x < 0) {
	//			x += delta;
	//		}
	//		while(x >= delta) {
	//			x -= delta;
	//		}
	//	}

	//---------------------------------------------------------------------------

	/** Machine epsilon gives an upper bound on the relative error due to rounding in floating point arithmetic.
	 * See http://en.wikipedia.org/wiki/Machine_epsilon
	 */
	template<typename K>
	K Epsilon();

	template<> inline
	float Epsilon<float>() {
		return 5.96e-05f; // machine epsilon: 5.96e-08;
	}

	template<> inline
	double Epsilon<double>() {
		return 1.11e-12; // machine epsilon: 1.11e-16;
	}

	template<> inline
	long double Epsilon<long double>() {
		return 9.63e-30; // machine epsilon: 9.63e-35;
	}

	template<typename K>
	bool IsEqual(K a, K b) {
		return a == b;
	}

	template<typename K>
	bool IsEqualEpsilon(K a, K b, K epsilon=Epsilon<K>()) {
		return std::abs(a - b) < epsilon;
	}

	template<> inline
	bool IsEqual<float>(float a, float b) {
		return IsEqualEpsilon(a, b);
	}

	template<> inline
	bool IsEqual<double>(double a, double b) {
		return IsEqualEpsilon(a, b);
	}

	template<> inline
	bool IsEqual<long double>(long double a, long double b) {
		return IsEqualEpsilon(a, b);
	}

	template<typename K>
	bool IsZero(K a) {
		return IsEqual(a, K(0));
	}

	//---------------------------------------------------------------------------

	inline
	float Degree2Radians(float a) {
		return a * float(C_DEG_TO_RAD);
	}

	inline
	double Degree2Radians(double a) {
		return a * double(C_DEG_TO_RAD);
	}

	inline
	long double Degree2Radians(long double a) {
		return a * C_DEG_TO_RAD;
	}

	inline
	float Radians2Degree(float a) {
		return a * float(C_RAD_TO_DEG);
	}

	inline
	double Radians2Degree(double a) {
		return a * double(C_RAD_TO_DEG);
	}

	inline
	long double Radians2Degree(long double a) {
		return a * C_RAD_TO_DEG;
	}

	//---------------------------------------------------------------------------

	/** Return smallest of a and b */
	template<typename K>
	K Min(const K& a, const K& b) {
		return (b < a) ? b : a;
	}

	/** Return largest of a and b */
	template<typename K>
	K Max(const K& a, const K& b) {
		return (b > a) ? b : a;
	}

	/** Return smallest of a, b and c */
	template<typename K>
	K Min(const K& a, const K& b, const K& c) {
		return Min(Min(a,b), c);
	}

	/** Return largest of a, b and c */
	template<typename K>
	K Max(const K& a, const K& b, const K& c) {
		return Max(Max(a,b), c);
	}

	/** Return smallest of a, b, c and d */
	template<typename K>
	K Min(const K& a, const K& b, const K& c, const K& d) {
		return Min(Min(a,b), Min(c,d));
	}

	/** Return largest of a, b, c and d */
	template<typename K>
	K Max(const K& a, const K& b, const K& c, const K& d) {
		return Max(Max(a,b), Max(c,d));
	}

	//---------------------------------------------------------------------------

	/** Computes angle in a triangle
	 * The triangle is given by the points A, B, C and
	 * the result gives the angle between the sides AB and AC.
	 */
	template<typename K, typename T>
	void TriangleCosineRule(T a1, T a2, T b1, T b2, T c1, T c2, K& result) {
		// TODO is it save to compute DotDiff in T?
		T l1_sqr = DotDiff(a1, a2, b1, b2);
		T l2_sqr = DotDiff(a1, a2, c1, c2);
		T lo_sqr = DotDiff(b1, b2, c1, c2);
		if(l1_sqr == 0 || l2_sqr == 0) {
			result = K(0);
		} else {
			// (b*b + c*c - a*a) / (2*b*c)
			result = std::acos(K(l1_sqr + l2_sqr - lo_sqr) / std::sqrt(K(4 * l1_sqr) * K(l2_sqr))); // do not multipy in T
		}
	}

	template<typename T>
	bool Clockwise(T a1, T a2, T b1, T b2, T c1, T c2) {
		return (b1 - a1) * (c2 - b2) < (b2 - a2) * (c1 - b1);
	}

	//---------------------------------------------------------------------------

	/** AccMin(a, x) is equal to a = min(a, x) */
	template<typename K>
	void AccMin(K& accumulated, const K& a) {
		if(a < accumulated) {
			accumulated = a;
		}
	}

	/** Computes accumulated = min(accumulated, a) */
	template<typename K>
	void ifmin(K& accumulated, const K& a) {
		if(a < accumulated) {
			accumulated = a;
		}
	}

	/** Computes accumulated = max(accumulated, a) */
	template<typename K>
	void ifmax(K& accumulated, const K& a) {
		if(a > accumulated) {
			accumulated = a;
		}
	}

	/** Like ifmin but returns true if a replacement took place */
	template<typename K>
	bool ifminR(K& accumulated, const K& a) {
		if(a < accumulated) {
			accumulated = a;
			return true;
		} else {
			return false;
		}
	}

	/** Like ifmax but returns true if a replacement took place */
	template<typename K>
	bool ifmaxR(K& accumulated, const K& a) {
		if(a > accumulated) {
			accumulated = a;
			return true;
		} else {
			return false;
		}
	}

	//---------------------------------------------------------------------------

	/** Sort u and v such that u < v */
	template<typename K>
	void order(K& u, K& v) {
		if(v < u) {
			std::swap(u, v);
		}
	}

	/** Sort u, v and w such that u < v < w */
	template<typename K>
	void order(K& u, K& v, K& w) {
		order(u, v);
		order(v, w);
		order(u, v);
	}

	/** Sort u, v, w and x such that u < v < w < x */
	template<typename K>
	void order(K& u, K& v, K& w, K& x) {
		order(u, v);
		order(v, w);
		order(w, x);
		order(u, v, w);
	}

	/** This functions if necessary swaps a and b s.t. a <= b */
	template<typename K>
	void orderAscending(K& a, K& b) {
		if(a > b) {
			std::swap(a, b);
		}
	}

	/** This functions if necessary swaps a, b and c s.t. a <= b <= c */
	template<typename K>
	void orderAscending(K& a, K& b, K& c) {
		orderAscending(a, b);
		orderAscending(b, c);
		orderAscending(a, b);
	}

	/** This functions if necessary swaps a and b s.t. a >= b */
	template<typename K>
	void orderDescending(K& a, K& b) {
		if(a < b) {
			std::swap(a, b);
		}
	}

	/** This functions if necessary swaps a, b and c s.t. a >= b >= c */
	template<typename K>
	void orderDescending(K& a, K& b, K& c) {
		orderDescending(a, b);
		orderDescending(b, c);
		orderDescending(a, b);
	}

	//---------------------------------------------------------------------------

	inline
	uint PowerOfTwo(uint i) {
		return 1 << i;
	}

	/** Computes 2^i for i >= 0 */
	inline
	uint P2Get(uint i) {
		return 1 << i;
	}

	/** Computes a \in Reals s.t. 2^a = n for n \in |N */
	inline
	double P2Log(uint n) {
		const double log_2_inv = 1.0 / log(2.0);
		return std::log((double)n) * log_2_inv;
	}

	/** Computes i \in |N s.t. 2^(i-1) < n <= 2^i */
	inline
	uint P2LogCeil(uint n) {
		return (int)std::ceil(P2Log(n));
	}

	/** Computes i \in |N s.t. 2^i <= n < 2^(i+1) */
	inline
	uint P2LogFloor(uint n) {
		return (int)std::floor(P2Log(n));
	}

	/** Computes p = 2^i with i \in |N s.t. 2^(i-1) < n <= 2^i
	 * Same as P2Get(P2LogCeil(n))
	 */
	inline
	uint P2Ceil(uint n) {
		return P2Get(P2LogCeil(n));
	}

	/** Computes p = 2^i with i \in |N s.t. 2^i <= n < 2^(i+1)
	 * Same as P2Get(P2LogFloor(n))
	 */
	inline
	uint P2Floor(uint n) {
		return P2Get(P2LogFloor(n));
	}

	/** Returns true if a is a power of two.
	 * That is if there is i \in |N s.t. x = 2^i
	 * An alternative on-liner is:
	 * 		return ((x != 0) && ((x & (~x + 1)) == x));
	 */
	inline
	bool P2Test(uint x) {
		return (x != 0) && !(x & (x - 1));
	}

	/** Same as P2Test */
	inline
	bool IsPowerOfTwo(uint x) {
		return (x != 0) && !(x & (x - 1));
	}

	/** Same as P2Test */
	inline
	bool IsPowerOfTwo(int x) {
		return (x > 0) && !(x & (x - 1));
	}

	/** Returns the smallest a s.t. 2^a >= x */
	inline
	uint PowerOfTwoExponent(uint x) {
		uint i=0;
		uint a=1;
		while(a < x) {
			i++;
			a <<= 1;
		}
		return i;
	}

	//---------------------------------------------------------------------------

}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
