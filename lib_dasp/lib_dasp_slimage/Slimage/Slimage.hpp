/*
 * Slimage.hpp
 *
 *  Created on: Feb 3, 2012
 *      Author: david
 */

#ifndef SLIMAGE_HPP_
#define SLIMAGE_HPP_
//----------------------------------------------------------------------------//
#include "Buffer.hpp"
#include "Pixel.hpp"
#include "Iterator.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/assert.hpp>
#include <algorithm>
#include <string>
#include <typeinfo>
#include <stdexcept>
//----------------------------------------------------------------------------//
namespace slimage {
//----------------------------------------------------------------------------//

namespace detail
{
	template<unsigned int CC>
	struct PixelTraitsFixed
	{
		PixelTraitsFixed() {
		}

#ifdef NDEBUG
		PixelTraitsFixed(unsigned int) {}
#else
		PixelTraitsFixed(unsigned int cc) {
			BOOST_ASSERT(cc == CC);
		}
#endif

		unsigned int channelCount() const {
			return CC;
		}
	};

	struct PixelTraitsDynamic
	{
		PixelTraitsDynamic(unsigned int cc)
		: cc_(cc) {}

		unsigned int channelCount() const {
			return cc_;
		}

	private:
		unsigned int cc_;
	};

	template<unsigned int CC>
	struct PixelTraitsSelector {
		typedef PixelTraitsFixed<CC> Result;
	};

	template<>
	struct PixelTraitsSelector<0> {
		typedef PixelTraitsDynamic Result;
	};
}

//----------------------------------------------------------------------------//

template<typename T>
struct Image
: public detail::PixelTraitsSelector<T::CC>::Result
{
	typedef typename T::element_t ElementType;

	typedef typename T::element_t K;
	enum { CC = T::CC };

	typedef typename detail::PixelTraitsSelector<CC>::Result ChannelTraitsType;

	typedef Pixel<T> pixel_t;

	Image()
	: width_(0), height_(0)
	{
		allocate();
	}

	Image(index_t w, index_t h)
	: width_(w), height_(h)
	{
		allocate();
	}

	Image(point_t dim)
	: width_(dim.x), height_(dim.y)
	{
		allocate();
	}

	Image(index_t w, index_t h, index_t cc)
	: ChannelTraitsType(cc), width_(w), height_(h)
	{
		allocate();
	}

	Image(index_t w, index_t h, const Buffer<K>& buff)
	: width_(w), height_(h), buffer_(buff)
	{
		BOOST_ASSERT(buffer_.size() == width_ * height_ * this->channelCount());
	}

	Image(index_t w, index_t h, index_t cc, const Buffer<K>& buff)
	: ChannelTraitsType(cc), width_(w), height_(h), buffer_(buff)
	{
		BOOST_ASSERT(buffer_.size() == width_ * height_ * this->channelCount());
	}

	Image(point_t dim, const pixel_t& p)
	: width_(dim.x), height_(dim.y)
	{
		allocate();
		fill(p);
	}

	Image(index_t w, index_t h, const pixel_t& p)
	: width_(w), height_(h)
	{
		allocate();
		fill(p);
	}

	/** Width of the image */
	index_t width() const {
		return width_;
	}

	/** Height of the image */
	index_t height() const {
		return height_;
	}

	point_t dimensions() const {
		return point_t{width_, height_};
	}

	/** Number of pixels */
	index_t size() const {
		return width() * height();
	}

	template<typename S>
	bool hasSameSize(const Image<S>& other) const {
		return width() == other.width() && height() == other.height();
	}

	template<typename S>
	bool hasSameShape(const Image<S>& other) const {
		return hasSameSize(other) && this->channelCount() == other.channelCount();
	}

	/** Checks for null image
	 * The image is a null image if either width, height or channel count is 0.
	 * @return true if the image is a null image
	 */
	bool isNull() const {
		return size() == 0;
	}

	operator bool() const {
		return !isNull();
	}

	/** Returns true if (x,y,c) is a valid pixel/channel index */
	bool isValidIndex(int x, int y) const {
		return 0 <= x && x < static_cast<int>(width()) && 0 <= y && y < static_cast<int>(height());
	}

	/** Access to the pixel at position (x,y) */
	PixelAccess<T> operator()(index_t x, index_t y) const {
		return { pointer(x,y) };
	}

	/** Access to the i-th pixel */
	PixelAccess<T> operator[](index_t i) const {
		return { buffer_.begin() + i*CC };
	}

	Iterator<T> begin() const {
		return { buffer_.begin() };
	}

	Iterator<T> end() const {
		return { buffer_.end() };
	}

	/** The underlying buffer with the image data */
	Buffer<K>& buffer() {
		return buffer_;
	}

	/** The underlying buffer with the image data  */
	const Buffer<K>& buffer() const {
		return buffer_;
	}

	/** Resizes the image */
	void resize(index_t w, index_t h) {
		width_ = w;
		height_ = h;
		index_t n = width_ * height_ * this->channelCount();
		buffer_.resize(n);
	}

	void resize(const point_t& dims) {
		resize(dims.x, dims.y);
	}

	/** Creates a deep clone of this image */
	Image clone() const {
		return Image(width(), height(), this->channelCount(), this->buffer().clone());
	}

	/** Sets all pixels to the same value */
	void fill(const pixel_t& v) const {
		for(auto it=begin(); it!=end(); it++) {
			*it = v;
		}
	}

	/** Multiplies all pixel values with a scalar */
	template<typename K>
	void operator*=(const K& v) const {
		for(auto it=begin(); it!=end(); it++) {
			*it *= v;
		}
	}

	/** Creates a deep copied sub image */
	Image sub(unsigned int x, unsigned int y, unsigned int w, unsigned int h) const {
		unsigned int cc = this->channelCount();
		if( x + w > width() || y + h > height() ) {
			throw std::runtime_error("Wrong frame in sub image extraction");
		}
		Image si(w, h, cc);
		for(unsigned int i=0; i<h; i++) {
			const K* src = scanline(y+i) + x*cc;
			std::copy(src, src + w*cc, si.scanline(i));
		}
		return si;
	}

	/** Creates a deep copied flipped image */
	Image flipY() const {
		unsigned int cc = this->channelCount();
		Image t(width(), height(), cc);
		// copy line by line
		for(size_t y=0; y<height(); y++) {
			std::copy(scanline(y), scanline(y) + cc * width(), t.scanline(height() - y - 1));
		}
		return t;
	}

	/** Index of pixel data */
	index_t index(index_t x, index_t y) const {
		BOOST_ASSERT(isValidIndex(x,y));
		return this->channelCount() * (x + y*width());
	}

	/** @deprecated */
	index_t getIndex(index_t x, index_t y) const {
		return index(x,y);
	}

	/** A pointer to the pixel data */
	K* pointer(index_t x, index_t y) const {
		return buffer_.begin() + index(x, y);
	}

	/** A pointer to the start to the y-th line */
	K* scanline(index_t y) const {
		return pointer(0, y);
	}

private:
	void allocate() {
		resize(width_, height_);
	}

private:
	index_t width_, height_;

	Buffer<K> buffer_;

};

//----------------------------------------------------------------------------//

namespace detail
{
	struct ImageContainerPtr
	{
		virtual ~ImageContainerPtr() {}

