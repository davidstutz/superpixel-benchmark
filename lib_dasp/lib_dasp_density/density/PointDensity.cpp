#include "PointDensity.hpp"
#include <Slimage/Slimage.hpp>
#define SLIMAGE_IO_OPENCV
#include <Slimage/IO.hpp>
#include <iostream>
#include <fstream>

namespace density
{

	Eigen::MatrixXf LoadDensity(const std::string& filename)
	{
		std::string stem = filename.substr(filename.size()-4, 4);
		bool is_image = (stem == ".png" || stem == ".jpg");
		if(is_image) {
			std::cout << "LOADING IMAGE" << std::endl;
			slimage::ImagePtr ptr = slimage::Load(filename);
			if(!ptr) {
				std::cerr << "Could not load image!" << std::endl;
				throw 0;
			}
			if(slimage::HasType<unsigned char, 3>(ptr)) {
				slimage::Image3ub img = slimage::Ref<unsigned char, 3>(ptr);
				Eigen::MatrixXf mat(img.width(), img.height());
				for(int y=0; y<mat.cols(); y++) {
					for(int x=0; x<mat.rows(); x++) {
						slimage::Pixel3ub p = img(x,y);
						mat(x,y) = static_cast<float>((int)p[0] + (int)p[1] + (int)p[2])/3.0f/255.0f;
					}
				}
				return mat;
			}
			if(slimage::HasType<unsigned char, 1>(ptr)) {
				slimage::Image1ub img = slimage::Ref<unsigned char, 1>(ptr);
				Eigen::MatrixXf mat(img.width(), img.height());
				for(int y=0; y<mat.cols(); y++) {
					for(int x=0; x<mat.rows(); x++) {
						mat(x,y) = static_cast<float>(img(x,y))/255.0f;
					}
				}
				return mat;
			}
		}
		else {
			// load matrix
			std::ifstream ifs(filename);
			if(!ifs.is_open()) {
				std::cerr << "Error opening file '" << filename << "'" << std::endl;
			}
			std::string line;
			std::vector<std::vector<float>> data;
			while(getline(ifs, line)) {
				std::istringstream ss(line);
				std::vector<float> q;
				while(!ss.eof()) {
					float v;
					ss >> v;
					q.push_back(v);
				}
				data.push_back(q);
			}
			Eigen::MatrixXf mat(data.front().size(),data.size());
			for(int y=0; y<mat.cols(); y++) {
				for(int x=0; x<mat.rows(); x++) {
					mat(x,y) = data[y][x];
				}
			}
			return mat;
		}
	}

	void SaveDensity(const std::string& filename, const Eigen::MatrixXf& mat)
	{
		bool is_image =
			filename.substr(filename.size()-3, 3) == ".png" ||
			filename.substr(filename.size()-3, 3) == ".jpg";
		if(is_image) {

		}
		else {
			const int rows = mat.rows();
			std::ofstream ofs(filename);
			for(int y=0; y<mat.cols(); y++) {
				for(int x=0; x<rows; x++) {
					ofs << mat(x,y);
					if(x+1 != rows) {
						ofs << "\t";
					}
					else {
						ofs << std::endl;
					}
				}
			}
		}
	}

	template<typename T, typename Fx, typename Fy>
	Eigen::MatrixXf PointDensityImpl(const std::vector<T>& seeds, const Eigen::MatrixXf& target, Fx fx, Fy fy)
	{
		// radius of box in which to average cluster density
		constexpr int RHO_R = 3;
		constexpr float cMagicSoftener = 0.5f; // 0.62f;
		constexpr float cRangeLossMult = 1.001f;
		constexpr float cRange = 1.3886f;
		const int rows = target.rows();
		const int cols = target.cols();
		// range of kernel s.t. 99.9% of mass is covered
		Eigen::MatrixXf density = Eigen::MatrixXf::Zero(rows, cols);
		for(const T& s : seeds) {
			const int sx = std::round(fx(s));
			const int sy = std::round(fy(s));
			// compute point density as average over a box
			float rho_sum = 0.0f;
			unsigned int rho_num = 0;
			for(int i=-RHO_R; i<=+RHO_R; ++i) {
				for(int j=-RHO_R; j<=+RHO_R; ++j) {
					const int sxj = sx + j;
					const int syi = sy + i;
					if( 0 <= sxj && sxj < rows &&
						0 <= syi && syi < cols)
					{
						rho_sum += target(sxj, syi);
						rho_num ++;
					}
				}
			}
			if(rho_sum == 0.0f || rho_num == 0) {
				continue;
			}
			const float rho = rho_sum / static_cast<float>(rho_num);
			// seed corresponds to a kernel at position (x,y)
			// with sigma = 1/sqrt(pi*rho)
			// i.e. 1/sigma^2 = pi*rho
			// factor pi is already compensated in kernel
			const float sxf = fx(s);
			const float syf = fy(s);
			const float rho_soft = cMagicSoftener * rho;
			// kernel influence range
			const int R = static_cast<int>(std::ceil(cRange / std::sqrt(rho_soft)));
			const int xmin = std::max<int>(sx - R, 0);
			const int xmax = std::min<int>(sx + R, int(rows) - 1);
			const int ymin = std::max<int>(sy - R, 0);
			const int ymax = std::min<int>(sy + R, int(cols) - 1);
			for(int yi=ymin; yi<=ymax; yi++) {
				for(int xi=xmin; xi<=xmax; xi++) {
					float dx = static_cast<float>(xi) - sxf;
					float dy = static_cast<float>(yi) - syf;
					float d2 = dx*dx + dy*dy;
					float delta = rho_soft * KernelSquare(rho_soft*d2);
					density(xi, yi) += delta;
				}
			}
		}
		{
			const float* psrc = target.data();
			const float* psrc_end = psrc + rows*cols;
			float* pdst = density.data();
			for(; psrc!=psrc_end; ++psrc, ++pdst) {
				if(*psrc == 0.0f) {
					*pdst = 0.0f;
				}
			}
		}
		return cRangeLossMult * density;
	}

	Eigen::MatrixXf PointDensity(const std::vector<Eigen::Vector2f>& seeds, const Eigen::MatrixXf& target)
	{
		return PointDensityImpl(seeds, target,
				[](const Eigen::Vector2f& s) { return s[0]; },
				[](const Eigen::Vector2f& s) { return s[1]; }
		);
	}

}
