#ifndef INCLUDED_DENSITY_SMOOTH_HPP
#define INCLUDED_DENSITY_SMOOTH_HPP

#include <Eigen/Dense>

namespace density
{

	/** Gaussian blur using density-adaptive radius as kernel radius */
	Eigen::MatrixXf DensityAdaptiveSmooth(const Eigen::MatrixXf& d);

}

#endif
