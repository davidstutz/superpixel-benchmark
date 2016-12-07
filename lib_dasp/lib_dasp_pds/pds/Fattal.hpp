/*
 * BlueNoise.hpp
 *
 *  Created on: Feb 6, 2012
 *      Author: david
 */

#ifndef BLUENOISE_HPP_
#define BLUENOISE_HPP_
//----------------------------------------------------------------------------//
#include <Slimage/Slimage.hpp>
#include <Danvil/Tools/FunctionCache.h>
#include <Eigen/Dense>
#include <vector>
#include <algorithm>
#include <cmath>
//----------------------------------------------------------------------------//
namespace pds {
//----------------------------------------------------------------------------//

namespace fattal
{
	// need to change some other functions too!!!
	constexpr unsigned int D = 2;

	struct Point {
		float x, y;
		float weight;
		float scale;
	};

	constexpr float KernelRange = 2.5f;

	constexpr float cMaxRefinementScale = 10.0f;

	constexpr float cPi = 3.141592654f;

	/** phi(x) = exp(-pi*x*x) */
	inline
	float KernelFunctorImpl(float d) {
		return std::exp(-cPi*d*d);
	}

	/**
	 * Warning: only defined for y <= 1!
	 */
	inline float KernelFunctorInverse(float y) {
		return std::sqrt(- std::log(y) / cPi);
	}

	inline
	float KernelFunctor(float d) {
		static Danvil::FunctionCache<float,1> cache(0.0f, KernelRange, &KernelFunctorImpl);
		return cache(std::abs(d));
	}

	inline
	float KernelFunctorSquareImpl(float d) {
		return std::exp(-cPi*d);
	}

	inline
	float KernelFunctorSquare(float d) {
		static Danvil::FunctionCache<float,1> cache(0.0f, KernelRange*KernelRange, &KernelFunctorSquareImpl);
		return cache(d);
	}

	inline
	float ZeroBorderAccess(const Eigen::MatrixXf& density, int x, int y) {
		if(0 <= x && x < int(density.rows()) && 0 <= y && y < int(density.cols())) {
			return density(x, y);
		}
		else {
			return 0.0f;
		}
	}

	inline
	float ZeroBorderAccess(const Eigen::MatrixXf& density, float x, float y) {
		return ZeroBorderAccess(density, (int)std::round(x), (int)std::round(y));
	}

	inline
	float KernelScaleFunction(float roh, float weight) {
//		return std::pow(roh / weight, -1.0f / float(D));
		return 1.0f / std::sqrt(roh / weight);
	}

	inline
	float ScalePowerD(float s) {
		//return std::pow(s, -float(D));
		return 1.0f / (s*s);
	}

	float EnergyApproximation(const std::vector<Point>& pnts, float x, float y);

	float Energy(const std::vector<Point>& pnts, const Eigen::MatrixXf& density);

	float EnergyDerivative(const std::vector<Point>& pnts, const Eigen::MatrixXf& density, unsigned int i, float& result_dE_x, float& result_dE_y);

	std::vector<Point> PlacePoints(const Eigen::MatrixXf& density, unsigned int p);

	void Refine(std::vector<Point>& points, const Eigen::MatrixXf& density, unsigned int iterations);

	std::vector<Point> Split(const std::vector<Point>& points, const Eigen::MatrixXf& density, bool& result_added);

	std::vector<Point> Compute(const Eigen::MatrixXf& density);

	struct Color {
		unsigned char r,g,b;
	};

	void PlotPoints(const std::vector<Point>& points, const slimage::Image1ub& img, unsigned char grey=0, bool plot_1px=true);

	void PlotPoints(const std::vector<Point>& points, const slimage::Image3ub& img, const slimage::Pixel3ub& color=slimage::Pixel3ub{{0,0,0}}, bool plot_1px=true);

}

//----------------------------------------------------------------------------//
}
//----------------------------------------------------------------------------//
#endif
