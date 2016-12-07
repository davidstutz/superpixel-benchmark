#ifndef INCLUDED_PDS_DENSITY_HPP
#define INCLUDED_PDS_DENSITY_HPP

#include <Danvil/Tools/FunctionCache.h>
#include <Eigen/Dense>
#include <vector>

namespace density
{

	/** Loads a density function from a file (image or tsv) */
	Eigen::MatrixXf LoadDensity(const std::string& fn);

	/** Saves a density function to a file (image or tsv) */
	void SaveDensity(const std::string& fn, const Eigen::MatrixXf& m);

	constexpr float KernelRange = 2.5f;
	constexpr float cPi = 3.141592654f;

	inline float KernelImpl(float d) {
		return std::exp(-cPi*d*d);
	}

	inline float Kernel(float d) {
		static Danvil::FunctionCache<float,1> cache(0.0f, KernelRange, &KernelImpl);
		return cache(std::abs(d));
	}

	inline float KernelSquareImpl(float d2) {
		return std::exp(-cPi*d2);
	}

	inline float KernelSquare(float d2) {
		static Danvil::FunctionCache<float,1> cache(0.0f, KernelRange*KernelRange, &KernelSquareImpl);
		return cache(d2);
	}

	/** Computes density approximation for a set of points  */
	Eigen::MatrixXf PointDensity(const std::vector<Eigen::Vector2f>& points, const Eigen::MatrixXf& density);

}

#endif
