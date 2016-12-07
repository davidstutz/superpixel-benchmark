/*
 * Convert.hpp
 *
 *  Created on: Apr 2, 2012
 *      Author: david
 */

#ifndef CONVERT_HPP_
#define CONVERT_HPP_
//----------------------------------------------------------------------------//
#include "Pixel.hpp"
#include "Slimage.hpp"
#include <cmath>
#include <stdint.h>
//----------------------------------------------------------------------------//
namespace slimage {
namespace conversion
{

	template<typename K, typename L>
	void Convert(K source, L& target);

	template<typename K>
	inline void Convert(K source, K& target) {
		target = source;
	}

	template<>
	inline void Convert(float source, unsigned char& target) {
		target = static_cast<unsigned char>(std::min<int>(255, std::max<int>(0, static_cast<int>(source * 255.0f))));
	}

	template<>
	inline void Convert(unsigned char source, float& target) {
		target = static_cast<float>(source) / 255.0f;
	}

	template<>
	inline void Convert(int source, uint16_t& target) {
		target = static_cast<uint16_t>(source);
	}

	template<>
	inline void Convert(uint16_t source, int& target) {
		target = static_cast<int>(source);
	}

	template<typename U, typename T>
	inline void Convert(const Pixel<U>& source, Pixel<T>& target) {
		static_assert(T::CC == U::CC, "Channel count does not match!");
		for(unsigned int i=0; i<T::CC; i++) {
			Convert(source[i], target[i]);
		}
	}

	template<typename U, typename T>
	inline void Convert(const Pixel<U>& source, const PixelAccess<T>& target) {
		static_assert(T::CC == U::CC, "Channel count does not match!");
		for(unsigned int i=0; i<T::CC; i++) {
			Convert(source[i], target[i]);
		}
	}

	template<typename U, typename T>
	inline void Convert(const PixelAccess<U>& source, const PixelAccess<T>& target) {
		static_assert(T::CC == U::CC, "Channel count does not match!");
		for(unsigned int i=0; i<T::CC; i++) {
			Convert(source[i], target[i]);
		}
	}

	template<typename U, typename T>
	inline void Convert(const Image<U>& source, Image<T>& target) {
		target.resize(source.dimensions());
		for(unsigned int i=0; i<source.size(); i++) {
			Convert(source[i], target[i]);
		}
	}

//	template<typename X, typename Y>
//	inline Y Convert(const X& x) {
//		Y y;
//		Convert(x, y);
//		return y;
//	}

//	template<unsigned int CC>
//	Image<Traits<float,CC>> Convert_ub_2_f(const Image<Traits<unsigned char,CC>>& u) {
//		Image<Traits<float,CC>> v(u.width(), u.height());
//		for(unsigned int i=0; i<u.size(); i++) {
//			v[i] = float(u[i]) / 255.0f;
//		}
//		return v;
//	}
//
//	template<unsigned int CC>
//	Image<Traits<unsigned char,CC>> Convert_f_2_ub(const Image<Traits<float,CC>>& u, float scl = 1.0f) {
//		Image<Traits<unsigned char,CC>> v(u.width(), u.height());
//		for(unsigned int i=0; i<u.size(); i++) {
//			v[i] = std::max(0, std::min(255, static_cast<int>(scl * 255.0f * u[i])));
//		}
//		return v;
//	}

//----------------------------------------------------------------------------//
}}
//----------------------------------------------------------------------------//
#endif
