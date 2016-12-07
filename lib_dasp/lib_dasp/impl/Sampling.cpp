/*
 * Sampling.cpp
 *
 *  Created on: Mar 25, 2012
 *      Author: david
 */

//------------------------------------------------------------------------------
#include <color.hpp>
#include "Sampling.hpp"
#include "../Superpixels.hpp"
#include <pds/Tools.hpp>
#include <density/ScalePyramid.hpp>
#include <Slimage/Paint.hpp>
#include <functional>
#include <boost/random.hpp>
#include <boost/math/constants/constants.hpp>
#include <cmath>

#define CREATE_DEBUG_IMAGES

#ifdef CREATE_DEBUG_IMAGES
	#include <fstream>
	#include <boost/format.hpp>
#endif

//------------------------------------------------------------------------------
namespace dasp {
//------------------------------------------------------------------------------

#ifdef CREATE_DEBUG_IMAGES

template<unsigned int Q>
Eigen::MatrixXf CombineMipmaps(const std::vector<Eigen::MatrixXf>& mm)
{
	Eigen::MatrixXf r = Eigen::MatrixXf::Zero(Q*3*mm[0].rows()/2, Q*mm[0].cols());
	r.block(0, 0, Q*mm[0].rows(), Q*mm[0].cols()) = density::ScaleUp(mm[0], Q);
	unsigned int y = 0;
	for(unsigned int i=1; i<mm.size(); ++i) {
		r.block(Q*mm[0].rows(), y, Q*mm[i].rows(), Q*mm[i].cols()) = density::ScaleUp(mm[i], Q);
		y += Q*mm[i].cols();
	}
	return r;
}

void DebugShowMatrix(const Eigen::MatrixXf& mat, const std::string& tag)
{
	const float range = 5000.0f / static_cast<float>((640*480)/25);
	sDebugImages[tag] = slimage::Ptr(
			common::MatrixToImage(mat,
	 			std::bind(&common::IntensityColor, std::placeholders::_1, 0.0f, range)));
}

void DebugWriteMatrix(const Eigen::MatrixXf& mat, const std::string& tag)
{
	std::ofstream ofs(tag);
	for(int i=0; i<mat.rows(); i++) {
		for(int j=0; j<mat.cols(); j++) {
			ofs << mat(i,j);
			if(j+1 != mat.cols()) {
				ofs << "\t";
			}
		}
		if(i+1 != mat.rows()) {
			ofs << "\n";
		}
	}
}

template<unsigned int Q>
void DebugMipmap(const std::vector<Eigen::MatrixXf>& mipmaps, const std::string& tag)
{
	// boost::format fmt(tag + "_%2d");
	// for(std::size_t i=0; i<mipmaps.size(); ++i) {
	// 	const float range = 3000.0f / static_cast<float>(mipmaps[i].rows() * mipmaps[i].cols());
	// 	Eigen::MatrixXf scl = density::ScaleUp(mipmaps[i], ((Q==2) ? 1 : Q)*(1<<i));
	// 	sDebugImages[(fmt % i).str()] = slimage::Ptr(
	// 		common::MatrixToImage(scl,
	// 			std::bind(&common::IntensityColor, std::placeholders::_1, 0.0f, range)));
	// }
	DebugShowMatrix(CombineMipmaps<Q>(mipmaps), tag);
}

template<unsigned int Q>
void DebugMipmapDelta(const std::vector<Eigen::MatrixXf>& mipmaps, const std::string& tag)
{
	// boost::format fmt(tag + "_%2d");
	// for(std::size_t i=0; i<mipmaps.size(); ++i) {
	// 	const float range = 3000.0f / static_cast<float>(mipmaps[i].rows() * mipmaps[i].cols());
	// 	Eigen::MatrixXf scl = density::ScaleUp(mipmaps[i], ((Q==2) ? 1 : Q)*(1<<i));
	// 	sDebugImages[(fmt % i).str()] = slimage::Ptr(
	// 		common::MatrixToImage(scl,
	// 			std::bind(&common::PlusMinusColor, std::placeholders::_1, range)));
	// }
	const float range = 2500.0f / static_cast<float>((640*480)/25);
	sDebugImages[tag] = slimage::Ptr(
	 		common::MatrixToImage(CombineMipmaps<Q>(mipmaps),
	 			std::bind(&common::PlusMinusColor, std::placeholders::_1, range)));
}

#endif

Eigen::MatrixXf ComputeDepthDensity(const ImagePoints& points, const Parameters& opt)
{
	constexpr float NZ_MIN = 0.174f; // = std::sin(80 deg)

	Eigen::MatrixXf density(points.width(), points.height());
	float* p_density = density.data();
	for(unsigned int i=0; i<points.size(); i++) {
		const Point& p = points[i];
		/** Estimated number of super pixels at this point
		 * We assume circular superpixels. So the area A of a superpixel at
		 * point location is R*R*pi and the superpixel density is 1/A.
		 * If the depth information is invalid, the density is 0.
		 */
		float cnt = 0.0f;
		if(p.is_valid) {
			cnt = 1.0f / (M_PI * p.cluster_radius_px * p.cluster_radius_px);
			// Additionally the local gradient has to be considered.
			if(opt.gradient_adaptive_density) {
				cnt /= std::max(NZ_MIN, p.computeCircularity());
			}
		}
		p_density[i] = cnt;
	}
	return density;
}

Eigen::MatrixXf ComputeSaliency(const ImagePoints& points, const Parameters& opt)
{
	const int rows = points.rows();
	const int cols = points.cols();
	const float BR2_INV = 1.0f / (opt.base_radius * opt.base_radius);
	Eigen::MatrixXf saliency_col(rows, cols);
//	Eigen::MatrixXf saliency_norm(rows, cols);
	float* p_saliency_col = saliency_col.data();
//	float* p_saliency_norm = saliency_norm.data();
	for(int y=0; y<cols; ++y) {
		for(int x=0; x<rows; ++x, ++p_saliency_col/*,++p_saliency_norm*/) {
			const Point& p = points(x,y);
			if(!p.is_valid) {
				*p_saliency_col = 0.0f;
//				*p_saliency_norm = 0.0f;
				continue;
			}
			const int r = static_cast<int>(p.cluster_radius_px + 0.5f);
			const int x0 = std::max(x - r, 0);
			const int x1 = std::min(x + r, rows - 1);
			const int y0 = std::max(y - r, 0);
			const int y1 = std::min(y + r, cols - 1);
			// compute mean
			const Eigen::Vector3f mean_col = p.color;
			const Eigen::Vector3f mean_pos = p.position;
//			const Eigen::Vector3f mean_normal = p.normal;
			// compute compression error
			float err_col = 0.0f;
			float err_norm = 0.0f;
			float w_total = 0.0f;
			for(int i=y0; i<=y1; i++) {
				for(int j=x0; j<=x1; j++) {
					const Point& q = points(j,i);
					if(!q.is_valid)
						continue;
					float w = 1.0f / (1.0f + (q.position - mean_pos).squaredNorm() * BR2_INV);
					w_total += w;
					err_col += w * (q.color - mean_col).squaredNorm();
//					err_norm += w * (1.0f - q.normal.dot(mean_normal));
				}
			}
			// write
			*p_saliency_col = std::sqrt(err_col / w_total);
//			*p_saliency_norm = err_norm / w_total;
		}
	}
	// normalize
	{
		const float mean = saliency_col.mean();
		const float min = saliency_col.minCoeff();
		const float max = saliency_col.maxCoeff();
//		std::cout << "color: mean=" << mean << ", min=" << min << ", max=" << max << std::endl;
		saliency_col = (saliency_col.array() - mean)/std::max(mean-min, max-mean);
	}
	// {
	// 	const float mean = saliency_norm.mean();
	// 	const float min = saliency_norm.minCoeff();
	// 	const float max = saliency_norm.maxCoeff();
//	// 	std::cout << "normal: mean=" << mean << ", min=" << min << ", max=" << max << std::endl;
	// 	saliency_norm = (saliency_norm.array() - mean)/std::max(mean-min, max-mean);
	// }
	return saliency_col;// + 0.25f * saliency_norm;
}

void AdaptClusterRadiusBySaliency(ImagePoints& points, const Eigen::MatrixXf& saliency, const Parameters& opt)
{
	// FIXME what is base?
	const float base = 0.5f;
	auto it_p = points.begin();
	auto it_p_end = points.end();
	auto it_s = saliency.data();
	for(; it_p!=it_p_end; ++it_p, ++it_s) {
		it_p->cluster_radius_px *= std::pow(base, -*it_s);
	}
}

//------------------------------------------------------------------------------

void SetRandomNumberSeed(unsigned int x)
{
	pds::impl::RndSeed(x);
}

//------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------

