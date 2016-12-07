/*
 * BlueNoise.cpp
 *
 *  Created on: Feb 6, 2012
 *      Author: david
 */

//#define DEBUG_SAVE_POINTS

#include "Fattal.hpp"
#include <density/ScalePyramid.hpp>
#include <boost/random.hpp>
#ifdef DEBUG_SAVE_POINTS
	#include <boost/format.hpp>
	#include <fstream>
#endif
#define VERBOSE
//----------------------------------------------------------------------------//
namespace pds {
namespace fattal {
//----------------------------------------------------------------------------//

constexpr unsigned MAX_DEPTH = 0;
constexpr unsigned LANGEVIN_STEPS = 20;
constexpr bool MH_TEST = false;
constexpr float STEPSIZE = 0.3f;
constexpr float TEMPERATURE = 0.03f;

float EnergyApproximation(const std::vector<Point>& pnts, float x, float y)
{
	float sum = 0.0f;
	for(const Point& p : pnts) {
		float dx = p.x - x;
		float dy = p.y - y;
		float d2 = dx*dx + dy*dy;
		float scl = p.scale * p.scale;
		if(d2 < KernelRange * KernelRange * scl) {
			sum += p.weight / scl * KernelFunctorSquare(d2 / scl);
		}
	}
	return sum;
}

Eigen::MatrixXf EnergyApproximationMat(const std::vector<Point>& pnts, int rows, int cols)
{
	Eigen::MatrixXf mat = Eigen::MatrixXf::Constant(rows, cols, 0.0f);
	for(const Point& p : pnts) {
		float px = p.x;
		float py = p.y;
		float pw = p.weight;
		float ps = p.scale;
		float scl = p.scale * p.scale;
		constexpr float cMaxRange = 1.482837414f; // eps = 0.001
		float radius = cMaxRange * ps;
		float x_min = std::max(0, int(std::floor(px - radius)));
		float x_max = std::min(int(mat.rows()) - 1, int(std::ceil(px + radius)));
		float y_min = std::max(0, int(std::floor(py - radius)));
		float y_max = std::min(int(mat.cols()) - 1, int(std::ceil(py + radius)));
		// sum over window
		for(unsigned int y=y_min; y<=y_max; y++) {
			for(unsigned int x=x_min; x<=x_max; x++) {
				float ux = float(x);
				float uy = float(y);
				float dx = ux - px;
				float dy = uy - py;
				float d2 = dx*dx + dy*dy;
				mat(x,y) += pw / scl * KernelFunctorSquare(d2 / scl);
			}
		}
	}
	return mat;
	// Eigen::MatrixXf mat(rows, cols);
	// for(unsigned int y=0; y<cols; y++) {
	// 	float py = float(y);
	// 	for(unsigned int x=0; x<rows; x++) {
	// 		float px = float(x);
	// 		float a = EnergyApproximation(pnts, px, py);
	// 		mat(x,y) = a;
	// 	}
	// }
	// return mat;
}

float Energy(const std::vector<Point>& pnts, const Eigen::MatrixXf& density)
{
	return (EnergyApproximationMat(pnts, density.rows(), density.cols()) - density).cwiseAbs().sum();
	// float error = 0.0f;
	// for(unsigned int y=0; y<density.cols(); y++) {
	// 	float py = float(y);
	// 	for(unsigned int x=0; x<density.rows(); x++) {
	// 		float px = float(x);
	// 		float a = EnergyApproximation(pnts, px, py);
	// 		float roh = density(x, y);
	// 		error += std::abs(a - roh);
	// 	}
	// }
	// return error;
}

float EnergyDerivative(const std::vector<Point>& pnts, const Eigen::MatrixXf& density, unsigned int i, float& result_dE_x, float& result_dE_y)
{
	float dE_x = 0.0f;
	float dE_y = 0.0f;

	const Point& pi = pnts[i];
	float px = pi.x;
	float py = pi.y;
	float ps = pi.scale;
	float ps_scl = 1.0f / (ps * ps);
	// find window (points outside the window do not affect the kernel)
	// range = sqrt(ln(1/eps)/pi)
	constexpr float cMaxRange = 1.482837414f; // eps = 0.001
	float radius = cMaxRange * ps;
	float x_min = std::max(0, int(std::floor(px - radius)));
	float x_max = std::min(int(density.rows()) - 1, int(std::ceil(px + radius)));
	float y_min = std::max(0, int(std::floor(py - radius)));
	float y_max = std::min(int(density.cols()) - 1, int(std::ceil(py + radius)));
	// sum over window
	for(unsigned int y=y_min; y<=y_max; y++) {
		for(unsigned int x=x_min; x<=x_max; x++) {
			float ux = float(x);
			float uy = float(y);
			float dx = ux - px;
			float dy = uy - py;
//			float k_arg = std::sqrt(dx*dx + dy*dy) * ps_scl;
//			float k_val = KernelFunctor(k_arg);
			float k_arg_square = (dx*dx + dy*dy) * ps_scl;
			float k_val = KernelFunctorSquare(k_arg_square);
			float apx = EnergyApproximation(pnts, ux, uy);
			float roh = density(x, y);
			if(apx < roh) {
				k_val = -k_val;
			}
			//k_val = 0.0f;
			dE_x += k_val * dx;
			dE_y += k_val * dy;
		}
	}
	float A = 2.0f * cPi / std::pow(ps, float(D + 1));
	result_dE_x = A * dE_x;
	result_dE_y = A * dE_y;
	return radius;
}

std::vector<Point> PlacePoints(const Eigen::MatrixXf& density, unsigned int p)
{
	// access original index in a random order
	std::vector<unsigned int> indices(density.size());
	for(unsigned int i=0; i<indices.size(); i++) {
		indices[i] = i;
	}
	std::random_shuffle(indices.begin(), indices.end());

	// compute points
	std::vector<Point> pnts;
	pnts.reserve(indices.size());
	// compute current error in density
	float error_current = Energy(pnts, density);
//	std::cout << "INITIAL ERROR: " << error_current << std::endl;
	// try add kernel points
	for(unsigned int i : indices) {
		float roh = density.data()[i];
		if(roh == 0) {
//				std::cout << i << " roh is 0!" << std::endl;
			continue;
		}
		Point u;
		u.x = float(i % density.rows());
		u.y = float(i / density.rows());
		int q = p - (roh < 1 ? 0 : std::ceil(std::log2(roh) / float(D)));
		u.weight = float(1 << (D*(p-q)));
		u.scale = KernelScaleFunction(roh, u.weight);
		// try to add
		pnts.push_back(u);
		// check if the points reduced the energy
		float error_new = Energy(pnts, density);
		if(error_new > error_current) {
			// reject
			pnts.pop_back();
//			std::cout << u.x << " " << u.y << " " << u.weight << " " << error_new << " REJECTED" << std::endl;
		}
		else {
			error_current = error_new;
//			std::cout << u.x << " " << u.y << " " << u.weight << " " << error_new << std::endl;
		}
	}

	// std::cout << "Density total: " << density.sum() << std::endl;
	// std::cout << "Approx total:" << EnergyApproximationMat(pnts, density.rows(), density.cols()).sum() << std::endl;

	return pnts;
}

void Refine(std::vector<Point>& points, const Eigen::MatrixXf& density, unsigned int iterations)
{
	static boost::mt19937 rng;
	static boost::normal_distribution<float> rnd(0.0f, 1.0f); // standard normal distribution
	static boost::variate_generator<boost::mt19937&, boost::normal_distribution<float> > die(rng, rnd);
//	float r_min = 1e9;
//	float r_max = 0;
	for(unsigned int k=0; k<iterations; k++) {
		float energy;
		if(MH_TEST) {
			energy = Energy(points, density);
		}
#ifdef VERBOSE
		if(!MH_TEST) {
			energy = Energy(points, density);
		}
		std::cout << "\tit=" << k << ", e=" << energy << std::endl;
#endif
		for(unsigned int i=0; i<points.size(); i++) {
			// random vector
			float rndx = die();
			float rndy = die();
			// compute next position
			Point p = points[i];
			if(p.scale > cMaxRefinementScale) {
				// omit low frequency kernels
				continue;
			}
			// dx = -STEPSIZE*s/2*dE + sqrt(TEMPERATURE*s*STEPSIZE)*rnd()
			float c0 = STEPSIZE * p.scale;
			float cA = c0 * 0.5f;
			float dx, dy;
			float R = EnergyDerivative(points, density, i, dx, dy);
//			std::cout << i << ": (" << dx << "," << dy << "), s=" << p.scale << std::endl;
//			r_min = std::min(R, r_min);
//			r_max = std::max(R, r_max);
//			std::cout << i << " p1: (" << p.x << "," << p.y << ")" << std::endl;
			p.x -= cA * dx;
			p.y -= cA * dy;
			float cB = std::sqrt(TEMPERATURE * c0);
			p.x += cB * rndx;
			p.y += cB * rndy;
			float roh = ZeroBorderAccess(density, p.x, p.y);
			if(roh > 0) {
				p.scale = KernelScaleFunction(roh, p.weight);
			}
			else {
				// reject
				continue;
			}
//			std::cout << i << " p2: (" << p.x << "," << p.y << ")" << std::endl;
			// check if we want to keep the point
			points[i] = p;
			if(MH_TEST) {
				Point pold = p;
				float dxn, dyn;
				EnergyDerivative(points, density, i, dxn, dyn);
				float h = cB / (2.0f*TEMPERATURE);
				float hx = h*(dx + dxn) + rndx;
				float hy = h*(dy + dyn) + rndy;
				float g1 = -0.5f*(hx*hx + hy*hy);
				float g2 = -0.5f*(rndx*rndx + rndy*rndy);
				float energy_new = Energy(points,density);
				float P = std::exp((energy-energy_new)/TEMPERATURE + g1 - g2);
//				std::cout << energy << " -> " << energy_new << ", P=" << P << std::endl;
				if(die() <= P) {
					// accept
					energy = energy_new;
				}
				else {
					// reject
					points[i] = pold;
				}
			}
		}
	}
//	std::cout << r_min << " " << r_max << std::endl;
}

std::vector<Point> Split(const std::vector<Point>& points, const Eigen::MatrixXf& density, bool& result_added)
{
	std::vector<Point> pnts_new;
	result_added = false;
	for(Point u : points) {
		if(u.weight > 1.0f) {
			result_added = true;
			u.x *= 2.0f;
			u.y *= 2.0f;
			u.weight /= float(1 << D);
			constexpr float A = 0.3*0.70710678f;
			constexpr float Delta[4][2] = {
					{-A, -A}, {+A, -A}, {-A, +A}, {+A, +A}
			};
			for(unsigned int i=0; i<4; i++) {
				Point ui = u;
				ui.x += u.scale * Delta[i][0];
				ui.y += u.scale * Delta[i][1];
				float roh = ZeroBorderAccess(density, ui.x, ui.y);
				if(roh > 0) {
					ui.scale = KernelScaleFunction(roh, ui.weight);
					pnts_new.push_back(ui);
				}
			}
		}
		else {
			u.x *= 2.0f;
			u.y *= 2.0f;
			u.weight = 1.0f;
			float roh = ZeroBorderAccess(density, u.x, u.y);
			if(roh > 0) {
				u.scale = KernelScaleFunction(roh, u.weight);
				pnts_new.push_back(u);
			}
		}
	}
	return pnts_new;
}

#ifdef DEBUG_SAVE_POINTS
void SavePoints(const std::vector<Point>& pnt, const std::string& filename)
{
	std::ofstream ofs(filename);
	for(const Point& p : pnt) {
		ofs << p.x << "\t" << p.y << std::endl;
	}
}
#endif

std::vector<Point> Compute(const Eigen::MatrixXf& density)
{
#ifdef DEBUG_SAVE_POINTS
	boost::format fn_fmt("pnt_%1%_%2%.tsv");
#endif
	// compute mipmaps
	std::vector<Eigen::MatrixXf> mipmaps = density::ComputeMipmaps(density, 8);
	int p = int(mipmaps.size()) - 1;
	std::vector<Point> pnts;
	for(int i=p; i>=0; i--) {
#ifdef VERBOSE
		std::cout << "Blue noise step " << i << "... " << std::flush;
#endif
		bool need_refinement;
		if(i == p) {
			// place initial points
			pnts = PlacePoints(mipmaps[i], i);
			need_refinement = true;
		}
		else {
			// split points
			pnts = Split(pnts, mipmaps[i], need_refinement);
		}
		// refine points for new density map
//		if(need_refinement) {
#ifdef DEBUG_SAVE_POINTS
			for(int k=0; k<(i+1)*LANGEVIN_STEPS; k++) {
				Refine(pnts, mipmaps[i], 1);
				SavePoints(pnts, (fn_fmt % i % k).str());
			}
#else
			Refine(pnts, mipmaps[i], LANGEVIN_STEPS);
#endif
//		}
#ifdef VERBOSE
		std::cout << pnts.size() << " points." << std::endl;
#endif
		if(MAX_DEPTH > 0 && p - i + 1 >= MAX_DEPTH) {
			float scl = static_cast<float>(1 << i);
			for(Point& p : pnts) {
				p.x *= scl;
				p.y *= scl;
			}
			break;
		}
	}
	return pnts;
}

template<typename T>
void PlotPoints(const std::vector<Point>& points, const slimage::Image<T>& img, const slimage::Pixel<T>& color, bool plot_1px)
{
	for(Point p : points) {
		// round position
		int px = int(p.x + 0.5f);
		int py = int(p.y + 0.5f);
		if(px < 0 || int(img.width()) <= px || py < 0 || int(img.height()) <= py) {
			continue;
		}
		img(px, py) = color;
		if(!plot_1px) {
			// paint a star
			//    X
			//   XXX
			//    X
			if(1 <= px) {
				img(px-1, py) = color;
			}
			if(px + 1 < int(img.width())) {
				img(px+1, py) = color;
			}
			if(1 <= py) {
				img(px, py-1) = color;
			}
			if(py + 1 < int(img.width())) {
				img(px, py+1) = color;
			}
		}
	}
}

void PlotPoints(const std::vector<Point>& points, const slimage::Image1ub& img, unsigned char grey, bool plot_1px)
{
	PlotPoints(points, img, slimage::Pixel1ub{grey}, plot_1px);
}

void PlotPoints(const std::vector<Point>& points, const slimage::Image3ub& img, const slimage::Pixel3ub& color, bool plot_1px)
{
	PlotPoints(points, img, color, plot_1px);
}

}

//----------------------------------------------------------------------------//

std::vector<Eigen::Vector2f> Fattal(const Eigen::MatrixXf& density)
{
	std::vector<fattal::Point> pnts = fattal::Compute(density);
	std::vector<Eigen::Vector2f> v(pnts.size());
	std::transform(pnts.begin(), pnts.end(), v.begin(),
		[](const fattal::Point& p) {
			return Eigen::Vector2f(
				2.0f*static_cast<float>(p.x), 2.0f*static_cast<float>(p.y));
		});
	return v;
}

//----------------------------------------------------------------------------//
}
//----------------------------------------------------------------------------//
