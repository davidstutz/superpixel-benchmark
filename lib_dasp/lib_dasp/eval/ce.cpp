#include "eval.hpp"
#include <Superpixels.hpp>
#include <iostream>

namespace dasp {
namespace eval {

template<typename FPX, typename FSEG, typename D>
float CompressionError(const Partition& partition, FPX fpx, FSEG fseg, D metric)
{
	// assert(partition.numSegments() == cluster_features.size());
	float total = 0.0f;
	unsigned int num = 0;
	for(unsigned int i=0; i<partition.numSegments(); i++) {
		const auto& fi = fseg(i);
		num += partition.segmentSize(i);
		// segment pixel contribution
		for(unsigned int j : partition.segmentPixelIds(i)) {
			float d = metric(fpx(j), fi);
			total += d*d;
		}
	}
	return std::sqrt(total / static_cast<float>(num));
}

float CompressionErrorColor(const Superpixels& sp)
{
	return CompressionError(
		sp.ComputePartition(),
		[&sp](unsigned int j) { return sp.points[j].color; },
		[&sp](unsigned int i) { return sp.cluster[i].center.color; },
		[](const Eigen::Vector3f& a, const Eigen::Vector3f& b) { return (a-b).norm(); }
	);
}

float CompressionErrorDepth(const Superpixels& sp)
{
	return CompressionError(
		sp.ComputePartition(),
		[&sp](unsigned int j) { return sp.points[j].depth(); },
		[&sp](unsigned int i) { return sp.cluster[i].center.depth(); },
		[](float a, float b) { return std::abs(a-b); }
	);
}

float CompressionErrorPosition(const Superpixels& sp)
{
	return CompressionError(
		sp.ComputePartition(),
		[&sp](unsigned int j) { return sp.points[j].position; },
		[&sp](unsigned int i) { return sp.cluster[i].center.position; },
		[](const Eigen::Vector3f& a, const Eigen::Vector3f& b) { return (a-b).norm(); }
	);
}

float CompressionErrorNormal(const Superpixels& sp)
{
	return CompressionError(
		sp.ComputePartition(),
		[&sp](unsigned int j) { return sp.points[j].normal; },
		[&sp](unsigned int i) { return sp.cluster[i].center.normal; },
		[](const Eigen::Vector3f& a, const Eigen::Vector3f& b) {
			// protect acos from slightly wrong dot product results
			float h = a.dot(b);
			h = std::min(1.0f, std::max(0.0f, h));
			return 180.0f / boost::math::constants::pi<float>() * std::acos(h);
		}
	);
}

}
}
