/*
 * Mipmaps.hpp
 *
 *  Created on: Feb 4, 2012
 *      Author: david
 */

#ifndef INCLUDED_DENSITY_SCALEPYRAMID_HPP_
#define INCLUDED_DENSITY_SCALEPYRAMID_HPP_

#include <Slimage/Slimage.hpp>
#include <Eigen/Dense>
#include <stdexcept>
#include <vector>
#include <tuple>
#include <cassert>

namespace density {

Eigen::MatrixXf SumMipMapWithBlackBorder(const Eigen::MatrixXf& img_big);

template<unsigned int Q>
Eigen::MatrixXf SumMipMap(const Eigen::MatrixXf& img_big)
{
	// size of original image
	const unsigned int w_big = img_big.rows();
	const unsigned int h_big = img_big.cols();
	// size of reduced image
	const unsigned int w_sma = w_big / Q;
	const unsigned int h_sma = h_big / Q;
	// the computed mipmap will have 2^i size
	if(Q*w_sma != w_big || Q*h_sma != h_big) {
		throw std::runtime_error("ERROR: Q and size does not match in function SumMipMap!");
	}
	Eigen::MatrixXf img_small(w_sma, h_sma);
	for(unsigned int y=0; y<h_sma; ++y) {
		const unsigned int y_big = Q*y;
		for(unsigned int x=0; x<w_sma; ++x) {
			const unsigned int x_big = Q*x;
			float sum = 0.0f;
			for(unsigned int i=0; i<Q; ++i) {
				for(unsigned int j=0; j<Q; ++j) {
					sum += img_big(x_big+j, y_big+i);
				}
			}
			img_small(x, y) = sum;
		}
	}
	return img_small;
}

Eigen::MatrixXf ScaleUp(const Eigen::MatrixXf& img_small, unsigned int S);

std::vector<Eigen::MatrixXf> ComputeMipmaps(const Eigen::MatrixXf& img, unsigned int min_size);

std::vector<Eigen::MatrixXf> ComputeMipmapsLevels(const Eigen::MatrixXf& img, unsigned int levels);

std::vector<Eigen::MatrixXf> ComputeMipmaps640x480(const Eigen::MatrixXf& img);

std::vector<std::pair<Eigen::MatrixXf,Eigen::MatrixXf>> ComputeMipmapsWithAbs(const Eigen::MatrixXf& img, unsigned int min_size);

}

#endif
