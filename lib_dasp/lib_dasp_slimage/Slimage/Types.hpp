/*
 * Types.hpp
 *
 *  Created on: Mar 22, 2012
 *      Author: david
 */

#ifndef SLIMAGE_TYPES_HPP_
#define SLIMAGE_TYPES_HPP_
//----------------------------------------------------------------------------//
#include <stdint.h>
//----------------------------------------------------------------------------//
namespace slimage {
//----------------------------------------------------------------------------//

#define SLIMAGE_CREATE_TYPEDEF(T,CC,S)\
	typedef Traits<T,CC> Traits##CC##S; \
	typedef Pixel<Traits##CC##S> Pixel##CC##S; \
	typedef PixelAccess<Traits##CC##S> PixelAccess##CC##S; \
	typedef Iterator<Traits##CC##S> It##CC##S; \
	typedef Image<Traits##CC##S> Image##CC##S;

SLIMAGE_CREATE_TYPEDEF(unsigned char, 1, ub)
SLIMAGE_CREATE_TYPEDEF(unsigned char, 3, ub)
SLIMAGE_CREATE_TYPEDEF(unsigned char, 4, ub)
SLIMAGE_CREATE_TYPEDEF(float, 1, f)
SLIMAGE_CREATE_TYPEDEF(float, 2, f)
SLIMAGE_CREATE_TYPEDEF(float, 3, f)
SLIMAGE_CREATE_TYPEDEF(float, 4, f)
SLIMAGE_CREATE_TYPEDEF(double, 1, d)
SLIMAGE_CREATE_TYPEDEF(double, 2, d)
SLIMAGE_CREATE_TYPEDEF(double, 3, d)
SLIMAGE_CREATE_TYPEDEF(double, 4, d)
SLIMAGE_CREATE_TYPEDEF(uint16_t, 1, ui16)
SLIMAGE_CREATE_TYPEDEF(int, 1, i)

//typedef Traits<unsigned char, 1> Traits1ub;
//typedef Traits<unsigned char, 3> Traits3ub;
//typedef Traits<unsigned char, 4> Traits4ub;
//typedef Traits<float, 1> Traits1f;
//typedef Traits<float, 2> Traits2f;
//typedef Traits<float, 3> Traits3f;
//typedef Traits<float, 4> Traits4f;
//typedef Traits<double, 1> Traits1d;
//typedef Traits<double, 2> Traits2d;
//typedef Traits<double, 3> Traits3d;
//typedef Traits<double, 4> Traits4d;
//typedef Traits<uint16_t, 1> Traits1ui16;
//typedef Traits<int, 1> Traits1i;
//
//
////typedef Pixel<unsigned char, 1> Pixel1ub;
////typedef Pixel<unsigned char, 3> Pixel3ub;
////typedef Pixel<unsigned char, 4> Pixel4ub;
////typedef Pixel<float, 1> Pixel1f;
////typedef Pixel<float, 2> Pixel2f;
////typedef Pixel<float, 3> Pixel3f;
////typedef Pixel<float, 4> Pixel4f;
////typedef Pixel<int, 1> Pixel1i;
//typedef Pixel<Traits1ub> Pixel1ub;
//typedef Pixel<Traits3ub> Pixel3ub;
//typedef Pixel<Traits4ub> Pixel4ub;
//typedef Pixel<Traits1f> Pixel1f;
//typedef Pixel<Traits2f> Pixel2f;
//typedef Pixel<Traits3f> Pixel3f;
//typedef Pixel<Traits4f> Pixel4f;
//typedef Pixel<Traits1i> Pixel1i;
//typedef Pixel<Traits1ui16> Pixel1ui16;
//
//
//typedef Iterator<Traits1ub> It1ub;
//typedef Iterator<Traits3ub> It3ub;
//typedef Iterator<Traits4ub> It4ub;
//typedef Iterator<Traits1f> It1f;
//typedef Iterator<Traits2f> It2f;
//typedef Iterator<Traits3f> It3f;
//typedef Iterator<Traits4f> It4f;
//typedef Iterator<Traits1i> It1i;
//typedef Iterator<Traits1ui16> It1ui16;
//
////typedef Image<unsigned char,0> ImageXub;
////typedef Image<uint16_t,0> ImageXui16;
////typedef Image<float,0> ImageXf;
////typedef Image<double,0> ImageXd;
//typedef Image<Traits1ub> Image1ub;
//typedef Image<Traits3ub> Image3ub;
//typedef Image<Traits4ub> Image4ub;
//typedef Image<Traits1ui16> Image1ui16;
//typedef Image<Traits1f> Image1f;
//typedef Image<Traits2f> Image2f;
//typedef Image<Traits3f> Image3f;
//typedef Image<Traits4f> Image4f;
//typedef Image<Traits1d> Image1d;
//typedef Image<Traits2d> Image3d;
//typedef Image<Traits3d> Image4d;
//typedef Image<Traits1i> Image1i;

//----------------------------------------------------------------------------//
}
//----------------------------------------------------------------------------//
#endif
