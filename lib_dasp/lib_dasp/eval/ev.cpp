#include "eval.hpp"
#include <Superpixels.hpp>
#include <iostream>

namespace dasp {
namespace eval {

template<typename FPX, typename FSEG, typename F, typename D>
float ExplainedVariation(const Partition& partition, FPX fpx, FSEG fseg, F foverall, D metric)
{
	// assert(partition.numSegments() == cluster_features.size());
	float segment_total = 0.0f;
	float pixel_total = 0.0f;
	for(unsigned int i=0; i<partition.numSegments(); i++) {
		const F& fi = fseg(i);
		// segment mean contribution
		float dS = metric(fi, foverall);
		segment_total += partition.segmentSize(i) * dS*dS;
		// segment pixel contribution
		for(unsigned int j : partition.segmentPixelIds(i)) {
			float d = metric(fpx(j), foverall);
			pixel_total += d*d;
		}
	}
	return segment_total / pixel_total;
}

float ExplainedVariationColor(const Superpixels& sp)
{
	Eigen::Vector3f mean_color = Eigen::Vector3f::Zero();
	unsigned int num_valid = 0;
	for(const Point& p : sp.points) {
		if(p.is_valid) {
			mean_color += p.color;
			num_valid++;
		}
	}
	mean_color /= static_cast<float>(num_valid);
	return ExplainedVariation(
		sp.ComputePartition(),
		[&sp](unsigned int j) { return sp.points[j].color; },
		[&sp](unsigned int i) { return sp.cluster[i].center.color; },
		mean_color,
		[](const Eigen::Vector3f& a, const Eigen::Vector3f& b) { return (a-b).norm(); }
	);
}

float ExplainedVariationDepth(const Superpixels& sp)
{
	float mean_depth = 0.0f;
	unsigned int num_valid = 0;
	for(const Point& p : sp.points) {
		if(p.is_valid) {
			mean_depth += p.depth();
			num_valid++;
		}
	}
	mean_depth /= static_cast<float>(num_valid);
	return ExplainedVariation(
		sp.ComputePartition(),
		[&sp](unsigned int j) { return sp.points[j].depth(); },
		[&sp](unsigned int i) { return sp.cluster[i].center.depth(); },
		mean_depth,
		[](float a, float b) { return std::abs(a-b); }
	);
}

float ExplainedVariationPosition(const Superpixels& sp)
{
	Eigen::Vector3f mean_pos = Eigen::Vector3f::Zero();
	unsigned int num_valid = 0;
	for(const Point& p : sp.points) {
		if(p.is_valid) {
			mean_pos += p.position;
			num_valid++;
		}
	}
	mean_pos /= static_cast<float>(num_valid);
	return ExplainedVariation(
		sp.ComputePartition(),
		[&sp](unsigned int j) { return sp.points[j].position; },
		[&sp](unsigned int i) { return sp.cluster[i].center.position; },
		mean_pos,
		[](const Eigen::Vector3f& a, const Eigen::Vector3f& b) { return (a-b).norm(); }
	);
}

float ExplainedVariationNormal(const Superpixels& sp)
{
	Eigen::Vector3f mean_normal = Eigen::Vector3f::Zero();
	unsigned int num_valid = 0;
	for(const Point& p : sp.points) {
		if(p.is_valid) {
			mean_normal += p.normal;
			num_valid++;
		}
	}
	mean_normal /= mean_normal.norm();

	// auto part = sp.ComputePartition();
	// std::cout << mean_normal.transpose() << std::endl;
	// for(unsigned int i=0; i<part.numSegments(); i++) {
	// 	std::cout << mean_normal.transpose() << std::endl;
	// 	std::cout << sp.cluster[i].center.normal.transpose() << " -> " << 1.0f - mean_normal.dot(sp.cluster[i].center.normal.transpose()) << std::endl;
	// 	float total = 0.0f;
	// 	for(unsigned int j : part.segmentPixelIds(i)) {
	// 		float d = 1.0f - mean_normal.dot(sp.points[j].normal);
	// 		total += d;
	// 		std::cout << "\t" << sp.points[j].normal.transpose() << " -> " << d << std::endl;
	// 	}
	// 	total /= static_cast<float>(part.segmentSize(i));
	// 	std::cout << "total=" << total << std::endl;
	// }

	return ExplainedVariation(
		sp.ComputePartition(),
		[&sp](unsigned int j) { return sp.points[j].normal; },
		[&sp](unsigned int i) { return sp.cluster[i].center.normal; },
		mean_normal,
		[](const Eigen::Vector3f& a, const Eigen::Vector3f& b) { return 1.0f - a.dot(b); }
	);
}

}
}
