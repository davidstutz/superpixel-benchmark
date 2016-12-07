#include "ScalePyramid.hpp"
#include <Danvil/Tools/MoreMath.h>
#include <boost/assert.hpp>
#include <iostream>
//----------------------------------------------------------------------------//
namespace density {
//----------------------------------------------------------------------------//

Eigen::MatrixXf SumMipMapWithBlackBorder(const Eigen::MatrixXf& img_big)
{
	size_t w_big = img_big.rows();
	size_t h_big = img_big.cols();
	// the computed mipmap will have 2^i size
	unsigned int size = Danvil::MoreMath::P2Ceil(std::max(w_big, h_big));
	Eigen::MatrixXf img_small(size / 2, size / 2);
	img_small.fill({0.0f});
	// only the part where at least one of the four pixels lies in the big image is iterated
	// the rest was set to 0 with the fill op
	size_t w_small = w_big / 2 + ((w_big % 2 == 0) ? 0 : 1);
	size_t h_small = h_big / 2 + ((h_big % 2 == 0) ? 0 : 1);
	for(size_t y = 0; y < h_small; y++) {
		size_t y_big = y * 2;
		for(size_t x = 0; x < w_small; x++) {
			size_t x_big = x * 2;
			// We sum over all four pixels in the big image (if they are valid).
			// May by invalid because the big image is considered to be enlarged
			// to have a size of 2^i.
			float sum = 0.0f;
			// Since we only test the part where at least one pixel is in also in the big image
			// we do not need to test that (x_big,y_big) is a valid pixel in the big image.
			const float* p_big = &img_big(x_big, y_big);
			sum += *(p_big);
			if(x_big + 1 < w_big) {
				sum += *(p_big + 1);
			}
			if(y_big + 1 < h_big) {
				sum += *(p_big + w_big);
				if(x_big + 1 < w_big) {
					sum += *(p_big + w_big + 1);
				}
			}
			img_small(x, y) = sum;
		}
	}
	return img_small;
}

Eigen::MatrixXf ScaleUp(const Eigen::MatrixXf& img_small, const unsigned int S)
{
	if(S == 0) {
		return Eigen::MatrixXf::Zero(0,0);
	}
	if(S == 1) {
		return img_small;
	}
	// size of original image
	const unsigned int w_sma = img_small.rows();
	const unsigned int h_sma = img_small.cols();
	// size of scaled up image
	const unsigned int w_big = w_sma * S;
	const unsigned int h_big = h_sma * S;
	Eigen::MatrixXf img_big(w_big, h_big);
	for(unsigned int y=0; y<h_sma; ++y) {
		const unsigned int y_big = S*y;
		for(unsigned int x=0; x<w_sma; ++x) {
			const unsigned int x_big = S*x;
			const float val = img_small(x, y);
			for(unsigned int i=0; i<S; ++i) {
				for(unsigned int j=0; j<S; ++j) {
					img_big(x_big+j, y_big+i) = val;
				}
			}
		}
	}
	return img_big;
}

std::vector<Eigen::MatrixXf> ComputeMipmaps(const Eigen::MatrixXf& img, unsigned int min_size)
{
	// find number of required mipmap level
	unsigned int max_size = std::max(img.rows(), img.cols());
	int n_mipmaps = Danvil::MoreMath::PowerOfTwoExponent(max_size);
	n_mipmaps -= Danvil::MoreMath::PowerOfTwoExponent(min_size);
	BOOST_ASSERT(n_mipmaps >= 1);
	std::vector<Eigen::MatrixXf> mipmaps(n_mipmaps);
	mipmaps[0] = SumMipMapWithBlackBorder(img);
	// create remaining mipmaps
	for(unsigned int i=1; i<n_mipmaps; i++) {
		BOOST_ASSERT(mipmaps[i-1].rows() == mipmaps[i-1].cols());
		BOOST_ASSERT(mipmaps[i-1].rows() >= 1);
		mipmaps[i] = SumMipMap<2>(mipmaps[i - 1]);
//		std::cout << std::accumulate(mipmaps[i].begin(), mipmaps[i].end(), 0.0f, [](float sum, float x) { return sum + x; }) << std::endl;
	}
	return mipmaps;
}

std::vector<Eigen::MatrixXf> ComputeMipmapsLevels(const Eigen::MatrixXf& img, unsigned int n_mipmaps)
{
	// find number of required mipmap level
	unsigned int max_size = std::max(img.rows(), img.cols());
	BOOST_ASSERT(n_mipmaps >= 1);
	std::vector<Eigen::MatrixXf> mipmaps(n_mipmaps);
	mipmaps[0] = SumMipMapWithBlackBorder(img);
	// create remaining mipmaps
	for(unsigned int i=1; i<n_mipmaps; i++) {
		BOOST_ASSERT(mipmaps[i-1].rows() == mipmaps[i-1].cols());
		BOOST_ASSERT(mipmaps[i-1].rows() >= 1);
		mipmaps[i] = SumMipMap<2>(mipmaps[i - 1]);
//		std::cout << std::accumulate(mipmaps[i].begin(), mipmaps[i].end(), 0.0f, [](float sum, float x) { return sum + x; }) << std::endl;
	}
	return mipmaps;
}

std::vector<Eigen::MatrixXf> ComputeMipmaps640x480(const Eigen::MatrixXf& img)
{
	// 640 = 4*32*5
	// 480 = 3*32*5
	// 32 = 2^5
	if(img.rows() != 640 || img.cols() != 480) {
		throw std::runtime_error("ERROR: ComputeMipmaps640x480 required size 640x480!");
	}
	std::vector<Eigen::MatrixXf> v(6);
	v[0] = SumMipMap<5>(img);
	for(unsigned int i=0; i<5; i++) {
		v[i+1] = SumMipMap<2>(v[i]);
	}
	return v;
}

std::vector<std::pair<Eigen::MatrixXf,Eigen::MatrixXf>> ComputeMipmapsWithAbs(const Eigen::MatrixXf& img, unsigned int min_size)
{
	// find number of required mipmap level
	unsigned int max_size = std::max(img.rows(), img.cols());
	int n_mipmaps = Danvil::MoreMath::PowerOfTwoExponent(max_size);
	n_mipmaps -= Danvil::MoreMath::PowerOfTwoExponent(min_size);
	BOOST_ASSERT(n_mipmaps >= 1);
	std::vector<std::pair<Eigen::MatrixXf,Eigen::MatrixXf>> mipmaps(n_mipmaps + 1);
	mipmaps[0].first = img;
	mipmaps[0].second = img.cwiseAbs();
	mipmaps[1].first = SumMipMapWithBlackBorder(mipmaps[0].first);
	mipmaps[1].second = SumMipMapWithBlackBorder(mipmaps[0].second);
	for(unsigned int i=2; i<=n_mipmaps; i++) {
		//BOOST_ASSERT(mipmaps[i-1].width() == mipmaps[i-1].height());
		//BOOST_ASSERT(mipmaps[i-1].width() >= 1);
		mipmaps[i].first = SumMipMap<2>(mipmaps[i - 1].first);
		mipmaps[i].second = SumMipMap<2>(mipmaps[i - 1].second);
	}
	return mipmaps;
}

//----------------------------------------------------------------------------//
}
//----------------------------------------------------------------------------//
