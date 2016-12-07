/*
 * Color.h
 *
 *  Created on: 12.02.2009
 *     Changed: 15.12.2009
 *      Author: david
 */

#ifndef DANVIL_COLOR_COLOR_H_
#define DANVIL_COLOR_COLOR_H_
#ifndef DANVIL_COLOR_H_
	#error "Do not include this file directly. Use Danvil/Color.h instead!"
#endif
//---------------------------------------------------------------------------
#include <Danvil/Types.h>
#include <cassert>
#include <vector>
//---------------------------------------------------------------------------
namespace Danvil {
//---------------------------------------------------------------------------

class Color
{
public:
	/** Color names */
	enum EColorName {
		LightRed, Red, DarkRed,
		LightYellow, Yellow, DarkYellow,
		LightGreen, Green, DarkGreen,
		LightCyan, Cyan, DarkCyan,
		LightBlue, Blue, DarkBlue,
		LightMagenta, Magenta, DarkMagenta,
		Orange, Brown,
		White, LightGrey, Grey, DarkGrey, Black,
		Transparent
	};

protected:
	Color() {}
	virtual ~Color() {}
};

/** Represents a 4 channel color value of arbitrary type */
template<typename K>
class TColor
:	public Color
{
public:
	/** Color type arithmetic for the choosen type */
	typedef ColorValueArithmetics<K> Math;

public:
	/** Default constructor */
	TColor() {}

	/** Constructor taking RGB values and using maximal alpha */
	TColor(K _r, K _g, K _b) : r(_r), g(_g), b(_b), a(Math::Max()) {}

	/** Constructor taking RGBA values */
	TColor(K _r, K _g, K _b, K _a) : r(_r), g(_g), b(_b), a(_a) {}

	/** (Default) Copy constructor */
	TColor(const TColor& c) : r(c.r), g(c.g), b(c.b), a(c.a) {}

	const TColor& operator=(const TColor& c) { r=c.r; g=c.g; b=c.b; a=c.a; return *this; }

	/** Default destructor */
	virtual ~TColor() {}

public:
	/** Sets RGB values and does not change alpha value */
	void set(K _r, K _g, K _b) {
		r=_r; g=_g; b=_b;
	}

	/** Sets RGBA values */
	void set(K _r, K _g, K _b, K _a) {
		r=_r; g=_g; b=_b; a=_a;
	}

	void writeRgb(K* p) const {
		p[0] = r; p[1] = g; p[2] = b;
	}

	void writeRgba(K* p) const {
		p[0] = r; p[1] = g; p[2] = b; p[3] = a;
	}

	void readRgb(const K* p) {
		r = p[0]; g = p[1]; b = p[2];
	}

	void readRgba(const K* p) {
		r = p[0]; g = p[1]; b = p[2]; a = p[3];
	}

	uint32_t toRgba() const {
		// use 0xAABBGGRR (used e.g. by OpenGL GL_RGBA)
		return (a << 24) + (b << 16) + (g << 8) + r;
	}

	uint32_t toBgra() const {
		// use 0xAARRGGBB (used e.g. by Qt)
		return (a << 24) + (r << 16) + (g << 8) + b;
	}

public:
	/** Number of bytes needed to save the color value in memory */
	static uint MemorySizeBytes() {
		return sizeof(K)*4;
	}

public:
	/** Creates a greyscale color vaöue with intensity v */
	static TColor FactorGray(K v) {
		return TColor(v, v, v, Math::Max());
	}

	/** Creates a color with given RGB value and maxima alpha value */
	static TColor FactorRgb(K r, K g, K b) {
		return TColor(r, g, b, Math::Max());
	}

	/** Creates a color with given RGBA value */
	static TColor FactorRgba(K r, K g, K b, K a) {
		return TColor(r, g, b, a);
	}

	/** Creates white color with given alpha value */
	static TColor FactorTransparency(K a) {
		return TColor(Math::Max(), Math::Max(), Math::Max(), a);
	}

	/** Creates a color reading RGB values from the given pointer and using maximal alpha */
	static TColor FactorRgb(const K* p) {
		return TColor(p[0], p[1], p[2], Math::Max());
	}

	/** Creates a color reading RGBA values from the given pointer */
	static TColor FactorRgba(const K* p) {
		return TColor(p[0], p[1], p[2], p[3]);
	}

	/** Constructor taking RGBA values */
	static TColor FactorChangeAlpha(const TColor& c_rgb, K a) {
		return TColor(c_rgb.r, c_rgb.g, c_rgb.b, a);
	}

	static TColor Factor(const K* data, uint channels) {
		switch(channels) {
		case 1:
			return FactorGray(data[0]);
		case 3:
			return FactorRgb(data[0], data[1], data[2]);
		case 4:
			return FactorRgba(data[0], data[1], data[2], data[3]);
		default:
			return FactorGray(0);
			//throw std::runtime_error("Invalid number of channels");
		}
	}

	/** Converts a color to a different color value type */
	template<typename L>
	static TColor<K> Convert(const TColor<L>& c) {
		TColor<K> x;
		ColorValueConversion(c.r, x.r);
		ColorValueConversion(c.g, x.g);
		ColorValueConversion(c.b, x.b);
		ColorValueConversion(c.a, x.a);
		return x;
	}

	/** Converts RGB values to a different color value type (using maximal alpha) */
	template<typename L>
	static TColor<K> Convert(L r, L g, L b) {
		TColor<K> x;
		ColorValueConversion(r, x.r);
		ColorValueConversion(g, x.g);
		ColorValueConversion(b, x.b);
		x.a = Math::Max();
		return x;
	}

	/** Converts RGBA values to a different color value type */
	template<typename L>
	static TColor<K> Convert(L r, L g, L b, L a) {
		TColor<K> x;
		ColorValueConversion(r, x.r);
		ColorValueConversion(g, x.g);
		ColorValueConversion(b, x.b);
		ColorValueConversion(a, x.a);
		return x;
	}

public:
	/** Constructs a color using a color name */
	TColor(EColorName t) {
		a = Math::Max();
		switch(t) {
		case LightRed:		set(Math::Max(), Math::Half(), Math::Half()); break;
		case Red:			set(Math::Max(), Math::Min(), Math::Min()); break;
		case DarkRed:		set(Math::Half(), Math::Min(), Math::Min()); break;
		case LightYellow:	set(Math::Max(), Math::Max(), Math::Half()); break;
		case Yellow:		set(Math::Max(), Math::Max(), Math::Min()); break;
		case DarkYellow:	set(Math::Half(), Math::Half(), Math::Min()); break;
		case LightGreen:	set(Math::Half(), Math::Max(), Math::Half()); break;
		case Green:			set(Math::Min(), Math::Max(), Math::Min()); break;
		case DarkGreen:		set(Math::Min(), Math::Half(), Math::Min()); break;
		case LightCyan:		set(Math::Half(), Math::Max(), Math::Max()); break;
		case Cyan:			set(Math::Min(), Math::Max(), Math::Max()); break;
		case DarkCyan:		set(Math::Min(), Math::Half(), Math::Half()); break;
		case LightBlue:		set(Math::Half(), Math::Half(), Math::Max()); break;
		case Blue:			set(Math::Min(), Math::Min(), Math::Max()); break;
		case DarkBlue:		set(Math::Min(), Math::Min(), Math::Half()); break;
		case LightMagenta:	set(Math::Max(), Math::Half(), Math::Max()); break;
		case Magenta:		set(Math::Max(), Math::Min(), Math::Max()); break;
		case DarkMagenta:	set(Math::Half(), Math::Min(), Math::Half()); break;
		case Orange:		set(Math::Max(), Math::Half(), Math::Min()); break;
		case Brown:			set(Math::Max()*0.6, Math::Half(), Math::Max()*0.1); break;
		case White:			set(Math::Max(), Math::Max(), Math::Max()); break;
		case LightGrey:		set(0.75*Math::Max(), 0.75*Math::Max(), 0.75*Math::Max()); break;
		case Grey:			set(Math::Half(), Math::Half(), Math::Half()); break;
		case DarkGrey:		set(0.25*Math::Max(), 0.25*Math::Max(), 0.25*Math::Max()); break;
		case Black:			set(Math::Min(),  Math::Min(),  Math::Min()); break;
		case Transparent:	set(Math::Max(), Math::Max(), Math::Max(), Math::Min()); break;
		};
	}

public:
	TColor& operator*=(const TColor& p) {
		r = Math::Mult(r, p.r);
		g = Math::Mult(g, p.g);
		b = Math::Mult(b, p.b);
		a = Math::Mult(a, p.a);
		return *this;
	}

	TColor& operator*=(const K& s) {
		r = Math::Mult(r, s);
		g = Math::Mult(g, s);
		b = Math::Mult(b, s);
		a = Math::Mult(a, s);
		return *this;
	}

	TColor& operator+=(const TColor& p) {
		r = Math::Add(r, p.r);
		g = Math::Add(g, p.g);
		b = Math::Add(b, p.b);
		a = Math::Add(a, p.a);
		return *this;
	}

	TColor& operator-=(const TColor& p) {
		r = Math::Sub(r, p.r);
		g = Math::Sub(g, p.g);
		b = Math::Sub(b, p.b);
		a = Math::Sub(a, p.a);
		return *this;
	}

	static TColor Mix(const TColor& p, const TColor& q, float s) {
		TColor c;
		c.r = static_cast<K>((1 - s) * static_cast<float>(p.r) + s * static_cast<float>(q.r));
		c.g = static_cast<K>((1 - s) * static_cast<float>(p.g) + s * static_cast<float>(q.g));
		c.b = static_cast<K>((1 - s) * static_cast<float>(p.b) + s * static_cast<float>(q.b));
		c.a = static_cast<K>((1 - s) * static_cast<float>(p.a) + s * static_cast<float>(q.a));
		return c;
//		return (Math::Max() - s)*p + s*q;
	}

public:
	K r, g, b, a;
};

template<typename K>
bool operator==(const TColor<K>& p, const TColor<K>& q) {
	return p.r == q.r && p.g == q.g && p.b == q.b && p.a == q.a;
}

template<typename K>
bool operator!=(const TColor<K>& p, const TColor<K>& q) {
	return p.r != q.r || p.g != q.g || p.b != q.b || p.a != q.a;
}

template<typename K>
TColor<K> operator*(const TColor<K>& p, const TColor<K>& q) {
	TColor<K> u = p;
	u *= q;
	return u;
}

template<typename K>
TColor<K> operator*(const K& s, const TColor<K>& p) {
	TColor<K> u = p;
	u *= s;
	return u;
}

template<typename K>
TColor<K> operator+(const TColor<K>& p, const TColor<K>& q) {
	TColor<K> u = p;
	u += q;
	return u;
}

template<typename K>
TColor<K> operator-(const TColor<K>& p, const TColor<K>& q) {
	TColor<K> u = p;
	u -= q;
	return u;
}

typedef TColor<float> Colorf;
typedef TColor<float> ColorF;
typedef TColor<uchar> Colorub;
typedef TColor<uchar> ColorUB;

//---------------------------------------------------------------------------

namespace ColorTools
{
	inline void Mix(
			uchar ar, uchar ag, uchar ab, uchar aa,
			uchar br, uchar bg, uchar bb, uchar ba,
			uchar& cr, uchar& cg, uchar& cb, uchar& ca,
			unsigned char s
	) {
		unsigned int p = (unsigned int)s;
		unsigned char q = 255 - p;
		cr = (unsigned char)((((unsigned int)ar) * q + ((unsigned int)br) * p) >> 8);
		cg = (unsigned char)((((unsigned int)ag) * q + ((unsigned int)bg) * p) >> 8);
		cb = (unsigned char)((((unsigned int)ab) * q + ((unsigned int)bb) * p) >> 8);
		ca = (unsigned char)((((unsigned int)aa) * q + ((unsigned int)ba) * p) >> 8);
	}

