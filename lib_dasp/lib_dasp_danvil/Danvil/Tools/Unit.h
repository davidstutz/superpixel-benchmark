/*
 * TemplateConstants.h
 *
 *  Created on: 30.07.2009
 *      Author: david
 */

#ifndef DANVIL_TOOLS_UNIT_H_
#define DANVIL_TOOLS_UNIT_H_
//---------------------------------------------------------------------------
#include <Danvil/Types.h>
#include <cassert>
#include <stdint.h>
//---------------------------------------------------------------------------
namespace Danvil {
//---------------------------------------------------------------------------

namespace MathUnit
{
	inline
	unsigned char MultU8(unsigned char base, unsigned char val) {
		// assumes sizeof(unsigned int) > sizeof(unsigned char)
		return (unsigned char)(((unsigned int)base * (unsigned int)val) >> 8);
	}

	inline
	void MultU8Inplace(unsigned char& base, unsigned char val) {
		base = MultU8(base, val);
	}

	namespace Private
	{
		/** Get type of unsigned integer from number of bits */
		template<int BITS> struct IntegerType;
		template<> struct IntegerType<8> { typedef uint8_t result; };
		template<> struct IntegerType<16> { typedef uint16_t result; };
		template<> struct IntegerType<32> { typedef uint32_t result; };
		template<> struct IntegerType<64> { typedef uint64_t result; };

		template<typename F>
		struct UnitFloatingPoint;

		/** integer unit */
		template<int BITS>
		struct UnitInteger
		{
			typedef typename IntegerType<BITS>::result I;
			UnitInteger() : value_(zero()) {}
			UnitInteger(I v) : value_(v) {}
			UnitInteger(const UnitInteger<BITS>& u) : value_(u.value_) {}
			template<int M> UnitInteger(const UnitInteger<M>& u);
			template<typename F> UnitInteger(const UnitFloatingPoint<F>& u);
			I value() const { return value_; }
			//template<int M> operator IntegerType<BITS>::result() const;
			//template<typename F> operator F() const;
			operator I() { return value_; }
			static I zero() { return I(0); }
			static I half() { return one() >> 1; }
			static I one() { return static_cast<I>(-1); }
			static int bits() { return BITS; }
		private:
			I value_;
		};

		/** floating point unit */
		template<typename F>
		struct UnitFloatingPoint
		{
			UnitFloatingPoint() : value_(zero()) {}
			UnitFloatingPoint(F v) : value_(v<zero()?zero():(v>one()?one():v)) {} // must clamp here!
			UnitFloatingPoint(const UnitFloatingPoint<F>& u) : value_(u.value_) {}
			template<int N> UnitFloatingPoint(const UnitInteger<N>& u);
			template<typename G> UnitFloatingPoint(const UnitFloatingPoint<G>& u);
			F value() const { return value_; }
			//template<int N> operator I() const;
			//template<typename G> operator G() const;
			operator F() { return value_; }
			static F zero() { return F(0.0); }
			static F half() { return F(0.5); }
			static F one() { return F(1.0); }
		private:
			F value_;
		};

		template<int N> template<int M>
		UnitInteger<N>::UnitInteger(const UnitInteger<M>& u)
		: value_((u.value()) << (UnitInteger<N>::bits() - UnitInteger<M>::bits())) {}

		template<int N> template<typename F>
		UnitInteger<N>::UnitInteger(const UnitFloatingPoint<F>& u)
		: value_((I)(u.value() * (F)UnitInteger<N>::one())) {}

		template<typename F> template<int N>
		UnitFloatingPoint<F>::UnitFloatingPoint(const UnitInteger<N>& u)
		: value_(((F)u.value()) / ((F)UnitInteger<N>::one())) {}

		template<typename F> template<typename G>
		UnitFloatingPoint<F>::UnitFloatingPoint(const UnitFloatingPoint<G>& u)
		: value_((G)u.value()) {}

		/*template<int N> template<int M>
		UnitInteger<N>::operator IntegerType<M>::result() const {
			return UnitInteger<M>(*this).value();
		}

		template<int N> template<typename F>
		UnitInteger<N>::operator F() const {
			return UnitFloatingPoint<F>(*this).value();
		}

		template<typename F> template<int N>
		UnitFloatingPoint<F>::operator IntegerType<N>::result() const {
			return UnitInteger<N>(*this).value();
		}

		template<typename F> template<typename G>
		UnitFloatingPoint<F>::operator G() const {
			return UnitFloatingPoint<G>(*this).value();
		}*/

	}

