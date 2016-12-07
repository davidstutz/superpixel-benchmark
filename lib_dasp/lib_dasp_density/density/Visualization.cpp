#include "Visualization.hpp"
#include <color.hpp>

namespace density
{

	slimage::Image3ub PlotDensity(const Eigen::MatrixXf& d, float a, float b)
	{
		return common::MatrixToImage(d,
			std::bind(&DensityColor, std::placeholders::_1, a, b));
	}

	slimage::Image3ub PlotDensity(const Eigen::MatrixXf& d) {
		return PlotDensity(d,
			d.minCoeff(), d.maxCoeff());
	}

	slimage::Image3ub PlotDeltaDensity(const Eigen::MatrixXf& dd, float a)
	{
		return common::MatrixToImage(dd,
			std::bind(&DeltaDensityColor, std::placeholders::_1, a));
	}

	slimage::Image3ub PlotDeltaDensity(const Eigen::MatrixXf& dd)
	{
		return PlotDeltaDensity(dd, 
			std::max(std::abs(dd.minCoeff()), std::abs(dd.maxCoeff())));
	}

	slimage::Image3ub PlotDeltaDensity(const Eigen::MatrixXf& actual, const Eigen::MatrixXf& reference)
	{
		float a = std::abs(reference.maxCoeff());
		return PlotDeltaDensity(actual - reference, 0.1f * a);
	}

}
