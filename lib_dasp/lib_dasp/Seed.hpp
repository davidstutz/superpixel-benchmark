/*
 * Seed.hpp
 *
 *  Created on: Aug 24, 2012
 *      Author: david
 */

#ifndef DASP_SEED_HPP_
#define DASP_SEED_HPP_

#include <Eigen/Dense>

namespace dasp
{

	/** A cluster seed */
	struct Seed
	{
		int x, y;
		float scala;
		int label;

		// if is_fixed is enabled the cluster 3d position is always set to fixed_world
		bool is_fixed;
		// only used if is_fixed equals true
		Eigen::Vector3f fixed_world;
		Eigen::Vector3f fixed_color;
		Eigen::Vector3f fixed_normal;

		static Seed Dynamic(int x, int y, float scala) {
			return Seed{x, y, scala, -1, false,
				Eigen::Vector3f::Zero(), Eigen::Vector3f::Zero(), Eigen::Vector3f::Zero()};
		}
		static Seed Static(int x, int y, float scala, const Eigen::Vector3f& position, const Eigen::Vector3f& color, const Eigen::Vector3f& normal) {
			return Seed{x, y, scala, -1, true,
				position, color, normal};
		}
	};

}

#endif
