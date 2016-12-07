/*
 * Pixel.hpp
 *
 *  Created on: Mar 21, 2012
 *      Author: david
 */

#ifndef SLIMAGE_PIXEL_HPP_
#define SLIMAGE_PIXEL_HPP_
//----------------------------------------------------------------------------//
#include <boost/assert.hpp>
//----------------------------------------------------------------------------//
namespace slimage {
//----------------------------------------------------------------------------//

typedef unsigned int index_t;

struct point_t
{
	index_t x, y;

	bool operator==(const point_t& p) const {
		return p.x == x && p.y == y;
	}

	bool operator!=(const point_t& p) const {
		return p.x != x || p.y != y;
	}

};

//----------------------------------------------------------------------------//

template<typename K_, unsigned int CC_>
struct Traits
{
	typedef K_ element_t;
	typedef K_* pointer_t;
	typedef K_& reference_t;
	typedef K_ K;
	static constexpr unsigned int CC = CC_;
};

//----------------------------------------------------------------------------//

//namespace detail
//{
//	template<typename K, unsigned int N>
//	struct PixelStorage
//	{
//		K values[N];
//
//		const K operator[](unsigned int i) const {
//			BOOST_ASSERT(i < N);
//			return values[i];
//		}
//
//		K& operator[](unsigned int i) {
//			BOOST_ASSERT(i < N);
//			return values[i];
//		}
//
//	};
//
//	template<typename K>
//	struct PixelStorage<K,1>
//	{
//		K value;
//
//		operator const K() {
//			return value;
//		}
//
//		const K operator[](unsigned int i) const {
//			BOOST_ASSERT(i == 0);
//			return value;
//		}
//
//		K& operator[](unsigned int i) {
//			BOOST_ASSERT(i == 0);
//			return value;
//		}
//
//	};
//
//	template<typename K, unsigned int N>
//	struct PixelPointer
//	{
//		K* p_;
//
//		const K operator[](unsigned int i) const {
//			BOOST_ASSERT(i < N);
//			return p_[i];
//		}
//
//		K& operator[](unsigned int i) {
//			BOOST_ASSERT(i < N);
//			return p_[i];
//		}
//
//	};
//
//	template<typename K>
//	struct PixelPointer<K,1>
//	{
//		K* p_;
//
//		operator const K() {
//			return *p_;
//		}
//
//		const K operator[](unsigned int i) const {
//			BOOST_ASSERT(i == 0);
//			return *p_;
//		}
//
//		K& operator[](unsigned int i) {
//			BOOST_ASSERT(i == 0);
//			return *p_;
//		}
//
//	};
//
//	template<typename K, unsigned int N>
//	struct StorageSelector {
//		typedef PixelStorage<K,N> result_t;
//	};
//
//	template<typename K, unsigned int N>
//	struct StorageSelector<K&,N> {
//		typedef PixelPointer<K,N> result_t;
//	};
//
//
//}

template<typename T>
struct Pixel
{
	typedef Pixel<T> SelfType;

	typedef typename T::element_t K;
	static constexpr unsigned int CC = T::CC;

	K values[CC];

	std::size_t size() const {
		return CC;
	}

	const K operator[](unsigned int i) const {
		BOOST_ASSERT(i < CC);
		return values[i];
	}

	K& operator[](unsigned int i) {
		BOOST_ASSERT(i < CC);
		return values[i];
	}

	SelfType& operator+=(const SelfType& x) {
		for(unsigned int i=0; i<CC; i++) {
			values[i] += x[i];
		}
		return *this;
	}

	friend SelfType operator+(const SelfType& x, const SelfType& y) {
		SelfType u = x;
		u += y;
		return u;
	}

};

// FIXME implement
template<typename K> struct Pixel<Traits<K,0>>;

template<typename K>
struct Pixel<Traits<K,1>>
{
	K value;

	std::size_t size() const {
		return 1;
	}

	operator K() const {
		return value;
	}

};

//----------------------------------------------------------------------------//

template<typename T>
struct PixelAccess
{
	typedef typename T::element_t K;
	enum { CC = T::CC };

	K* p;

	PixelAccess(K* pp) : p(pp) {}

	PixelAccess(const PixelAccess&) = delete;

	PixelAccess& operator=(const PixelAccess&) = delete;

	/** This function is dangerous!! */
	K* pointer() const {
		return p;
	}

	operator Pixel<T>() const {
		Pixel<T> px;
		for(unsigned int i=0; i<CC; i++) {
			px[i] = p[i];
		}
		return px;
	}

	K& operator[](unsigned int i) const {
		return p[i];
	}

