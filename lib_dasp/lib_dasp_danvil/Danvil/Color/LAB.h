/*
 * LAB.h
 *
 * See http://en.wikipedia.org/wiki/Lab_color_space
 * See http://www.brucelindbloom.com/
 *
 *  Created on: 31.07.2009
 *      Author: david
 */

#ifndef DANVIL_COLOR_LAB_H_
#define DANVIL_COLOR_LAB_H_
//------------------------------------------------------------------------------
#include <Danvil/Types.h>
#include <Danvil/Tools/MoreMath.h>
#include <Danvil/Color/XYZ.h>
//------------------------------------------------------------------------------
namespace Danvil {
//------------------------------------------------------------------------------

inline
float color_xyz_to_lab_help(float p, float p_n) {
	// see http://en.wikipedia.org/wiki/Lab_color_space
	static const float Min = 216.0f / 24389.0f;
	float a = p / p_n;
	if(a <= Min) {
		static const float Fac1 = 841.0f / 108.0f; // = (29/6)^2 / 3
		static const float Fac2 = 4.0f / 29.0f; // = 16/116
		return Fac1*a + Fac2;
	} else {
		return std::pow(a, 0.33333f);
	}
}

inline
void white_point_xyChromatic(float t, float& xD, float& yD) {
	// 6500 K
	xD = 0.312713f;
	yD = 0.329016f;
	// TODO other color temperatures
	/*float ti = 1.0f / t;
	if(t < 7000.0f) {
		xD = 0.244063f + (0.09911f*10e3f + (2.9678f*10e6f - 4.6070f*10e9f*ti)*ti)*ti;
	} else {
		xD = 0.237040f + (0.24748f*10e3f + (1.9018f*10e6f - 2.0064f*10e9f*ti)*ti)*ti;
	}
	yD = -3.000f*xD*xD + 2.870f*xD - 0.275f;*/
}

inline
void white_point_xyz(float t, float& x, float& y, float& z) {
	float xD, yD;
	white_point_xyChromatic(t, xD, yD);
	y = 1.0f;
	xyChromatic_to_xyz(xD, yD, y, x, z);
}

inline
void color_xyz_to_lab(float x, float y, float z, float& l, float& a, float& b) {
	// see http://en.wikipedia.org/wiki/Lab_color_space
	// use D65 as reference white
	float x_n, y_n, z_n;
	white_point_xyz(6500.0f, y_n, x_n, z_n);
	// convert xyz (tristimulus) to lab
	float u = color_xyz_to_lab_help(y, y_n);
	l = 116.0f * u - 16.0f;
	a = 500.0f * (color_xyz_to_lab_help(x, x_n) - u);
	b = 200.0f * (u - color_xyz_to_lab_help(z, z_n));
}

inline
void color_xyz_to_lab_nowhite(float x, float y, float z, float& l, float& a, float& b) {
	// convert xyz (tristimulus) to lab
	float u = color_xyz_to_lab_help(y, 1.0f);
	l = 116.0f * u - 16.0f;
	a = 500.0f * (color_xyz_to_lab_help(x, 1.0f) - u);
	b = 200.0f * (u - color_xyz_to_lab_help(z, 1.0f));
}

inline
float color_lab_to_xyz_xz(float f) {
	float u = f*f*f;
	static const float epsilon = 216.0f / 24389.0f;
	static const float kappa = 24389.0f / 27.0f;
	static const float ConstA = 116.0f / kappa;
	static const float ConstB = 16.0f / kappa;
	if(u > epsilon) {
		return u;
	} else {
		return ConstA * f - ConstB;
	}
}

inline
float color_lab_to_xyz_y(float L, float u) {
	static const float epsilon = 216.0f / 24389.0f;
	static const float kappa = 24389.0f / 27.0f;
	static const float ConstA = epsilon * kappa;
	static const float kappa_inv = 1.0f / kappa;
	if(L > ConstA) {
		return u*u*u;
	} else {
		return L * kappa_inv;
	}
}

inline
void color_lab_to_xyz(float l, float a, float b, float& x, float& y, float& z) {
	// use D65 as reference white
	float x_n, y_n, z_n;
	white_point_xyz(6500.0f, y_n, x_n, z_n);
	// convert lab to xyz
	float fy = (l + 16.0f) / 116.0f;
	float fx = a / 500.0f + fy;
	float fz = fy - b / 200.0f;
	float sy = color_lab_to_xyz_y(l, fy);
	float sx = color_lab_to_xyz_xz(fx);
	float sz = color_lab_to_xyz_xz(fz);
	x = sx * x_n;
	y = sy * y_n;
	z = sz * z_n;
}

inline
void color_rgb_to_lab(float r, float g, float b, float& l, float& la, float& lb) {
	float x, y, z;
	color_rgb_to_xyz(r, g, b, x, y, z);
	color_xyz_to_lab(x, y, z, l, la, lb);
}

inline
void color_lab_to_rgb(float l, float la, float lb, float& r, float& g, float& b) {
	float x, y, z;
	color_lab_to_xyz(l, la, lb, x, y, z);
	color_xyz_to_rgb(x, y, z, r, g, b);
}

//------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------
#endif /* DANVIL_COLOR_LAB_H_ */