	/** Define Unit types for 8, 16, 32 and 64 unsigned integer types and for float and double */

	template<typename K> struct U;

	#define DEFINE_UNIT_INT(N) \
		template<> struct U<Private::IntegerType<N>::result> : public Private::UnitInteger<N> {\
			typedef Private::UnitInteger<N> BaseT; \
			U() {} \
			template<class X> U(const X& x) : BaseT(x) {} \
		}; \
		typedef U<Private::IntegerType<N>::result> Ui##N;
	DEFINE_UNIT_INT(8)
	DEFINE_UNIT_INT(16)
	DEFINE_UNIT_INT(32)
	DEFINE_UNIT_INT(64)
	#undef DEFINE_UNIT_INT

	#define DEFINE_UNIT_FLOAT(F) template<> struct U<F> : public Private::UnitFloatingPoint<F> {\
			typedef Private::UnitFloatingPoint<F> BaseT; \
			U() {} \
			template<class X> U(const X& x) : BaseT(x) {} \
		};
	DEFINE_UNIT_FLOAT(float)
	DEFINE_UNIT_FLOAT(double)
	#undef DEFINE_UNIT_FLOAT
	typedef U<float> Uf;
	typedef U<double> Uf64;

	namespace Private
	{
		template<int N, int M> struct Shifts;
		#define DEFINE_SHIFT(N,M,R,S1,S2) template<> struct Shifts<N,M> { static const int result=R; static const int shift1=S1; static const int shift2=S2; };
		DEFINE_SHIFT( 8, 8,16,  0,  0)
		DEFINE_SHIFT( 8,16,32,  8,  0)
		DEFINE_SHIFT( 8,32,64, 24,  0)
		DEFINE_SHIFT( 8,64,64,  0, -8)
		DEFINE_SHIFT(16, 8,32,  0,  8)
		DEFINE_SHIFT(16,16,32,  0,  0)
		DEFINE_SHIFT(16,32,64, 16,  0)
		DEFINE_SHIFT(16,64,64,  0,-16)
		DEFINE_SHIFT(32, 8,64,  0, 24)
		DEFINE_SHIFT(32,16,64,  0, 16)
		DEFINE_SHIFT(32,32,64,  0,  0)
		DEFINE_SHIFT(32,64,64,  0,-32)
		DEFINE_SHIFT(64, 8,64, -8,  0)
		DEFINE_SHIFT(64,16,64,-16,  0)
		DEFINE_SHIFT(64,32,64,-32,  0)
		DEFINE_SHIFT(64,64,64,-32,-32)
		#undef DEFINE_SHIFT

		template<typename F, typename G> struct FMerge;
		template<typename F> struct FMerge<F,F> { typedef F result; };
		template<> struct FMerge<float,double> { typedef double result; };
		template<> struct FMerge<double,float> { typedef double result; };
	}

} // end of namespace Unit
} // end of namespace Danvil

//------------------------------------------------------------------------------

template<int N, int M>
Danvil::MathUnit::Private::UnitInteger<Danvil::MathUnit::Private::Shifts<N,M>::result> operator*(const Danvil::MathUnit::Private::UnitInteger<N>& a, const Danvil::MathUnit::Private::UnitInteger<M>& b)
{
	using namespace Danvil::MathUnit::Private;
	const int bits = Shifts<N,M>::result;
	typedef typename UnitInteger<bits>::I I;
	I sa;
	int SHIFT1 = Shifts<N,M>::shift1;
	if(SHIFT1 < 0) {
		sa = (((I)a.value()) >> (-SHIFT1));
	} else {
		sa = (((I)a.value()) << (SHIFT1));
	}
	I sb;
	int SHIFT2 = Shifts<N,M>::shift2;
	if(SHIFT2 < 0) {
		sb = (((I)b.value()) >> (-SHIFT2));
	} else {
		sb = (((I)b.value()) << (SHIFT2));
	}
	return UnitInteger<bits>(sa * sb);
}

