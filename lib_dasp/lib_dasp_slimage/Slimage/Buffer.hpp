/*
 * Buffer.hpp
 *
 *  Created on: Mar 21, 2012
 *      Author: david
 */

#ifndef BUFFER_HPP_
#define BUFFER_HPP_
//----------------------------------------------------------------------------//
#include <stdint.h>
#include <boost/shared_array.hpp>
#include <boost/assert.hpp>
//----------------------------------------------------------------------------//
namespace slimage {
//----------------------------------------------------------------------------//

/** A buffer to a block of data
 * Memory can either be a managed shared pointer or an unmanaged pointer.
 */
template<typename K>
struct Buffer
{
	typedef K ElementType;

	Buffer()
	: size_(0), begin_(0) {
	}

	Buffer(std::size_t n)
	: size_(0) {
		resize(n);
	}

	Buffer(std::size_t size, K* begin)
	: size_(size), begin_(begin) {
	}

	Buffer(std::size_t size, const boost::shared_array<K>& data)
	: size_(size), data_(data), begin_(data.get()) {
	}

	std::size_t size() const {
		return size_;
	}

	K* begin() const {
		return begin_;
	}

	K* end() const {
		return begin_ + size_;
	}

	/** Resizes the image */
	void resize(size_t n) {
		if(size_ != n) {
			// TODO what if data_ is not null and size matches?
			size_ = n;
			if(n == 0) {
				data_.reset();
				begin_ = 0;
			}
			else {
				data_.reset(new K[size_]);
				begin_ = data_.get();
			}
		}
	}

	/** Sets all elements to 'v' */
	void fill(K v) const {
		std::fill(begin(), end(), v);
	}

	void scale(K v) const {
		for(K* p=begin(); p!=end(); p++) {
			*p *= v;
		}
	}

	/** Creates a sub buffer with same memory */
	Buffer sub(size_t pos, size_t n) const {
		BOOST_ASSERT(pos + n <= size_);
		Buffer x;
		x.size_ = n;
		x.begin_ = begin_ + pos;
		x.data_ = data_;
		return x;
	}

	/** Creates a sub buffer with same memory */
	Buffer subFromTo(size_t begin, size_t end) const {
		BOOST_ASSERT(end >= begin);
		return sub(begin, end - begin);
	}

	/** Creates a deep copy */
	Buffer clone() const {
		Buffer x(size_);
		std::copy(begin(), end(), x.begin());
		return x;
	}

	void copyFrom(const K* mem) {
		std::copy(mem, mem + size_, begin());
	}

	void copyFromInterleaved(const K* mem, unsigned int step) {
		for(unsigned int i=0; i<size_; i++) {
			begin_[i] = mem[step*i];
		}
	}

private:
	std::size_t size_;
	boost::shared_array<K> data_;
	K* begin_;
};

//----------------------------------------------------------------------------//

typedef Buffer<unsigned char> Buffer8;
typedef Buffer<uint16_t> Buffer16;
typedef Buffer<unsigned int> BufferUint;
typedef Buffer<float> BufferF;

//----------------------------------------------------------------------------//
}
//----------------------------------------------------------------------------//
#endif
