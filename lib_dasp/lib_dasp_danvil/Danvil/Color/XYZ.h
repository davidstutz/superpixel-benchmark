/*
 * XYZ.h
 *
 *  Created on: 31.07.2009
 *      Author: david
 */

#ifndef DANVIL_COLOR_XYZ_H_
#define DANVIL_COLOR_XYZ_H_
//------------------------------------------------------------------------------
#include <Danvil/Types.h>
#include <Danvil/Tools/MoreMath.h>
//------------------------------------------------------------------------------
namespace Danvil {
//------------------------------------------------------------------------------

inline
void color_rgb_to_xyz(float r, float g, float b, float& x, float& y, float& z) {
	// Adobe RGB 1998 with D65 as reference white
	x = 0.5767309*r + 0.1855540*g + 0.1881852*b;
	y = 0.2973769*r + 0.6273491*g + 0.0752741*b;
	z = 0.0270343*r + 0.0706872*g + 0.9911085*b;
	/*x = +2.36460 * r - 0.51515 * g + 0.00520 * b;
	y = -0.89653 * r + 1.42640 * g - 0.01441 * b;
	z = -0.46807 * r + 0.08875 * g + 1.00921 * b;*/
}

inline
void color_xyz_to_rgb(float x, float y, float z, float& r, float& g, float& b) {
	// Adobe RGB 1998 with D65 as reference white
	r = +2.0413690*x - 0.5649464*y - 0.3446944*z;
	g = -0.9692660*x + 1.8760108*y + 0.0415560*z;
	b = +0.0134474*x - 0.1183897*y + 1.0154096*z;
	// rgb values can be negative if the color can not be represented correctly
	MoreMath::ClampInplace(r, 0.0f, 1.0f);
	MoreMath::ClampInplace(g, 0.0f, 1.0f);
	MoreMath::ClampInplace(b, 0.0f, 1.0f);
}

inline
void xyChromatic_to_xyz(float xD, float yD, float y, float& x, float& z) {
	float u = y / yD;
	x = u * xD;
	z = u * (1 - xD - yD);
}

inline
void xyz_to_xyChromatic(float x, float y, float z, float& xC, float& yC) {
	float u = x + y + z;
	xC = x / u;
	yC = y / u;
}

//------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------
#endif /* DANVIL_COLOR_XYZ_H_ */
