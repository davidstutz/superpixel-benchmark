#ifndef INCLUDED_PDS_PDS_HPP
#define INCLUDED_PDS_PDS_HPP

#include <Eigen/Dense>
#include <vector>

namespace pds
{

	std::vector<Eigen::Vector2f> Random(const Eigen::MatrixXf& density);

	std::vector<Eigen::Vector2f> RectGrid(const Eigen::MatrixXf& density);

	std::vector<Eigen::Vector2f> HexGrid(const Eigen::MatrixXf& density);

	std::vector<Eigen::Vector2f> SimplifiedPDS(const Eigen::MatrixXf& density);

	std::vector<Eigen::Vector2f> SimplifiedPDSOld(const Eigen::MatrixXf& density);

	std::vector<Eigen::Vector2f> FloydSteinberg(const Eigen::MatrixXf& density);

	std::vector<Eigen::Vector2f> FloydSteinbergExpo(const Eigen::MatrixXf& density);

	std::vector<Eigen::Vector2f> FloydSteinbergMultiLayer(const Eigen::MatrixXf& density);

	std::vector<Eigen::Vector2f> Fattal(const Eigen::MatrixXf& density);

	std::vector<Eigen::Vector2f> DeltaDensitySampling(
		const Eigen::MatrixXf& density,
		const std::vector<Eigen::Vector2f>& old_seeds,
		std::vector<int>* seed_origin = 0
	);

	std::vector<Eigen::Vector2f> DeltaDensitySamplingOld(const Eigen::MatrixXf& density, const std::vector<Eigen::Vector2f>& old_seeds);

	inline std::vector<Eigen::Vector2f> PoissonDiscSampling(const std::string& name, const Eigen::MatrixXf& density, const std::vector<Eigen::Vector2f>& old_points = std::vector<Eigen::Vector2f>())
	{
		if(name == "rnd") return Random(density);
		if(name == "rect") return RectGrid(density);
		if(name == "spds") return SimplifiedPDS(density);
		if(name == "fattal") return Fattal(density);
		if(name == "dds") return DeltaDensitySampling(density, old_points);
		throw 0;
	}

}

#endif
