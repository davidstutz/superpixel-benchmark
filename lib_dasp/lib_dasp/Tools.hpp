/*
 * Tools.hpp
 *
 *  Created on: Feb 19, 2012
 *      Author: david
 */

#ifndef POINTSANDNORMALS_HPP_
#define POINTSANDNORMALS_HPP_

#include <Slimage/Slimage.hpp>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Eigenvalues>
#include <boost/math/constants/constants.hpp>
#include <vector>
#include <cmath>
#include <ctype.h>
#include <ostream>

namespace dasp {

template<typename K>
K Square(K x) {
	return x*x;
}

template<typename K>
std::ostream& operator<<(std::ostream& os, const std::vector<K>& v)
{
	os << "{";
	for(unsigned int i=0; i<v.size(); i++) {
		os << v[i];
		if(i + 1 != v.size()) {
			os << ", ";
		}
	}
	os << "}";
	return os;
}

template<typename K,typename U=unsigned int>
struct Histogram
{
	Histogram()
	: min_(0), max_(1) {}
	Histogram(unsigned int bin_count, K min, K max)
	: min_(min), max_(max), bins_(bin_count) {}
	void add(K x, U v=U(1)) {
		float p = float(x - min_) * float(bins_.size()) / float(max_ - min_);
		int i = std::round(p);
		if(i < 0) i = 0;
		if(i >= int(bins_.size())) i = bins_.size() - 1;
		bins_[i] += v;
	}
	const std::vector<U>& bins() const {
		return bins_;
	}
	const U operator[](unsigned int i) const {
		return bins_[i];
	}
	friend std::ostream& operator<<(std::ostream& os, const Histogram<K,U>& h) {
		os << h.bins();
		return os;
	}
private:
	K min_, max_;
	std::vector<U> bins_;
};

/***
 *
 */
struct Camera
{
	float cx, cy;
        float crx;
        float cry;
	float focal;
	float z_slope;

	/** Projects a 3D point into the image plane */
	Eigen::Vector2f project(const Eigen::Vector3f& p) const {
		return Eigen::Vector2f(p[0] / p[2] * focal + cx - crx, p[1] / p[2] * focal + cy - cry);
	}

	/** Computes a 3D point from pixel position and z/focal */
	Eigen::Vector3f unprojectImpl(float px, float py, float z_over_f) const {
		return z_over_f * Eigen::Vector3f(px - cx + crx, py - cy + cry, focal);
	}
	
	/** Computes a 3D point from pixel position and depth */
	Eigen::Vector3f unproject(int x, int y, uint16_t depth) const {
		return unprojectImpl(
			static_cast<float>(x), static_cast<float>(y),
			convertKinectToMeter(depth) / focal
		);
	}

	/** Gets kinect depth for a 3D point */
	uint16_t depth(const Eigen::Vector3f& p) const {
		return convertMeterToKinect(p[2]);
	}

	/** Convert kinect depth to meter */
	float convertKinectToMeter(uint16_t d) const {
		return static_cast<float>(d) * z_slope;
	}

	float convertKinectToMeter(int d) const {
		return static_cast<float>(d) * z_slope;
	}

	float convertKinectToMeter(float d) const {
		return d * z_slope;
	}

	/** Convert meter to kinect depth */
	uint16_t convertMeterToKinect(float z) const {
		return static_cast<uint16_t>(z / z_slope);
	}

