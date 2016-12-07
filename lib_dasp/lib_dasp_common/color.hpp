#ifndef COMMON_COLOR_HPP_
#define COMMON_COLOR_HPP_
//----------------------------------------------------------------------------//
#include <Danvil/Color.h>
#include <Eigen/Dense>
#include <Slimage/Slimage.hpp>
//----------------------------------------------------------------------------//
namespace common {
//----------------------------------------------------------------------------//

template<typename K>
inline Eigen::Vector3f GreyColor(K x, K a, K b)
{
	float p = (static_cast<float>(x) - static_cast<float>(a)) / (static_cast<float>(b) - static_cast<float>(a));
	p = std::min(1.0f, std::max(0.0f, p));
	return {p,p,p};
}

/** Computes a color to express a similarity value between 0 and 1 */
inline Eigen::Vector3f SimilarityColor(float x)
{
	static auto cm = Danvil::ContinuousIntervalColorMapping<float, float>::Factor_Black_Blue_Red_Yellow_White();
	cm.setRange(0.0f, 1.0f);
	Danvil::Colorf color = cm(x);
	return {color.r,color.g,color.b};
}

inline Eigen::Vector3f IntensityColor(float x, float a=0.0f, float b=1.0f)
{
	static auto cm = Danvil::ContinuousIntervalColorMapping<float, float>::Factor_Black_Blue_Red_Yellow_White();
	cm.setRange(a, b);
	Danvil::Colorf color = cm(x);
	return {color.r,color.g,color.b};
}

inline Eigen::Vector3f InvIntensityColor(float x, float a=0.0f, float b=1.0f)
{
	return IntensityColor(a + b - x, a, b);
}

inline Eigen::Vector3f PlusMinusColor(float x, float range=1.0f)
{
	static auto cm = Danvil::ContinuousIntervalColorMapping<float, float>::Factor_MinusPlus();
	cm.setRange(-range, +range);
	Danvil::Colorf color = cm(x);
	return {color.r,color.g,color.b};
}

template<typename T>
inline Eigen::Vector3f CountColor(T num, T min, T max)
{
	return IntensityColor(
		(num < min)
			? 0.0f
			: static_cast<float>(num - min)/static_cast<float>(max-min)
	);
}

inline slimage::Pixel3ub ColorToPixel(const Eigen::Vector3f& color) {
	return slimage::Pixel3ub{{
		static_cast<unsigned char>(255.f*std::min(1.0f, std::max(0.0f, color[0]))),
		static_cast<unsigned char>(255.f*std::min(1.0f, std::max(0.0f, color[1]))),
		static_cast<unsigned char>(255.f*std::min(1.0f, std::max(0.0f, color[2])))
	}};
}

inline slimage::Image3ub Colorize(const slimage::Image1ub& img) {
	slimage::Image3ub result(img.width(), img.height());
	const int n = img.size();
	for(int i=0; i<n; i++) {
		result[i] = ColorToPixel(CountColor<uint8_t>(img[i], 0, 255));
	}
	return result;
}

inline slimage::Image3ub ColorizeDepth(const slimage::Image1ui16& img16, uint16_t min, uint16_t max) {
	slimage::Image3ub img(img16.width(), img16.height());
	const int n = img16.size();
	for(int i=0; i<n; i++) {
		img[i] = ColorToPixel(CountColor((uint16_t)img16[i], min, max));
	}
	return img;
}

inline slimage::Image3ub GreyDepth(const slimage::Image1ui16& img16, uint16_t min, uint16_t max) {
	slimage::Image3ub img(img16.width(), img16.height());
	const int n = img16.size();
	for(int i=0; i<n; i++) {
		img[i] = ColorToPixel(GreyColor<uint16_t>(img16[i], min, max));
	}
	return img;
}

template<typename CF>
inline slimage::Image3ub MatrixToImage(const Eigen::MatrixXf& mat, CF cf)
{
	slimage::Image3ub vis = slimage::Image3ub(mat.rows(), mat.cols());
	const float* p = mat.data();
	for(unsigned int i=0; i<vis.size(); i++) {
		vis[i] = ColorToPixel(cf(p[i]));
	}
	return vis;
}

namespace detail
{
	inline unsigned char mean(unsigned char x1, unsigned char x2, unsigned char x3, unsigned char x4, unsigned char x5) {
		unsigned int nb = 4*static_cast<unsigned int>(x1)
			+ static_cast<unsigned int>(x2)
			+ static_cast<unsigned int>(x3)
			+ static_cast<unsigned int>(x4)
			+ static_cast<unsigned int>(x5);
		return nb / 8;
	}
	// inline slimage::Pixel1ub mean(slimage::Pixel1ub x1, slimage::Pixel1ub x2, slimage::Pixel1ub x3, slimage::Pixel1ub x4) {
	// 	return slimage::Pixel1ub{{ mean(x1[0],x2[0],x3[0],x4[0]) }};
	// }
	inline slimage::Pixel3ub mean(slimage::Pixel3ub x1, slimage::Pixel3ub x2, slimage::Pixel3ub x3, slimage::Pixel3ub x4, slimage::Pixel3ub x5) {
		return slimage::Pixel3ub{{
			mean(x1[0],x2[0],x3[0],x4[0],x5[0]),
			mean(x1[1],x2[1],x3[1],x4[1],x5[1]),
			mean(x1[2],x2[2],x3[2],x4[2],x5[2]) }};
	}
}

template<typename T>
slimage::Image<T> Smooth(const slimage::Image<T>& src)
{
	slimage::Image3ub tmp = src.clone();
	for(unsigned int x=1; x<tmp.width()-1; x++) {
		for(unsigned int y=1; y<tmp.height()-1; y++) {
			tmp(x,y) = detail::mean(src(x,y), src(x-1,y), src(x+1,y), src(x,y-1), src(x,y+1));
		}
	}
	return tmp;
}

//----------------------------------------------------------------------------//
}
//----------------------------------------------------------------------------//
#endif
