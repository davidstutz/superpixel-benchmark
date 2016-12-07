#ifndef INCLUDED_PDS_TOOLS_HPP
#define INCLUDED_PDS_TOOLS_HPP

#include <boost/random.hpp>

namespace pds
{

	namespace impl
	{
		inline boost::mt19937& Rnd()
		{
			static boost::mt19937 rnd;
			return rnd;
		}

		inline void RndSeed(unsigned int x)
		{
			Rnd().seed(x);
		}
		
		/** Selects a random point in the tree node using uniform distribution */ 
		inline Eigen::Vector2f RandomCellPoint(int scale, int x, int y, float gamma)
		{
			float sf = static_cast<float>(scale);
			float xf = static_cast<float>(x);
			float yf = static_cast<float>(y);
			boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > delta(
					Rnd(), boost::uniform_real<float>(0.5f-gamma, 0.5f+gamma));
			return Eigen::Vector2f(sf*(xf + delta()), sf*(yf + delta()));
		}

		/** Selects the point in the tree node with highest probability */ 
		inline Eigen::Vector2f OptimalCellPoint(const Eigen::MatrixXf& m0, int scale, int x, int y)
		{
			x *= scale;
			y *= scale;
			const auto& b = m0.block(x, y, scale, scale);
			unsigned int best_j=scale/2, best_i=scale/2;
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
			return Eigen::Vector2f(x + best_j, y + best_i);
		}

		/** Randomly selects a point in the given tree node by considering probabilities 
		 * Runtime: O(S*S + log(S*S))
		 */
		inline Eigen::Vector2f ProbabilityCellPoint(const Eigen::MatrixXf& m0, int scale, int x, int y)
		{
			x *= scale;
			y *= scale;
			const auto& b = m0.block(x, y, scale, scale);
			// build cdf
			std::vector<float> cdf(scale*scale);
			for(int i=0; i<scale; ++i) {
				for(int j=0; j<scale; ++j) {
					float v = b(j,i);
					int q = scale*i + j;
					if(q > 0) {
						cdf[q] = cdf[q-1] + v;
					}
					else {
						cdf[q] = v;
					}
				}
			}
			// sample in cdf
			boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > rnd(
					Rnd(), boost::uniform_real<float>(0.0f, cdf.back()));
			float v = rnd();
			// find sample
			auto it = std::lower_bound(cdf.begin(), cdf.end(), v);
			int pos = std::distance(cdf.begin(), it);
			return Eigen::Vector2f(x + pos%scale, y + pos/scale);
		}

		/** Randomly selects a point in the given tree node by considering probabilities
		 * Assumes that cdf_sum is the probability sum in the given tree node
		 * Runtime: O(S*S/2)
		 */
		inline Eigen::Vector2f ProbabilityCellPoint(const Eigen::MatrixXf& m0, int scale, int x, int y, float cdf_sum)
		{
			// sample in cdf
			boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > rnd(
					Rnd(), boost::uniform_real<float>(0.0f, cdf_sum));
			float v = rnd();
			// find sample
			x *= scale;
			y *= scale;
			const auto& b = m0.block(x, y, scale, scale);
			for(int i=0; i<scale; ++i) {
				for(int j=0; j<scale; ++j) {
					v -= b(j,i);
					if(v <= 0.0f) {
						return Eigen::Vector2f(x + j, y + i);
					}
				}
			}
			// should never be here
			assert(false);
			return Eigen::Vector2f(x + scale/2, y + scale/2);
		}

		inline void ScalePoints(std::vector<Eigen::Vector2f>& pnts, float scale)
		{
			for(Eigen::Vector2f& u : pnts) {
				u *= scale;
			}
		}

		/** Randomly rounds a float up or down s.t. the expected value is the given value */
		inline unsigned int RandomRound(float x)
		{
			if(x <= 0.0f) {
				return 0;
			}
			float a = std::floor(x);
			float r = x - a;
			boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > uniform01(
					Rnd(), boost::uniform_real<float>(0.0f,1.0f));
			return a + (uniform01() >= r ? 0.0f : 1.0f);
		}

		inline std::vector<unsigned int> RandomSample(const std::vector<float>& v, unsigned int num)
		{
			std::vector<float> a(v.size());
			std::partial_sum(v.begin(), v.end(), a.begin());
//			std::copy(a.begin(), a.end(), std::ostream_iterator<float>(std::cout, ", "));
			float ws = a.back();
			boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > rndv(
					Rnd(), boost::uniform_real<float>(0.0f, ws));
			std::vector<unsigned int> idx(num);
			std::generate(idx.begin(), idx.end(),
				[&rndv,&a]() -> unsigned int {
					float x = rndv();
					auto it = std::lower_bound(a.begin(), a.end(), x);
					if(it == a.end()) {
						return a.size() - 1;
					}
					else {
						return it - a.begin();
					}
				});
			return idx;
		}

	}

}

#endif