template<typename F, typename G>
Danvil::MathUnit::Private::UnitFloatingPoint<typename Danvil::MathUnit::Private::FMerge<F,G>::result> operator*(const Danvil::MathUnit::Private::UnitFloatingPoint<F>& a, const Danvil::MathUnit::Private::UnitFloatingPoint<G>& b)
{
	using namespace Danvil::MathUnit::Private;
	typedef typename Danvil::MathUnit::Private::FMerge<F,G>::result H;
	return UnitFloatingPoint<H>((H)a.value() * (H)b.value());
}
// TODO 64-bit integer should use double - not float!

template<typename F, int N>
Danvil::MathUnit::Private::UnitFloatingPoint<F> operator*(const Danvil::MathUnit::Private::UnitFloatingPoint<F>& a, const Danvil::MathUnit::Private::UnitInteger<N>& b) {
	using namespace Danvil::MathUnit::Private;
	return UnitFloatingPoint<F>(b.value() * UnitFloatingPoint<F>(b.value()).value());
}

template<typename F, int N>
Danvil::MathUnit::Private::UnitFloatingPoint<F> operator*(const Danvil::MathUnit::Private::UnitInteger<N>& a, const Danvil::MathUnit::Private::UnitFloatingPoint<F>& b) {
	using namespace Danvil::MathUnit::Private;
	return UnitFloatingPoint<F>(UnitFloatingPoint<F>(a.value()).value() * b.value());
}

//------------------------------------------------------------------------------

namespace Danvil {

namespace MathUnit
{
	template<int N>
	Private::UnitInteger<Private::Shifts<N,N>::result> Square(const Private::UnitInteger<N>& a) {
		return a * a;
	}

	template<typename F>
	Private::UnitFloatingPoint<F> Square(const Private::UnitFloatingPoint<F>& a) {
		return a * a;
	}
}

}

////---------------------------------------------------------------------------
//
//template<typename K>
//K zero() {
//	// this is ok for all integer types
//	return (K)0;
//}
//
//template<> inline
//float zero<float>() { return 0.0f; }
//
//template<> inline
//double zero<double>() { return 0.0; }
//
//template<typename K>
//K unit() {
//	// only specialized types are supported
//	// provoke a compile time error
//	assert(false);
//}
//
//template<> inline
//uchar unit<uchar>() {
//	// we don't use 256 because otherwise we can't represent the 1
//	return 255;
//}
//
//template<> inline
//float unit<float>() { return 1.0f; }
//
//template<> inline
//double unit<double>() { return 1.0; }
//
//inline
//int unitSquaredChar() { return 65025;/* = 255*255*/; }
//
////---------------------------------------------------------------------------
//
//template<typename K>
//K clampedUnit(K x) {
//	return clamped(x, zero<K>(), unit<K>());
//}
//
//template<typename K, typename L>
//K convertToUnit(L x) {
//	return (K)clamped<L>(x, (L)zero<K>(), (L)unit<K>());
//}
//
////---------------------------------------------------------------------------
//
///** Computes (a/255) * (b/255) * 255.
// * This interprets 8 bit unsigned integers (uchar) as representations of the
// * unit interval [0,1]. One has to multiply/divide by 255 (and not 256),
// * because 255 should map to one, to map the full interval [0,1] and not
// * the open interval [0,1[ without 1.
// * This is equal to (a * b)/255.
// */
//inline
//uchar Mult8(uchar a, uchar b) {
//	uint c = ((uint)a) * ((uint)b);
//	c = c / 255;
//	return (uchar)c;
//}
//
///** Same as Mult8 but in place */
//inline
//void Mult8(uchar* pa, uchar b) {
//	*pa = Mult8(*pa, b);
//}
//
//inline
//float Map8ToUnit(uchar a) {
//	static const float inv = 0.003921569f; //1.0f / 255.0f;
//	return ((float)a) * inv;
//}
//
//inline
//float Map16ToUnit(uint a) {
//	static const float inv = 0.000015379f; //1.0f / (255.0f*255.0f);
//	return ((float)a) * inv;
//}
//
//inline
//float MapUnitTo16f(float a_unit) {
//	static const float Scale = 255.0f * 255.0f;
//	return a_unit * Scale;
//}
//
//inline
//float MapUnitTo8f(float a_unit) {
//	static const float Scale = 255.0f;
//	return a_unit * Scale;
//}

//---------------------------------------------------------------------------
#endif
