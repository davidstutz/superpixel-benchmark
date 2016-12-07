
#include "PDS.hpp"
#include "Tools.hpp"
#include <iostream>

namespace pds {

std::vector<Eigen::Vector2f> Random(const Eigen::MatrixXf& density)
{
	boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > die(
		impl::Rnd(),
		boost::uniform_real<float>(0.0f, 1.0f));

	std::vector<Eigen::Vector2f> seeds;
	for(unsigned int iy=0; iy<density.cols(); iy++) {
		for(unsigned int ix=0; ix<density.rows(); ix++) {
			if(die() < density(ix,iy))
				seeds.push_back(Eigen::Vector2f(ix, iy));
		}
	}

	return seeds;
}

std::vector<Eigen::Vector2f> RectGrid(const Eigen::MatrixXf& density)
{
	const float width = static_cast<float>(density.rows());
	const float height = static_cast<float>(density.cols());
	const float numf = density.sum();
	const float d = std::sqrt(float(width*height) / numf);
	const unsigned int Nx = static_cast<unsigned int>(std::ceil(width / d));
	const unsigned int Ny = static_cast<unsigned int>(std::ceil(height / d));
	const float Dx = width / static_cast<float>(Nx);
	const float Dy = height / static_cast<float>(Ny);
	const float Hx = Dx/2.0f;
	const float Hy = Dy/2.0f;

	std::vector<Eigen::Vector2f> seeds;
	seeds.reserve(Nx*Ny);
	for(unsigned int iy=0; iy<Ny; iy++) {
		float y = Hy + Dy * static_cast<float>(iy);
		for(unsigned int ix=0; ix<Nx; ix++) {
			float x = Hx + Dx * static_cast<float>(ix);
			seeds.push_back(Eigen::Vector2f(x, y));
		}
	}

	return seeds;
}

std::vector<Eigen::Vector2f> HexGrid(const Eigen::MatrixXf& density)
{
	// FIXME implement
	throw 0;
}

}
