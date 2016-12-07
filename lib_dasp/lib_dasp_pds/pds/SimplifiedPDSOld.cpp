
#include "PDS.hpp"
#include "Tools.hpp"
#include <density/ScalePyramid.hpp>

namespace pds
{

	namespace spds_old
	{

		void FindSeedsDepthMipmap_Walk(
				std::vector<Eigen::Vector2f>& seeds,
				const std::vector<Eigen::MatrixXf>& mipmaps,
				int level, unsigned int x, unsigned int y)
		{
			static boost::uniform_real<float> rnd(0.0f, 1.0f);
			static boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > die(impl::Rnd(), rnd);

			float v = mipmaps[level](x, y);

			if(v > 1.0f && level > 0) { // do not access mipmap 0!
				// go down
				FindSeedsDepthMipmap_Walk(seeds, mipmaps, level - 1, 2*x,     2*y    );
				FindSeedsDepthMipmap_Walk(seeds, mipmaps, level - 1, 2*x,     2*y + 1);
				FindSeedsDepthMipmap_Walk(seeds, mipmaps, level - 1, 2*x + 1, 2*y    );
				FindSeedsDepthMipmap_Walk(seeds, mipmaps, level - 1, 2*x + 1, 2*y + 1);
			}
			else {
				if(die() <= v) {
					seeds.push_back(
						impl::RandomCellPoint(1 << level, x, y, 0.38f));
				}
			}
		}

		std::vector<Eigen::Vector2f> FindSeedsDepthMipmap(const Eigen::MatrixXf& density)
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
			FindSeedsDepthMipmap_Walk(seeds, mipmaps, mipmaps.size() - 1, 0, 0);
			// scale points with base constant
			impl::ScalePoints(seeds, 2.f);
			return seeds;
		}

		std::vector<Eigen::Vector2f> FindSeedsDepthMipmap640(const Eigen::MatrixXf& density)
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
					FindSeedsDepthMipmap_Walk(seeds, mipmaps, l0, x, y);
				}
			}
			// scale points with base constant
			impl::ScalePoints(seeds, 5.f);
			return seeds;
		}
	}

	std::vector<Eigen::Vector2f> SimplifiedPDSOld(const Eigen::MatrixXf& density)
	{
		if(density.rows() == 640 && density.cols() == 480) {
			return spds_old::FindSeedsDepthMipmap640(density);
		}
		else {
			return spds_old::FindSeedsDepthMipmap(density);
		}
	}

}
