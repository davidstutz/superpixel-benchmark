#ifndef INCLUDED_DENSITY_VISUALIZATION_HPP
#define INCLUDED_DENSITY_VISUALIZATION_HPP

#include <Danvil/Color.h>
#include <Slimage/Slimage.hpp>
#include <Eigen/Dense>
#include <vector>

namespace density
{

	inline Eigen::Vector3f DensityColor(float x, float a, float b)
	{
		static auto cm = Danvil::ContinuousIntervalColorMapping<float, float>::Factor_Black_Blue_Red_Yellow_White();
		cm.setRange(a, b);
		Danvil::Colorf color = cm(x);
		return {color.r,color.g,color.b};
	}

	inline Eigen::Vector3f DeltaDensityColor(float x, float a)
	{
		static auto cm = Danvil::ContinuousIntervalColorMapping<float, float>::Factor_MinusPlus();
		cm.setRange(-a, +a);
		Danvil::Colorf color = cm(x);
		return {color.r,color.g,color.b};
	}

	slimage::Image3ub PlotDensity(const Eigen::MatrixXf& d, float a, float b);

	slimage::Image3ub PlotDensity(const Eigen::MatrixXf& d);

	slimage::Image3ub PlotDeltaDensity(const Eigen::MatrixXf& dd, float a);

	slimage::Image3ub PlotDeltaDensity(const Eigen::MatrixXf& dd);

	slimage::Image3ub PlotDeltaDensity(const Eigen::MatrixXf& actual, const Eigen::MatrixXf& reference);

}

#endif
