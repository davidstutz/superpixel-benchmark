/*
 * FunctionCache.h
 *
 *  Created on: Jun 1, 2011
 *      Author: david
 */

#ifndef DANVIL_FUNCTIONCACHE_H_
#define DANVIL_FUNCTIONCACHE_H_
//---------------------------------------------------------------------------
#include "MoreMath.h"
#include <cassert>
//---------------------------------------------------------------------------
namespace Danvil {
//---------------------------------------------------------------------------

namespace Private {
	template<typename K> struct SelectNumberByType;
	template<> struct SelectNumberByType<float> { static const size_t Result=256; };
	template<> struct SelectNumberByType<double> { static const size_t Result=1024; };
	template<> struct SelectNumberByType<long double> { static const size_t Result=4096; };

	/** Computed via -std::log(MoreMath::Epsilon<K>()) */
	template<typename U> struct ExpRangeByType;
	template<> struct ExpRangeByType<float> { static constexpr float Result = 9.7f; };
	template<> struct ExpRangeByType<double> { static constexpr double Result = 27.6; };
	template<> struct ExpRangeByType<long double> { static constexpr long double Result = 67.0; };

}

/** Caches a function K -> V where K is a floating point type
 * N: number of cached points
 * InterpolationOrder: method to compute function values
 *                     0: round down; 1: linear interpolation;
 */
template<typename V, unsigned int InterpolationOrder=0, size_t N=Private::SelectNumberByType<V>::Result, typename K=V>
struct FunctionCache
{
	FunctionCache()
	: a_(0), b_(0), scl_(0) {
		cache_[0] = V(0);
		// this will ensure, that the uninitialized function is 0
	}

	template<typename F>
	FunctionCache(K a, K b, F f)
	: a_(a), b_(b) {
		assert(a_ <= b_);
		scl_ = K(N - 1) / (b_ - a_);
		K u = (b_ - a_) / K(N - 1);
		K x = a_;
		for(size_t i=0; i<N; i++, x+=u) {
			cache_[i] = f(x);
		}
	}

	K operator()(K x) const {
		// will select at compile time!
		switch(InterpolationOrder) {
		case 0: {
			// round down
			int i = int(scl_ * (x - a_));
			return cache_[clamp(i)];
		}
		case 1: {
			// linear interpolation
			K u = scl_ * (x - a_);
			int i1 = int(u); // round down
			int i2 = i1 + 1;
			K p = u - float(i1);
			V y1 = cache_[clamp(i1)];
			V y2 = cache_[clamp(i2)];
			return (K(1) - p) * y1 + p * y2;
		}
		default: {
			// not implemented
			throw 0;
		}
		}
	}

private:
	static size_t clamp(int i) {
		return i < 0 ? 0 : (i >= int(N) ? N - 1 : size_t(i));
	}

private:
	K a_, b_, scl_;
	V cache_[N];
};

template<typename V, unsigned int Order, typename K> struct FunctionCache<V,Order,0,K>; // N must be >= 2!
template<typename V, unsigned int Order, typename K> struct FunctionCache<V,Order,1,K>; // N must be >= 2!

//---------------------------------------------------------------------------

template<typename K, unsigned int InterpolationOrder=0, size_t N=Private::SelectNumberByType<K>::Result>
struct ExpNegFunctionCache
: FunctionCache<K,InterpolationOrder,N,K>
{
private:
public:
	ExpNegFunctionCache()
	: FunctionCache<K,InterpolationOrder,N,K>(0, Private::ExpRangeByType<K>::Result, &evaluate) {
	}

private:
	static K evaluate(K x) {
		return std::exp(-x);
	}
};

//---------------------------------------------------------------------------

template<typename K, unsigned int InterpolationOrder=0, size_t N=Private::SelectNumberByType<K>::Result>
struct ProbabilityExpFunctionCache
: FunctionCache<K,InterpolationOrder,N,K>
{
private:
public:
	ProbabilityExpFunctionCache()
	: FunctionCache<K,InterpolationOrder,N,K>(0, K(2) * Private::ExpRangeByType<K>::Result, &evaluate) {
	}

private:
	static K evaluate(K x) {
		return std::exp(-K(0.5)*x);
	}
};

//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
