
#include "PDS.hpp"
#include "Tools.hpp"
#include <density/ScalePyramid.hpp>
#include <iostream>

namespace pds
{

	namespace spds
	{

		constexpr float GAMMA = 0.38f;

		void spds_rec(
				std::vector<Eigen::Vector2f>& seeds,
				const std::vector<Eigen::MatrixXf>& mipmaps,
				int level, unsigned int x, unsigned int y)
		{
			static boost::uniform_real<float> rnd(0.0f, 1.0f);
			static boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > die(impl::Rnd(), rnd);

			float v = mipmaps[level](x, y);

//			std::cout << x << " " << y << " " << v << std::endl;

			if(v > 4.0f && level > 1) {
//				std::cout << "-> down" << std::endl;
				// go down
				spds_rec(seeds, mipmaps, level - 1, 2*x,     2*y    );
				spds_rec(seeds, mipmaps, level - 1, 2*x,     2*y + 1);
				spds_rec(seeds, mipmaps, level - 1, 2*x + 1, 2*y    );
				spds_rec(seeds, mipmaps, level - 1, 2*x + 1, 2*y + 1);
			}
			else {
				unsigned int num = impl::RandomRound(v);
//				std::cout << "sampling: num=" << num << std::endl;
				// compute weight of children
				const Eigen::MatrixXf& mmc = mipmaps[level-1];
				std::vector<float> w {
					mmc(2*x,     2*y    ),
					mmc(2*x,     2*y + 1),
					mmc(2*x + 1, 2*y    ),
					mmc(2*x + 1, 2*y + 1)
				};
//				std::cout << "sampling: weights=" << w[0] << ", " << w[1] << ", " << w[2] << ", " << w[3] << std::endl;
				// randomly select children based on weight and place points in cells
				for(unsigned int i : impl::RandomSample(w, num)) {
//					std::cout << i << std::endl;
					seeds.push_back(
						//impl::OptimalCellPoint(mipmaps[0], 1 << (level-1), 2*x + (i/2), 2*y + (i%2))
						//impl::RandomCellPoint(1 << (level-1), 2*x + (i/2), 2*y + (i%2), GAMMA)
						impl::ProbabilityCellPoint(mipmaps[0], 1 << (level-1), 2*x + (i/2), 2*y + (i%2), w[i])
					);
				}
			}
//			std::cout << "<- up" << std::endl;
		}

		std::vector<Eigen::Vector2f> spds_impl(const Eigen::MatrixXf& density)
		{
			// compute mipmaps
			std::vector<Eigen::MatrixXf> mipmaps = density::ComputeMipmaps(density, 1);
		#ifdef CREATE_DEBUG_IMAGES
			//DebugMipmap<2>(mipmaps, "mm");
			for(unsigned int i=0; i<mipmaps.size(); i++) {
				std::string tag = (boost::format("mm_%1d") % i).str();
				DebugShowMatrix(mipmaps[i], tag);
				DebugWriteMatrix(mipmaps[i], tag);
			}
		#endif
			// sample points
			std::vector<Eigen::Vector2f> seeds;
			spds_rec(seeds, mipmaps, mipmaps.size() - 1, 0, 0);
			// scale points with base constant
			impl::ScalePoints(seeds, 2.f);
			return seeds;
		}

		std::vector<Eigen::Vector2f> spds_impl_640x480(const Eigen::MatrixXf& density)
		{
			// compute mipmaps
			std::vector<Eigen::MatrixXf> mipmaps = density::ComputeMipmaps640x480(density);
		#ifdef CREATE_DEBUG_IMAGES
			DebugMipmap<5>(mipmaps, "mm640");
			for(unsigned int i=0; i<mipmaps.size(); i++) {
				std::string tag = (boost::format("mm640_%1d") % i).str();
				DebugShowMatrix(mipmaps[i], tag);
				DebugWriteMatrix(mipmaps[i], tag);
			}
		#endif
			// now create pixel seeds
			std::vector<Eigen::Vector2f> seeds;
			const unsigned int l0 = mipmaps.size() - 1;
			for(unsigned int y=0; y<mipmaps[l0].cols(); ++y) {
				for(unsigned int x=0; x<mipmaps[l0].rows(); x++) {
					spds_rec(seeds, mipmaps, l0, x, y);
				}
			}
			// scale points with base constant
			impl::ScalePoints(seeds, 5.f);
			return seeds;
		}
	}

	std::vector<Eigen::Vector2f> SimplifiedPDS(const Eigen::MatrixXf& density)
	{
		if(density.rows() == 640 && density.cols() == 480) {
			return spds::spds_impl_640x480(density);
		}
		else {
			return spds::spds_impl(density);
		}
	}

}