	float computeOpenGLFOV(unsigned int height) const {
		return 2.0f * std::atan(static_cast<float>(height)/focal*0.5f) / boost::math::constants::pi<float>() * 180.0f;
	}

};

template<typename K>
inline float LocalFiniteDifferencesKinect(K v0, K v1, K v2, K v3, K v4)
{
	if(v0 == 0 && v4 == 0 && v1 != 0 && v3 != 0) {
		return float(v3 - v1);
	}

	bool left_invalid = (v0 == 0 || v1 == 0);
	bool right_invalid = (v3 == 0 || v4 == 0);
	if(left_invalid && right_invalid) {
		return 0.0f;
	}
	else if(left_invalid) {
		return float(v4 - v2);
	}
	else if(right_invalid) {
		return float(v2 - v0);
	}
	else {
		float a = static_cast<float>(std::abs(v2 + v0 - static_cast<K>(2)*v1));
		float b = static_cast<float>(std::abs(v4 + v2 - static_cast<K>(2)*v3));
		float p, q;
		if(a + b == 0.0f) {
			p = q = 0.5f;
		}
		else {
			p = a / (a + b);
			q = b / (a + b);
		}
		return q * static_cast<float>(v2 - v0) + p * static_cast<float>(v4 - v2);
	}
}

template<typename K>
inline K LocalFiniteDifferencesKinectSimple(K v0, K v1, K v2, K v3, K v4)
{
	if(v0 == 0 && v4 == 0 && v1 != 0 && v3 != 0) {
		return v3 - v1;
	}

	bool left_invalid = (v0 == 0 || v1 == 0);
	bool right_invalid = (v3 == 0 || v4 == 0);
	if(left_invalid && right_invalid) {
		return 0;
	}
	else if(left_invalid) {
		return v4 - v2;
	}
	else if(right_invalid) {
		return v2 - v0;
	}
	else {
		K a = std::abs(v2 + v0 - static_cast<K>(2)*v1);
		K b = std::abs(v4 + v2 - static_cast<K>(2)*v3);
		if(a < b) {
			return v2 - v0;
		}
		else {
			return v4 - v2;
		}
	}
}

inline Eigen::Vector2f LocalDepthGradient(const slimage::Image1ui16& depth, unsigned int j, unsigned int i, float z_over_f, float window, const Camera& camera)
{
	// compute w = base_scale*f/d
	unsigned int w = std::max(static_cast<unsigned int>(window + 0.5f), 4u);
	if(w % 2 == 1) w++;

	// can not compute the gradient at the border, so return 0
	if(i < w || depth.height() - w <= i || j < w || depth.width() - w <= j) {
		return Eigen::Vector2f::Zero();
	}

	float dx = LocalFiniteDifferencesKinect<int>(
		depth(j-w,i),
		depth(j-w/2,i),
		depth(j,i),
		depth(j+w/2,i),
		depth(j+w,i)
	);

	float dy = LocalFiniteDifferencesKinect<int>(
		depth(j,i-w),
		depth(j,i-w/2),
		depth(j,i),
		depth(j,i+w/2),
		depth(j,i+w)
	);

	// Theoretically scale == base_scale, but w must be an integer, so we
	// compute scale from the actually used w.

	// compute 1 / scale = 1 / (w*d/f)
	float scl = 1.0f / (float(w) * z_over_f);

	return scl * Eigen::Vector2f(camera.convertKinectToMeter(dx), camera.convertKinectToMeter(dy));
}

inline Eigen::Vector2f LocalDepthGradient(const slimage::Image1ui16& depth, unsigned int j, unsigned int i, float base_radius_m, const Camera& camera)
{
	uint16_t d00 = depth(j,i);
	if(d00 == 0) {
		return Eigen::Vector2f::Zero();
	}
	float z_over_f = camera.convertKinectToMeter(d00) / camera.focal;
	float window = base_radius_m/z_over_f;
	return LocalDepthGradient(depth, j, i, z_over_f, window, camera);
}

template<typename T, typename F>
Eigen::Matrix3f PointCovariance(const std::vector<T>& points, F f)
{
//	Eigen::Matrix3f A = Eigen::Matrix3f::Zero();
//	for(const Eigen::Vector3f& p : points) {
//		A += p * p.transpose();
//	}
	float xx=0.0f, xy=0.0f, xz=0.0f, yy=0.0f, yz=0.0f, zz=0.0f;
	for(auto it=points.begin(); it!=points.end(); ++it) {
		const Eigen::Vector3f& p = f(*it);
		float x = p[0];
		float y = p[1];
		float z = p[2];
		xx += x*x;
		xy += x*y;
		xz += x*z;
		yy += y*y;
		yz += y*z;
		zz += z*z;
	}
	Eigen::Matrix3f A; A << xx, xy, xz, xy, yy, yz, xz, yz, zz;
	A /= static_cast<float>(points.size());
	return A;
}

template<typename T, typename F>
Eigen::Matrix<float,6,1> Shape(const std::vector<T>& points, F f)
{
	typedef double K;
	const K SCL = 100.0f;
	typedef Eigen::Matrix<K,6,6> Mat6;
	typedef Eigen::Matrix<K,6,1> Vec6;
	K Sx=0, Sy=0;
	K Sxx=0, Sxy=0, Syy=0;
	K Sxxx=0, Sxxy=0, Sxyy=0, Syyy=0;
	K Sxxxx=0, Sxxxy=0, Sxxyy=0, Sxyyy=0, Syyyy=0;
	K Sz=0, Szx=0, Szy=0, Szxx=0, Szxy=0, Szyy=0;
	for(auto it=points.begin(); it!=points.end(); ++it) {
		const Eigen::Vector3f& p = f(*it);
		K x = SCL*p[0];
		K y = SCL*p[1];
		K z = SCL*p[2];
		K xx = x*x;
		K xy = x*y;
		K yy = y*y;
		Sx += x;
		Sy += y;
		Sxx += xx;
		Sxy += xy;
		Syy += yy;
		Sxxx += x*xx;
		Sxxy += x*xy;
		Sxyy += x*yy;
		Syyy += y*yy;
		Sxxxx += xx*xx;
		Sxxxy += xx*xy;
		Sxxyy += xx*yy;
		Sxyyy += xy*yy;
		Syyyy += yy*yy;
		Sz += z;
		Szx += z*x;
		Szy += z*y;
		Szxx += z*xx;
		Szxy += z*xy;
		Szyy += z*yy;
	}
	K S0 = points.size();
	Mat6 A;
	A << S0, Sx, Sy, Sxy, Sxx, Syy,
		 Sx, Sxx, Sxy, Sxxy, Sxxx, Sxyy,
		 Sy, Sxy, Syy, Sxyy, Sxxy, Syyy,
		 Sxy, Sxxy, Sxyy, Sxxyy, Sxxxy, Sxyyy,
		 Sxx, Sxxx, Sxxy, Sxxxy, Sxxxx, Sxxyy,
		 Syy, Sxyy, Syyy, Sxyyy, Sxxyy, Syyyy;
	Vec6 b;
	b << Sz, Szx, Szy, Szxy, Szxx, Szyy;
	Vec6 r = A.colPivHouseholderQr().solve(b);
	r[0] /= SCL;
	r[3] *= SCL;
	r[4] *= SCL;
	r[5] *= SCL;
	return r.cast<float>();
}

/** Fits a plane into points and returns the plane normal */
template<typename T, typename F>
Eigen::Vector3f FitNormal(const std::vector<T>& points, F f)
{
//		return Eigen::Vector3f(0.0f, 0.0f, 1.0f);
	// compute covariance matrix
	Eigen::Matrix3f A = PointCovariance(points, f);
	// compute eigenvalues/-vectors
	Eigen::SelfAdjointEigenSolver<Eigen::Matrix3f> solver;
	solver.compute(A);
	// take eigenvector (first eigenvalue is smallest!)
	return solver.eigenvectors().col(0).normalized();
}

}

#endif