	template<unsigned int N>
	struct OpAdd {
		PixelAccess summands[N];
		const PixelAccess& operator[](unsigned int i) const {
			return summands[i];
		}
		PixelAccess& operator[](unsigned int i) {
			return summands[i];
		}
		operator K() const {
			K sum = summands[0];
			for(unsigned int k=1; k<N; k++) {
				sum += summands[k];
			}
			return sum;
		}
		void increment(const PixelAccess& target) {
			for(unsigned int i=0; i<CC; i++) {
				for(unsigned int k=0; k<N; k++) {
					target[i] += summands[k][i];
				}
			}
		}
		void setTo(const PixelAccess& target) {
			for(unsigned int i=0; i<CC; i++) {
				target[i] = summands[0][i];
				for(unsigned int k=1; k<N; k++) {
					target[i] += summands[k][i];
				}
			}
		}
	};

	struct OpScale {
		K s;
		PixelAccess x;
		void setTo(const PixelAccess& target) {
			for(unsigned int i=0; i<CC; i++) {
				target[i] = s * x[i];
			}
		}
	};

	PixelAccess& operator=(const Pixel<T>& v) {
		for(unsigned int i=0; i<CC; i++) {
			p[i] = v[i];
		}
		return *this;
	}

	PixelAccess& operator=(const OpScale& v) {
		v.setTo(*this);
		return *this;
	}

	template<unsigned int N>
	PixelAccess& operator=(const OpAdd<N>& v) {
		v.setTo(*this);
		return *this;
	}

	PixelAccess& operator+=(const PixelAccess& x) {
		for(unsigned int i=0; i<CC; i++) {
			p[i] += x.p[i];
		}
		return *this;
	}

	PixelAccess& operator-=(const PixelAccess& x) {
		for(unsigned int i=0; i<CC; i++) {
			p[i] -= x.p[i];
		}
		return *this;
	}

	template<unsigned int N>
	PixelAccess& operator+=(const OpAdd<N>& x) {
		x.increment(*this);
		return *this;
	}

	friend OpScale operator*(K s, const PixelAccess& x) {
		return OpScale{s, x};
	}

	friend OpAdd<2> operator+(const PixelAccess& x, const PixelAccess& y) {
		return OpAdd<2>{{x, y}};
	}

	template<unsigned int N>
	friend OpAdd<N+1> operator+(const PixelAccess& x, const OpAdd<N>& y) {
		OpAdd<N+1> q;
		q[0] = x;
		for(unsigned int i=0; i<N; i++) {
			q[i+1] = y[i];
		}
		return q;
	}

	template<unsigned int N>
	friend OpAdd<N+1> operator+(const OpAdd<N>& y, const PixelAccess& x) {
		OpAdd<N+1> q;
		for(unsigned int i=0; i<N; i++) {
			q[i] = y[i];
		}
		q[N] = x;
		return q;
	}

};

template<typename K>
struct PixelAccess<Traits<K,1>>
{
	K* p;

	PixelAccess(K* pp) : p(pp) {}

	PixelAccess(const PixelAccess&) = delete;

	PixelAccess& operator=(const PixelAccess&) = delete;

	/** This function is dangerous!! */
	K* pointer() const {
		return p;
	}

	operator const K() const {
		return *p;
	}

	operator Pixel<Traits<K,1>>() const {
		return { *p };
	}

	K& operator[](unsigned int) const {
		return *p;
	}

	K& operator*() const {
		return *p;
	}

	const PixelAccess& operator=(K x) const {
		*p = x;
		return *this;
	}

	const PixelAccess& operator=(const Pixel<Traits<K,1>>& v) const {
		*p = v;
		return *this;
	}

	const PixelAccess& operator*=(K x) const {
		*p *= x;
		return *this;
	}

	const PixelAccess& operator+=(K x) const {
		*p += x;
		return *this;
	}

	const PixelAccess& operator-=(K x) const {
		*p -= x;
		return *this;
	}

	const PixelAccess& operator+=(const PixelAccess& v) const {
		*p += static_cast<K>(v);
		return *this;
	}

	const PixelAccess& operator-=(const PixelAccess& v) const {
		*p -= static_cast<K>(v);
		return *this;
	}

	friend K operator*(K s, const PixelAccess& v) {
		return s * static_cast<K>(v);
	}

	friend K operator+(const PixelAccess& u, const PixelAccess& v) {
		return static_cast<K>(u) + static_cast<K>(v);
	}

	friend K operator*(const PixelAccess& u, const PixelAccess& v) {
		return static_cast<K>(u) * static_cast<K>(v);
	}

};

// FIXME
template<typename K> struct PixelAccess<Traits<K,0>>;

//----------------------------------------------------------------------------//
}
//----------------------------------------------------------------------------//
#endif
