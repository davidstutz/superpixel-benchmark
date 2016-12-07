/*
 * Iterator.hpp
 *
 *  Created on: Mar 22, 2012
 *      Author: david
 */

#ifndef ITERATOR_HPP_
#define ITERATOR_HPP_
//----------------------------------------------------------------------------//
#include "Pixel.hpp"
#include <boost/assert.hpp>
//----------------------------------------------------------------------------//
namespace slimage {
//----------------------------------------------------------------------------//

template<typename T>
struct Iterator
{
	typedef typename T::element_t K;
	enum { CC = T::CC };

	K* p_;

	/** This function is dangerous!! */
	K* pointer() const {
		return p_;
	}

	operator K*() const {
		return p_;
	}

	PixelAccess<T> operator*() const {
		return { p_ };
	}

	K& operator[](index_t i) const {
		BOOST_ASSERT(i < CC);
		return p_[i];
	}

	const Iterator& operator++() {
		static_assert(CC != 0, "CC = 0 currently not supported!");
		p_ += CC;
		return *this;
	}

	Iterator operator++(int) {
		static_assert(CC != 0, "CC = 0 currently not supported!");
		Iterator t = *this;
		++(*this);
		return t;
	}

	Iterator operator+(index_t i) const {
		static_assert(CC != 0, "CC = 0 currently not supported!");
		return { p_ + i * CC };
	}

	Iterator operator-(index_t i) const {
		static_assert(CC != 0, "CC = 0 currently not supported!");
		return { p_ - i * CC };
	}

	bool operator==(const Iterator& a) const {
		return p_ == a.p_;
	}

	bool operator!=(const Iterator& a) const {
		return p_ != a.p_;
	}

	bool operator<(const Iterator& a) const {
		return p_ < a.p_;
	}

	bool operator>(const Iterator& a) const {
		return p_ > a.p_;
	}

	friend int operator-(const Iterator& a, const Iterator& b) {
		return a.p_ - b.p_;
	}

};

//----------------------------------------------------------------------------//
}
//----------------------------------------------------------------------------//
#endif
