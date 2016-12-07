
#include "PDS.hpp"
#include "Tools.hpp"
#include <density/ScalePyramid.hpp>

namespace pds
{

	namespace mlfs
	{

		void WriteMipmap(std::vector<Eigen::MatrixXf>& mipmaps, int level, int x, int y, float d)
		{
			// mipmaps[level](x,y) += d;
			for(int k=level,s=1; k>=0; k--,s*=2) {
				Eigen::MatrixXf& mm = mipmaps[k];
				const float ds = d / static_cast<float>(s*s);
				for(int i=0; i<s; i++) {
					for(int j=0; j<s; j++) {
						mm(x+j,y+i) += ds;
					}
				}
			}
		}

		void FindSeedsDepthMipmapFS_Walk(
				std::vector<Eigen::Vector2f>& seeds,
				std::vector<Eigen::MatrixXf>& mipmaps,
				int level, unsigned int x, unsigned int y)
		{
			Eigen::MatrixXf& mm = mipmaps[level];

			// compute density by multiplying percentage with parent total
			float v = mm(x, y);

			if(level == 0 || v <= 1.5f) {
				if(v >= 0.5f) {
					seeds.push_back(
						impl::RandomCellPoint(1 << level, x, y, 0.38f));
					// reduce density
					v -= 1.0f;
				}
				// distribute remaining density to neighbours
				// mm(x+1,y  ) += 7.0f / 16.0f * v;
				// mm(x-1,y+1) += 3.0f / 16.0f * v;
				// mm(x  ,y+1) += 5.0f / 16.0f * v;
				// mm(x+1,y+1) += 1.0f / 16.0f * v;
				// with range test *sigh*
				float q = 0.0f;
				bool xm1ok = (0 < x);
				bool xp1ok = (x+1 < mm.rows());
				bool yp1ok = (y+1 < mm.cols());
				if(xp1ok) 			q += 7.0f;
				if(yp1ok) {
					if(xm1ok) 		q += 3.0f;			
									q += 5.0f;
					if(xp1ok) 		q += 1.0f;
				}
		//		if(q > 0) {
					float scl = v / q;
					if(xp1ok) 		WriteMipmap(mipmaps, level, x+1, y  , 7.0f*scl);
					if(yp1ok) {
						if(xm1ok) 	WriteMipmap(mipmaps, level, x-1, y+1, 3.0f*scl);
									WriteMipmap(mipmaps, level, x  , y+1, 5.0f*scl);
						if(xp1ok) 	WriteMipmap(mipmaps, level, x+1, y+1, 1.0f*scl);
					}
		//		}
			}
			else {
				// go down
				FindSeedsDepthMipmapFS_Walk(seeds, mipmaps, level - 1, 2*x,     2*y    );
				FindSeedsDepthMipmapFS_Walk(seeds, mipmaps, level - 1, 2*x,     2*y + 1);
				FindSeedsDepthMipmapFS_Walk(seeds, mipmaps, level - 1, 2*x + 1, 2*y    );
				FindSeedsDepthMipmapFS_Walk(seeds, mipmaps, level - 1, 2*x + 1, 2*y + 1);
			}
		}

		std::vector<Eigen::Vector2f> FindSeedsDepthMipmapFS(const Eigen::MatrixXf& density)
		{
			// compute mipmaps
			std::vector<Eigen::MatrixXf> mipmaps = density::ComputeMipmaps(density, 1);
		// #ifdef CREATE_DEBUG_IMAGES
		// 	DebugMipmap<2>(mipmaps, "mmfs");
		// #endif
			// now create pixel seeds
			std::vector<Eigen::Vector2f> seeds;
			FindSeedsDepthMipmapFS_Walk( seeds, mipmaps, mipmaps.size() - 1, 0, 0);
			impl::ScalePoints(seeds, 2.f);
			return seeds;
		}

		std::vector<Eigen::Vector2f> FindSeedsDepthMipmapFS640(const Eigen::MatrixXf& density)
		{
			// compute mipmaps
			std::vector<Eigen::MatrixXf> mipmaps = density::ComputeMipmaps640x480(density);
		#ifdef CREATE_DEBUG_IMAGES
			DebugMipmap<5>(mipmaps, "mmfs640");
		#endif
			// now create pixel seeds
			std::vector<Eigen::Vector2f> seeds;
			const unsigned int l0 = mipmaps.size() - 1;
			for(unsigned int y=0; y<mipmaps[l0].cols(); ++y) {
				for(unsigned int x=0; x<mipmaps[l0].rows(); x++) {
					FindSeedsDepthMipmapFS_Walk(seeds, mipmaps, l0, x, y);
				}
			}
			impl::ScalePoints(seeds, 5.f);
			return seeds;
		}

	}

	std::vector<Eigen::Vector2f> FloydSteinbergMultiLayer(const Eigen::MatrixXf& density)
	{
		if(density.rows() == 640 && density.cols() == 480) {
			return mlfs::FindSeedsDepthMipmapFS640(density);
		}
		else {
			return mlfs::FindSeedsDepthMipmapFS(density);
		}
	}

}
