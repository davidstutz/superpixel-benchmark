/*
 * Point.hpp
 *
 *  Created on: Apr 4, 2012
 *      Author: david
 */

#ifndef DASP_POINT_HPP_
#define DASP_POINT_HPP_

#include "Parameters.hpp"
#include "Array.hpp"
#include <Danvil/Tools/MoreMath.h>
#include <Eigen/Dense>
#include <vector>

namespace dasp
{
	struct Point
	{
		/** image pixel coordinate of point */
		int px, py;

		/** point color */
		Eigen::Vector3f color;

		/** position [m] of world source point */
		Eigen::Vector3f position;

		/** point surface normal */
		Eigen::Vector3f normal;

		/** estimated radius [px] on the image screen of a super pixel at point depth */
		float cluster_radius_px;

		/** Invalid points are ignored during point to cluster assignment */
		bool is_valid;

		/** Depth [m] of point */
		float depth() const {
			return position[2];
		}

		/** Sets the normal and assures that it points towards the camera (=origin) */
		void setNormal(const Eigen::Vector3f& n) {
			normal = n;
			// force normal to look towards the camera
			// check if point to camera direction and normal are within 90 deg
			// enforce: normal * (cam_pos - pos) > 0
			// do not need to normalize (cam_pos - pos) as only sign is considered
			const float q = normal.dot(-position);
			if(q < 0) {
				normal *= -1.0f;
			}
			else if(q == 0) {
				// this should not happen ...
				normal = Eigen::Vector3f(0,0,-1);
			}
		}

		/** Sets normal from gradient */
		void setNormalFromGradient(const Eigen::Vector2f& g) {
			const float gx = g.x();
			const float gy = g.y();
			const float scl = Danvil::MoreMath::FastInverseSqrt(gx*gx + gy*gy + 1.0f);
			setNormal(Eigen::Vector3f(scl*gx, scl*gy, -scl));
		}

		/** Computes local depth gradient (depth[m]/distance[m]) */
		Eigen::Vector2f computeGradient() const {
			return Eigen::Vector2f(normal.x() / normal.z(), normal.y() / normal.z());
		}

		/** Computes direction of local depth gradient */
		Eigen::Vector2f computeGradientDirection() const {
			const float nx = normal.x();
			const float ny = normal.y();
			if(nx == 0.0f && ny == 0.0f) {
				return Eigen::Vector2f::Unit(0);
			}
			else {
				const float scl = Danvil::MoreMath::FastInverseSqrt(nx*nx + ny*ny);
				return scl * Eigen::Vector2f(nx, ny);
			}
		}

		/** Computes "circularity"
		  * This is |n_z| = 1/sqrt(||gradient||^2 + 1) = ea/eb = sqrt(1 - ecc*ecc)
		  */
		float computeCircularity() const {
			return std::abs(normal.z());
		}

//	public:
//		 EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	};

	typedef Array<Point,unsigned int> ImagePoints;

	struct Cluster
	{
		static constexpr float cPercentage = 0.95f; //0.99f;
		static constexpr float cSigmaScale = 1.959964f; //2.575829f;

		int seed_id;

		bool is_fixed;

		Point center;

		// Eigen::Matrix3f color_covariance;

		std::vector<unsigned int> pixel_ids;

		bool isValid() const {
			return is_fixed || pixel_ids.size() > 3;
		}

		void addPixel(unsigned int index) {
			pixel_ids.push_back(index);
		}

		void removePixel(unsigned int index) {
			auto it = std::find(pixel_ids.begin(), pixel_ids.end(), index);
			if(it != pixel_ids.end()) {
				pixel_ids.erase(it);
			}
		}

//		void addPixels(const std::vector<unsigned int>& v) {
//			pixel_ids.insert(pixel_ids.begin(), v.begin(), v.end());
//		}

		// point covariance matrix
		Eigen::Matrix3f cov;
		// eigenvalues of the covariance matrix
		Eigen::Vector3f ew;
		// eigenvectors of the covariance matrix
		Eigen::Matrix3f ev;

		/** Thickness of the cluster computed using smalles eigenvalue */
		float thickness;
		/** eccentricity of the ellipse described by a and b */
		float eccentricity;
		/** flatness of the ellipsoide described by a and c */
		float flatness;
		/** actual area */
		float area;
		/** actual area / expected area defined by base radius*/
		float area_quotient;

		/** Shape fitting */
		float shape_0, shape_x, shape_y, shape_xy, shape_xx, shape_yy;

		/** Thickness of cluster computed using orthogonal distance from plane
		 * WARNING: only computed if ComputeExt is called!
		 */
		float thickness_plane;

		/** number of pixel which are within superpixel radius but are not part of the superpixel
		 * WARNING: only computed if ComputeExt is called!
		 */
		float coverage_error;

		/** actual area of the superpixel (computed from all pixels) */
		float area_actual;
		/** expected area of the superpixel (considering local geometry and thickness) */
		float area_expected;
		/** expected area using the actual base radius (computed from cluster count) (same for all clusters...) */
		float area_expected_global;

		void UpdateCenter(const ImagePoints& points, const Parameters& opt);

		void ComputeExt(const ImagePoints& points, const Parameters& opt);

	};

}

#endif