		virtual unsigned int width() const = 0;
		virtual unsigned int height() const = 0;
		virtual unsigned int channelCount() const = 0;
		virtual bool hasElementType(const std::type_info& id) const = 0;
		virtual const void* begin() const = 0;
		virtual void* begin() = 0;
	};

	template<typename T>
	struct ImagePtrImpl
	: public ImageContainerPtr
	{
		typedef Image<T> Type;

		ImagePtrImpl(const Type& img)
		: image_(img) {}

		const Type& image() const {
			return image_;
		}

		unsigned int width() const {
			return image_.width();
		}

		unsigned int height() const {
			return image_.height();
		}

		unsigned int channelCount() const {
			return image_.channelCount();
		}

		bool hasElementType(const std::type_info& id) const {
			return id == typeid(typename T::element_t);
		}

		const void* begin() const {
			return static_cast<const void*>(image_.buffer().begin());
		}

		void* begin() {
			return static_cast<void*>(image_.buffer().begin());
		}

	private:
		Type image_;
	};
}

typedef boost::shared_ptr<detail::ImageContainerPtr> ImagePtr;

template<typename T>
ImagePtr Ptr(const Image<T>& img)
{
	return ImagePtr(new detail::ImagePtrImpl<T>(img));
}

template<typename K, unsigned int CC>
Image<Traits<K,CC>> Ref(const ImagePtr& ptr)
{
	typedef detail::ImagePtrImpl<Traits<K,CC>> Type;
	Type* x = dynamic_cast<Type*>(ptr.get());
	if(x == 0) {
		throw "Invalid type";
	}
	else {
		return x->image();
	}
}

template<typename K, unsigned int CC>
bool HasType(const ImagePtr& ptr)
{
	typedef detail::ImagePtrImpl<Traits<K,CC>> Type;
	Type* x = dynamic_cast<Type*>(ptr.get());
	return (x != 0);
}

//----------------------------------------------------------------------------//

template<typename U>
Image<Traits<unsigned char,1>> Threshold(const Image<Traits<U,1>>& u, U threshold) {
	Image<Traits<unsigned char,1>> v(u.width(), u.height());
	for(unsigned int i=0; i<u.size(); i++) {
		v[i] = (u[i] >= threshold) ? 255 : 0;
	}
	return v;
}

template<typename T>
Image<T> operator+(const Image<T>& a, const Image<T>& b) {
	BOOST_ASSERT(a.dimensions() == b.dimensions());
	Image<T> c = a.clone();
	for(unsigned int i=0; i<c.size(); i++) {
		c[i] += b[i];
	}
	return c;
}

template<typename T>
Image<T> operator-(const Image<T>& a, const Image<T>& b) {
	BOOST_ASSERT(a.dimensions() == b.dimensions());
	Image<T> c = a.clone();
	for(unsigned int i=0; i<c.size(); i++) {
		c[i] -= b[i];
	}
	return c;
}

template<typename T>
Image<T> operator*(const Pixel<T>& a, const Image<T>& b) {
	Image<T> c(b.dimensions());
	for(unsigned int i=0; i<c.size(); i++) {
		c[i] = a * b[i];
	}
	return c;
}

template<typename T>
Image<T> abs(const Image<T>& a) {
	Image<T> c(a.width(), a.height());
	for(unsigned int i=0; i<c.size(); i++) {
		c[i] = std::abs(a[i]);
	}
	return c;
}

template<typename K>
Image<Traits<K,1>> Pick(const ImagePtr& raw, unsigned int c) {
	Image<Traits<K,1>> img(raw->width(), raw->height());
	img.buffer().copyFromInterleaved(static_cast<const K*>(raw->begin()) + c, raw->channelCount());
	return img;
}

//----------------------------------------------------------------------------//
}
//----------------------------------------------------------------------------//
#include "Types.hpp"
//----------------------------------------------------------------------------//
#endif
