/*
 * Metric.hpp
 *
 *  Created on: May 18, 2012
 *      Author: david
 */

#ifndef DASP_METRIC_HPP_
#define DASP_METRIC_HPP_

#include "Point.hpp"
#include <Danvil/Tools/MoreMath.h>
#include <Danvil/Tools/FunctionCache.h>
#include <Eigen/Dense>

namespace dasp
{

	namespace metric
	{
		inline int PixelDistanceSquared(const Point& a, const Point& b) {
			const int dx = a.px - b.px;
			const int dy = a.py - b.py;
			return dx*dx + dy*dy;
		}

		inline float ImageDistanceRaw(const Point& x, const Point& y) {
			return static_cast<float>(PixelDistanceSquared(x,y)) / (y.cluster_radius_px * y.cluster_radius_px);
		}

		inline float SpatialDistanceRaw(const Eigen::Vector3f& x, const Eigen::Vector3f& y) {
			return (x - y).squaredNorm();
		}

		inline float SpatialDistanceRaw(const Point& x, const Point& y) {
			return SpatialDistanceRaw(x.position, y.position);
		}

		inline float ColorDistanceRaw(const Eigen::Vector3f& u, const Eigen::Vector3f& v) {
			return (u - v).squaredNorm();
		}

		inline float ColorDistanceRaw(const Point& u, const Point& v) {
			return ColorDistanceRaw(u.color, v.color);
		}

		inline float NormalDistanceRaw(const Eigen::Vector3f& u, const Eigen::Vector3f& v) {
			// this is an approximation to the angle between the normals
			return 1.0f - u.dot(v);
		}

		inline float NormalDistanceRaw(const Point& u, const Point& v) {
			return NormalDistanceRaw(u.normal, v.normal);
		}

		inline float NormalDistanceWithDepth(const Point& u, const Point& v) {
			const float q = NormalDistanceRaw(u.normal, v.normal);
			return 2.0f * q / (u.position.z() + v.position.z());
		}

	}

	/** Computes the density-adaptive distance from a point to a center point
	 * - uses pixel distance and color metric
	 * - Takes the density at the center point
	 */
	struct DensityAdaptiveMetric_UxRGB
	{
		DensityAdaptiveMetric_UxRGB(float w_u, float w_c) {
			weights_ = {
				w_u,
				w_c };
		}

		float operator()(const Point& p, const Point& q) const {
			return weights_.dot(
					Eigen::Vector2f(
							metric::ImageDistanceRaw(p, q),
							metric::ColorDistanceRaw(p, q)));
		}

	private:
		Eigen::Vector2f weights_;
	};

	/** Computes the density-adaptive distance from a point to a center point
	 * - uses pixel distance, color metric and depth difference
	 * - Takes the density at the center point
	 */
	struct DensityAdaptiveMetric_UxRGBxD
	{
		DensityAdaptiveMetric_UxRGBxD(float w_u, float w_c, float w_d) {
			weights_ = {
				w_u,
				w_c,
				w_d };
		}

		float operator()(const Point& p, const Point& q) const {
			return weights_.dot(
					Eigen::Vector3f(
							metric::ImageDistanceRaw(p, q),
							metric::ColorDistanceRaw(p, q),
							std::abs(p.depth() - q.depth())));
		}

	private:
		Eigen::Vector3f weights_;
	};

	/** Computes the depth-adaptive distance from a point to a center point
	 * - Takes the density at the center point
	 * - Uses 3D position instead of pixel position
	 * - Additionally uses normals.
	 */
	struct DepthAdaptiveMetric
	{
		DepthAdaptiveMetric(float w_r, float w_c, float w_n, float R) {
			weights_ = {
				w_r / (R * R),
				w_c,
				w_n };
		}

		float operator()(const Point& p, const Point& q) const {
			return weights_.dot(
					Eigen::Vector3f(
							metric::SpatialDistanceRaw(p, q),
							metric::ColorDistanceRaw(p, q),
							metric::NormalDistanceWithDepth(p, q)));
		}

	private:
		Eigen::Vector3f weights_;
	};

	template<bool SupressConvexEdges=true>
	struct ClassicSpectralAffinity
	{
		ClassicSpectralAffinity(unsigned int num_superpixels, float superpixel_radius, float w_spatial=1.0f, float w_color=1.0f, float w_normal=1.0f)
		: num_superpixels_(num_superpixels),
		  superpixel_radius_(superpixel_radius)
		{
			scl_spatial_ = w_spatial / (4.0f * superpixel_radius_ * superpixel_radius_);
			scl_color_ = w_color / (std::sqrt(static_cast<float>(num_superpixels_)) * cWeightRho);
			scl_normal_ = w_normal;
		}

