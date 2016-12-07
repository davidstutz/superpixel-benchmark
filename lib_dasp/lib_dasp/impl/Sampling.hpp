/*
 * Sampling.hpp
 *
 *  Created on: Aug 24, 2012
 *      Author: david
 */

#ifndef DASP_IMPL_SAMPLING_HPP_
#define DASP_IMPL_SAMPLING_HPP_

#include "../Point.hpp"
#include "../Seed.hpp"
#include <Eigen/Dense>
#include <vector>

namespace dasp
{
	Eigen::MatrixXf ComputeDepthDensity(const ImagePoints& points, const Parameters& opt);

	Eigen::MatrixXf ComputeSaliency(const ImagePoints& points, const Parameters& opt);

	void AdaptClusterRadiusBySaliency(ImagePoints& points, const Eigen::MatrixXf& saliency, const Parameters& opt);

}

#endif
