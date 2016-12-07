
#include "PDS.hpp"
#include "Tools.hpp"
#include <density/ScalePyramid.hpp>
#include <density/PointDensity.hpp>
#include <iostream>

namespace pds
{

	namespace dds
	{
		constexpr float GAMMA = 0.38f;

		template<unsigned int Q>
		void FindSeedsDeltaMipmap_Walk(std::vector<Eigen::Vector2f>& seeds,
			const std::vector<Eigen::MatrixXf>& mipmaps_value,
			const std::vector<Eigen::MatrixXf>& mipmaps_delta,
			const std::vector<Eigen::MatrixXf>& mipmaps_delta_abs,
			int level, unsigned int x, unsigned int y)
		{
			constexpr float BREAK_SMOOTH = 2.0f;

			static boost::uniform_real<float> rnd(0.0f, 1.0f);
			static boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > die(impl::Rnd(), rnd);

			const Eigen::MatrixXf& mm_v = mipmaps_value[level];
			const Eigen::MatrixXf& mm_s = mipmaps_delta[level];
			const Eigen::MatrixXf& mm_a = mipmaps_delta_abs[level];

			const float v_val = mm_v(x, y);
			const float v_sum = mm_s(x, y);
			const float v_sum_abs = std::abs(v_sum);
			const float v_abs = mm_a(x, y);

			if(
				level > 0 // do not access mipmap 0!
				&& v_abs > 1.0f
				&& v_val > BREAK_SMOOTH*BREAK_SMOOTH
				//|| (std::abs(v_sum) - v_abs) / (std::abs(v_sum) + v_abs)
			) {
				// go down
				FindSeedsDeltaMipmap_Walk<Q>(seeds, mipmaps_value, mipmaps_delta, mipmaps_delta_abs, level - 1, 2*x,     2*y    );
				FindSeedsDeltaMipmap_Walk<Q>(seeds, mipmaps_value, mipmaps_delta, mipmaps_delta_abs, level - 1, 2*x,     2*y + 1);
				FindSeedsDeltaMipmap_Walk<Q>(seeds, mipmaps_value, mipmaps_delta, mipmaps_delta_abs, level - 1, 2*x + 1, 2*y    );
				FindSeedsDeltaMipmap_Walk<Q>(seeds, mipmaps_value, mipmaps_delta, mipmaps_delta_abs, level - 1, 2*x + 1, 2*y + 1);
			}
			else {
				// const float brok = std::abs(v_sum_abs - v_abs) / (v_sum_abs + v_abs);

				// std::cout << level << "," << x << "," << y << ": t=" << v_val << ", r=" << v_sum << ", |r|=" << v_sum_abs << ", a=" << v_abs << "; q=" << brok << std::endl;

				// if(brok > 0.5f) {
				// 	return;
				// }
				if(die() < v_sum_abs)
				{
					const Eigen::MatrixXf& mmd0 = mipmaps_delta[0];

					const unsigned int x0 = std::min<unsigned int>(mmd0.rows(), (x << level));
					const unsigned int y0 = std::min<unsigned int>(mmd0.cols(), (y << level));
					const unsigned int x1 = std::min<unsigned int>(mmd0.rows(), ((x+1) << level));
					const unsigned int y1 = std::min<unsigned int>(mmd0.cols(), ((y+1) << level));

		#ifdef CREATE_DEBUG_IMAGES
					slimage::Image3ub debug = slimage::Ref<unsigned char, 3>(sDebugImages["seeds_delta"]);
					auto color = (v_sum > 0.0f ? slimage::Pixel3ub{{255,0,0}} : slimage::Pixel3ub{{0,255,255}});
					slimage::PaintLine(debug, Q*x0, Q*y0, Q*x1, Q*y0, color);
					slimage::PaintLine(debug, Q*x0, Q*y1, Q*x1, Q*y1, color);
					slimage::PaintLine(debug, Q*x0, Q*y0, Q*x0, Q*y1, color);
					slimage::PaintLine(debug, Q*x1, Q*y0, Q*x1, Q*y1, color);
		#endif

					if(v_sum > 0.0f) {
						// find coordinate in cell where delta density is minimal
						const auto& b = mmd0.block(x0, y0, 1 << level, 1 << level);
						unsigned int best_j=-1, best_i=-1;
						float best_val = -1000000.0f;
						for(unsigned int i=0; i<b.cols(); ++i) {
							for(unsigned int j=0; j<b.rows(); ++j) {
								float val = b(j,i);
								if(val > best_val) {
									best_j = j;
									best_i = i;
									best_val = val;
								}
							}
						}
						// add seed to middle of cell
						if(best_i != -1 && best_j != -1) {
							seeds.push_back(impl::RandomCellPoint(Q, x0 + best_j, y0 + best_i, GAMMA));
		#ifdef CREATE_DEBUG_IMAGES
							for(unsigned int i=0; i<2; i++) {
								for(unsigned int j=0; j<2; j++) {
									debug(sx+j, sy+i) = slimage::Pixel3ub{{255,0,0}};
								}
							}
		#endif
						}
					}
					else {
						if(seeds.empty()) {
							return;
						}
						// find best in cell
						float best_val = +1000000.0f;
						std::size_t best_index = -1;
						for(std::size_t i=0; i<seeds.size(); i++) {
							const Eigen::Vector2f& s = seeds[i];
							int sx = s[0] / Q;
							int sy = s[1] / Q;
							// do not remove fixed seed points
							if(sx < x0 || x1 <= sx || sy < y0 || y1 <= sy) {
								continue;
							}
							float val = mmd0(sx, sy);
							if(val < best_val) {
								best_index = i;
								best_val = val;
							}
						}
						// delete nearest seed
						if(best_index != -1) {
		#ifdef CREATE_DEBUG_IMAGES
							unsigned int sx = seeds[best_index][0];
							unsigned int sy = seeds[best_index][1];
							for(unsigned int i=0; i<2; i++) {
								for(unsigned int j=0; j<2; j++) {
									debug(sx+j, sy+i) = slimage::Pixel3ub{{0,255,255}};
								}
							}
		#endif
							seeds.erase(seeds.begin() + best_index);
						}
					}
				}
			}
		}