		float operator()(const Point& x, const Point& y) const {
			const Eigen::Vector3f& x_pos = x.position;
			const Eigen::Vector3f& y_pos = y.position;
			const Eigen::Vector3f& x_col = x.color;
			const Eigen::Vector3f& y_col = y.color;
			const Eigen::Vector3f& x_norm = x.normal;
			const Eigen::Vector3f& y_norm = y.normal;
			// spatial distance
			float scl_d_spatial = metric::SpatialDistanceRaw(x_pos, y_pos) * scl_spatial_;
			scl_d_spatial = std::max(0.0f, scl_d_spatial - 1.2f); // distance of 1 indicates estimated distance
			// color distance
			float d_color = metric::ColorDistanceRaw(x_col, y_col);
			// normal distance
			float d_normal;
			if(SupressConvexEdges) {
				// only use concave edges
				Eigen::Vector3f d = y_pos - x_pos;
				d_normal = (x_norm - y_norm).dot(d) * Danvil::MoreMath::FastInverseSqrt(d.squaredNorm());
				d_normal = std::max(0.0f, d_normal);
			}
			else {
				d_normal = metric::NormalDistanceRaw(x_norm, y_norm);
			}
			// compute total edge connectivity
			float d_combined = scl_d_spatial + scl_color_*d_color + scl_normal_*d_normal;
			return exp_cache_(d_combined);
		}

	private:
		static constexpr float cWeightRho = 0.01f; // 640x480 clusters would yield 0.1 which is used in gPb
		unsigned int num_superpixels_;
		float superpixel_radius_;
		float scl_spatial_;
		float scl_color_;
		float scl_normal_;
		Danvil::ExpNegFunctionCache<float> exp_cache_; // used for std::exp(-x)
	};

	struct ImprovedSpectralAffinity
	{
		ImprovedSpectralAffinity(float superpixel_radius,
			float w_spatial=1.0f, float w_color=1.0f, float w_normal=1.0f)
		: superpixel_radius_(superpixel_radius),
		  ww_(w_spatial), wc_(w_color), wn_(w_normal)
		{
			float w_total = ww_ + wc_ + wn_;
			ww_ /= w_total;
			wc_ /= w_total;
			wn_ /= w_total;
		}

		float operator()(const Cluster& x, const Cluster& y) const {
			const Eigen::Vector3f& x_pos = x.center.position;
			const Eigen::Vector3f& y_pos = y.center.position;
			const Eigen::Vector3f& x_col = x.center.color;
			const Eigen::Vector3f& y_col = y.center.color;
			const Eigen::Vector3f& x_norm = x.center.normal;
			const Eigen::Vector3f& y_norm = y.center.normal;
			// spatial distance
			float dw = (x_pos - y_pos).squaredNorm() / (4.0f * superpixel_radius_ * superpixel_radius_);
			dw = std::max(0.0f, dw - 1.2f); // distance of 1 indicates estimated distance
			// color distance
			float dc = 3.2f * (x_col - y_col).squaredNorm();
			// normal distance (only use concave edges)
			Eigen::Vector3f u = y_pos - x_pos;
			float dn = (x_norm - y_norm).dot(u) / u.norm();
			dn = std::max(0.0f, dn);
			// compute total edge connectivity
			float d = ww_*dw + wc_*dc + wn_*dn;
			return exp_cache_(d);
		}

	private:
		float superpixel_radius_;
		float ww_, wc_, wn_;
		Danvil::ExpNegFunctionCache<float> exp_cache_; // used for std::exp(-x)
	};	

	struct ClassicSpectralAffinitySLIC
	{
		ClassicSpectralAffinitySLIC(unsigned int num_superpixels, float w_color=1.0f)
		: num_superpixels_(num_superpixels),
		  w_color(w_color)
		{}

		float operator()(const Point& x, const Point& y) const {
			float w_maha_color = 4.0f * metric::ColorDistanceRaw(x, y) / (std::sqrt(static_cast<float>(num_superpixels_)) * cWeightRho);
			return std::exp(-w_color*w_maha_color);
		}

	private:
		static constexpr float cWeightRho = 0.01f; // 640x480 clusters would yield 0.1 which is used in gPb
		unsigned int num_superpixels_;
		float w_color;
	};

}

#endif
