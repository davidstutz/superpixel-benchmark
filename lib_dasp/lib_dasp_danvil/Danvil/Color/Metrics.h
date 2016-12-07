#ifndef DANVIL_COLOR_COLORSPACEMETRIC_H_
#define DANVIL_COLOR_COLORSPACEMETRIC_H_
//------------------------------------------------------------------------------
#include <Danvil/LinAlg.h>
#include <Danvil/Color/HSL.h>
#include <Danvil/Color/HSV.h>
#include <Danvil/Color/LAB.h>
#include <Danvil/Color/XYZ.h>
#include <Danvil/Tools/Unit.h>
#include <Danvil/Tools/MoreMath.h>
#include <Danvil/Tools/Metric.h>
#include <Danvil/Types.h>
//------------------------------------------------------------------------------
namespace Danvil {
//------------------------------------------------------------------------------

template<typename K>
class ColorSpaceMetric {
};

template<>
class ColorSpaceMetric<uchar> {
public:
	/** 1-Norm one in RGB color space
	 * This is the sum of the absolute of the differences of the r/g/b values
	 */
	static float rgb_norm1(uchar r1, uchar g1, uchar b1, uchar r2, uchar g2, uchar b2) {
		// maximum value is 3*255 < 2^15
		short x = Metric::abs_diff_int(r1, r2)
			+ Metric::abs_diff_int(g1, g2)
			+ Metric::abs_diff_int(b1, b2);
		return (float)x / 255.0f;
	}

	/** 2-Norm in RGB color space
	 * This is the sum of the squared differences of the r/g/b values
	 */
	static float rgb_norm2(uchar r1, uchar g1, uchar b1, uchar r2, uchar g2, uchar b2) {
		// maximum value is 3*255*255 < 2^31
		int x = MoreMath::Square((int)r1 - (int)r2)
				+ MoreMath::Square((int)g1 - (int)g2)
				+ MoreMath::Square((int)b1 - (int)b2);
		return (float)x / 255.0f / 255.0f;
	}

	/** 2-Norm in HSV color space
	 * This is the sum of the squared differences of the h/s/v values
	 * computed from the given RGB values.
	 */
	static float hsv_norm2(uchar r1, uchar g1, uchar b1, uchar r2, uchar g2, uchar b2) {
		uchar h1, s1, v1, h2, s2, v2;
		Danvil::convert_rgb_2_hsv(r1, g1, b1, h1, s1, v1);
		Danvil::convert_rgb_2_hsv(r2, g2, b2, h2, s2, v2);
		int x = MoreMath::Square((int)h1 - (int)h2)
				+ MoreMath::Square((int)s1 - (int)s2)
				+ MoreMath::Square((int)v1 - (int)v2);
		return (float)x / 255.0f / 255.0f;
	}

	/** Computes the euler distance between two points in HSV space.
	 * A point in HSV space is a point inside a cone of radius 1 and height 1.
	 * Value (v) is the height, saturation (s) is the radius and hue (h) is the angle phi.
	 * This functions computes the euler distance between two such points.
	 */
	static float hsv_cone(uchar r1, uchar g1, uchar b1, uchar r2, uchar g2, uchar b2) {
		using namespace Danvil::MathUnit;
		uchar h1, s1, v1, h2, s2, v2;
		Danvil::convert_rgb_2_hsv(r1, g1, b1, h1, s1, v1);
		Danvil::convert_rgb_2_hsv(r2, g2, b2, h2, s2, v2);
		float v1f = Ui8(v1);
		float v2f = Ui8(v2);
		float s1f = Ui8(s1);
		float s2f = Ui8(s2);
		float u = v1f*v1f*(s1f*s1f + 1);
		float v = v2f*v2f*(s2f*s2f + 1);
		float C = std::cos((h1 - h2) * Danvil::C_2_PI);// cosEstimated256Diff(h1, h2);
		float w = 2*v1f*v2f*(s1f*s2f*C + 1);
		return u + v + w;
	}

	/** Computes the euler distance between two points in HSL space.
	 * A point in HSL space is a point inside a double cone of radius 1 and height 2*0.5.
	 * Lightness (v) is the height, saturation (s) is the radius and hue (h) is the angle phi.
	 * This functions computes the euler distance between two such points.
	 */
	static float hsl_doublecone(uchar r1, uchar g1, uchar b1, uchar r2, uchar g2, uchar b2) {
		uchar h1, s1, l1, h2, s2, l2;
		Danvil::convert_rgb_2_hsl(r1, g1, b1, h1, s1, l1);
		Danvil::convert_rgb_2_hsl(r2, g2, b2, h2, s2, l2);
		Danvil::ctLinAlg::Vec3f p1 = hsl_2_point(h1, s1, l1);
		Danvil::ctLinAlg::Vec3f p2 = hsl_2_point(h2, s2, l2);
		return Danvil::ctLinAlg::Length2(p1 - p2);
	}