		std::vector<Eigen::Vector2f> FindSeedsDelta(const std::vector<Eigen::Vector2f>& old_seeds, const Eigen::MatrixXf& density_old, const Eigen::MatrixXf& density_new)
		{
			// difference
			Eigen::MatrixXf density_delta = density_new - density_old;
			// compute mipmaps
			std::vector<Eigen::MatrixXf> mm_v = density::ComputeMipmaps640x480(density_new);
			std::vector<Eigen::MatrixXf> mm_dv = density::ComputeMipmaps640x480(density_delta);
			std::vector<Eigen::MatrixXf> mm_da = density::ComputeMipmaps640x480(density_delta.cwiseAbs());
		#ifdef CREATE_DEBUG_IMAGES
			DebugMipmap<5>(mm_v, "mm_v");
			DebugMipmapDelta<5>(mm_dv, "mm_dv");
			DebugMipmap<5>(mm_da, "mm_da");
		#endif
			// we need to add and delete points!
			std::vector<Eigen::Vector2f> seeds = old_seeds;
			const unsigned int l0 = mm_dv.size() - 1;
			for(unsigned int y=0; y<mm_dv[l0].cols(); ++y) {
				for(unsigned int x=0; x<mm_dv[l0].rows(); x++) {
					FindSeedsDeltaMipmap_Walk<5>(seeds, mm_v, mm_dv, mm_da, l0, x, y);
				}
			}
			return seeds;
		}
	}

	std::vector<Eigen::Vector2f> DeltaDensitySamplingOld(const Eigen::MatrixXf& density_new, const std::vector<Eigen::Vector2f>& old_seeds)
	{
	#ifdef CREATE_DEBUG_IMAGES
		slimage::Image3ub debug(points.width(), points.height(), {{0,0,0}});
		sDebugImages["seeds_delta"] = slimage::Ptr(debug);
	#endif
		// compute old density
		Eigen::MatrixXf density_old = density::PointDensity(old_seeds, density_new);
		// use function
		return dds::FindSeedsDelta(old_seeds, density_old, density_new);
	}

}
