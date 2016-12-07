#include "Smooth.hpp"
#include "PointDensity.hpp"

namespace density
{

	Eigen::MatrixXf DensityAdaptiveSmoothBase(const Eigen::MatrixXf& src)
	{
		const int width = src.rows();
		const int height = src.cols();
		Eigen::MatrixXf result(width, height);
		for(int i=0; i<height; i++) {
			for(int j=0; j<width; j++) {
				const float rho = src(j,i);
				if(rho == 0.0f) {
					result(j,i) = 0.0f;
					continue;
				}
				const int R = static_cast<int>(std::ceil(KernelRange / std::sqrt(rho)));
				const int xmin = std::max<int>(j - R, 0);
				const int xmax = std::min<int>(j + R, width - 1);
				const int ymin = std::max<int>(i - R, 0);
				const int ymax = std::min<int>(i + R, height - 1);
				float a_sum = 0.0f;
				float w_sum = 0.0f;
				for(int ki=ymin; ki<=ymax; ki++) {
					for(int kj=xmin; kj<=xmax; kj++) {
						int dx = kj - j;
						int dy = ki - i;
						int d2 = dx*dx + dy*dy;
						float w = KernelSquare(rho*static_cast<float>(d2));
						a_sum += w * src(kj,ki);
						w_sum += w;
					}
				}
				result(j,i) = a_sum / w_sum;
			}
		}
		return result;
	}

	Eigen::MatrixXf DensityAdaptiveSmoothSeparated(const Eigen::MatrixXf& src)
	{
		const int width = src.rows();
		const int height = src.cols();
		Eigen::MatrixXf result1(width, height);
		for(int i=0; i<height; i++) {
			for(int j=0; j<width; j++) {
				const float rho = src(j,i);
				if(rho == 0.0f) {
					result1(j,i) = 0.0f;
					continue;
				}
				const int R = static_cast<int>(std::ceil(KernelRange / std::sqrt(rho)));
				const int xmin = std::max<int>(j - R, 0);
				const int xmax = std::min<int>(j + R, width - 1);
				float a_sum = 0.0f;
				float w_sum = 0.0f;
				for(int kj=xmin; kj<=xmax; kj++) {
					int dx = kj - j;
					float w = KernelSquare(rho*static_cast<float>(dx*dx));
					float v = src(kj,i);
					if(v > 0.0f) {
						a_sum += w * v;
						w_sum += w;
					}
				}
				result1(j,i) = a_sum / w_sum;
			}
		}
		Eigen::MatrixXf result2(width, height);
		for(int i=0; i<height; i++) {
			for(int j=0; j<width; j++) {
				const float rho = src(j,i);
				if(rho == 0.0f) {
					result2(j,i) = 0.0f;
					continue;
				}
				const int R = static_cast<int>(std::ceil(KernelRange / std::sqrt(rho)));
				const int ymin = std::max<int>(i - R, 0);
				const int ymax = std::min<int>(i + R, height - 1);
				float a_sum = 0.0f;
				float w_sum = 0.0f;
				for(int ki=ymin; ki<=ymax; ki++) {
					int dy = ki - i;
					float w = KernelSquare(rho*static_cast<float>(dy*dy));
					float v = result1(j,ki);
					if(v > 0.0f) {
						a_sum += w * v;
						w_sum += w;
					}
				}
				result2(j,i) = a_sum / w_sum;
			}
		}
		return result2;
	}

	Eigen::MatrixXf DensityAdaptiveSmooth(const Eigen::MatrixXf& src)
	{
		//return DensityAdaptiveSmoothBase(src);
		return DensityAdaptiveSmoothSeparated(src);
	}

}