	static float lab(uchar r1, uchar g1, uchar b1, uchar r2, uchar g2, uchar b2) {
		using namespace Danvil::MathUnit;
		// [0..255] -> [1..0]
		float r1f = float(r1) / 255.0f;
		float g1f = float(g1) / 255.0f;
		float b1f = float(b1) / 255.0f;
		float r2f = float(r2) / 255.0f;
		float g2f = float(g2) / 255.0f;
		float b2f = float(b2) / 255.0f;
		// RGB -> LAB
		float lab1_l, lab1_a, lab1_b, lab2_l, lab2_a, lab2_b;
		Danvil::color_rgb_to_lab(r1f, g1f, b1f, lab1_l, lab1_a, lab1_b);
		Danvil::color_rgb_to_lab(r2f, g2f, b2f, lab2_l, lab2_a, lab2_b);
		float d = MoreMath::DotDiff(lab1_l, lab1_a, lab1_b, lab2_l, lab2_a, lab2_b);
		return d;
	}

	static const float HslDoubleConeScaledColorScale = 2.0f;
	static const float HslDoubleConeScaledLightnessScale = 0.25f;

	/** Same as hsl_euler but with scaling
	 * Lighting differences are weighted less than color/saturation differences
	 */
	static float hsl_doublecone_scaled(uchar r1, uchar g1, uchar b1, uchar r2, uchar g2, uchar b2) {
		uchar h1, s1, l1, h2, s2, l2;
		Danvil::convert_rgb_2_hsl(r1, g1, b1, h1, s1, l1);
		Danvil::convert_rgb_2_hsl(r2, g2, b2, h2, s2, l2);
		Danvil::ctLinAlg::Vec3f p1 = hsl_2_point(h1, s1, l1);
		Danvil::ctLinAlg::Vec3f p2 = hsl_2_point(h2, s2, l2);
		Danvil::ctLinAlg::Vec3f d = p1 - p2;
		static const float lightness_scale = 0.25;
		static const float color_scale = 2.0;
		d[0] *= color_scale;
		d[1] *= color_scale;
		d[2] *= lightness_scale;
		return Danvil::ctLinAlg::Length2(d);
	}

	static float no_artefacts(uchar r1, uchar g1, uchar b1, uchar r2, uchar g2, uchar b2) {
		uchar h1, s1, l1, h2, s2, l2;
		Danvil::convert_rgb_2_hsl(r1, g1, b1, h1, s1, l1);
		Danvil::convert_rgb_2_hsl(r2, g2, b2, h2, s2, l2);
		Danvil::ctLinAlg::Vec3f p1 = hsl_2_point(h1, s1, l1);
		Danvil::ctLinAlg::Vec3f p2 = hsl_2_point(h2, s2, l2);
		Danvil::ctLinAlg::Vec3f d = p1 - p2;
		float a = 0.5f*sqrt(MoreMath::Square(d.x) + MoreMath::Square(d.y));
		float bSq = MoreMath::Square(0.5f*d.z);
		float v = MoreMath::Square(a + (1-a)*bSq);
		//float v = d.x*d.x + d.y*d.y + d.z*d.z;
		return v;
	}

	/** Converts an HSL value to a point coordinate in the HSL double cone */
	static Danvil::ctLinAlg::Vec3f hsl_2_point(uchar hi, uchar si, uchar li) {
		using namespace Danvil::MathUnit;
		float h = float(hi) / 255.0f;
		float s = float(si) / 255.0f;
		float l = float(li) / 255.0f;
		float a = float(((li<128)?li:(255-li)) << 1) / 255.0f;
		float as = std::min(a, s);
		return Danvil::ctLinAlg::Vec3f(
				as * std::cos(h * Danvil::C_2_PI),
				as * std::sin(h * Danvil::C_2_PI),
				l);
//		// if l <= 128: we are in the lower cone (height = 2*l)
//		// if l > 128 we are in the upper cone (height = 2*(1-l))
//		uint a = ((l<128)?l:(255-l)) << 1;
//		uint as = Danvil::min<uint>(a, s);
//		float as_unit = Danvil::Map16ToUnit(as);
//		float l_unit = Danvil::Map8ToUnit(l);
//		return Danvil::ctLinAlg::Vec3f(
//				as_unit*Danvil::cosEstimated256(h),
//				as_unit*Danvil::sinEstimated256(h),
//				l_unit);
	}
};

//------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------
#endif