	inline Colorub Mix(const Colorub& a, const Colorub& b, unsigned char s) {
		unsigned int p = (unsigned int)s;
		unsigned char q = 255 - p;
		unsigned int c1 = (((unsigned int)a.r) * q + ((unsigned int)b.r) * p) >> 8;
		unsigned int c2 = (((unsigned int)a.g) * q + ((unsigned int)b.g) * p) >> 8;
		unsigned int c3 = (((unsigned int)a.b) * q + ((unsigned int)b.b) * p) >> 8;
		unsigned int c4 = (((unsigned int)a.a) * q + ((unsigned int)b.a) * p) >> 8;
		return Colorub((unsigned char)c1, (unsigned char)c2, (unsigned char)c3, (unsigned char)c4);
	}

	inline Colorub Mix(const std::vector<Colorub>& colors) {
		unsigned int cnt = colors.size();
		if(cnt < 0x00FFFFFF) {
			unsigned int r, g, b, a;
			for(std::vector<Colorub>::const_iterator it=colors.begin(); it!=colors.end(); it++) {
				r += (unsigned int)it->r;
				g += (unsigned int)it->g;
				b += (unsigned int)it->b;
				a += (unsigned int)it->a;
			}
			r /= cnt;
			g /= cnt;
			b /= cnt;
			a /= cnt;
			typedef unsigned char K;
			return Colorub((K)r, (K)g, (K)b, (K)a);
		} else {
			// 256 * 0x00FFFFFF is more than a unsigned int can hold!
			float r, g, b, a;
			for(std::vector<Colorub>::const_iterator it=colors.begin(); it!=colors.end(); it++) {
				r += (float)it->r;
				g += (float)it->g;
				b += (float)it->b;
				a += (float)it->a;
			}
			float scl = 1.0f / (float)cnt;
			r *= scl;
			g *= scl;
			b *= scl;
			a *= scl;
			typedef unsigned char K;
			return Colorub(static_cast<K>(r), static_cast<K>(g), static_cast<K>(b), static_cast<K>(a));
		}
	}

	inline Colorf Mix(const std::vector<Colorf>& colors) {
		float r, g, b, a;
		for(std::vector<Colorf>::const_iterator it=colors.begin(); it!=colors.end(); it++) {
			r += it->r;
			g += it->g;
			b += it->b;
			a += it->a;
		}
		float scl = 1.0f / float(colors.size());
		r *= scl;
		g *= scl;
		b *= scl;
		a *= scl;
		return Colorf(r, g, b, a);
	}

};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
