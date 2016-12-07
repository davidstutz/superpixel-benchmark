/*
 * Superpixels.hpp
 *
 *  Created on: Jan 26, 2012
 *      Author: david
 */

#ifndef SUPERPIXELS_HPP_
#define SUPERPIXELS_HPP_

#include "Point.hpp"
#include "Tools.hpp"
#include "Seed.hpp"
#include <Slimage/Slimage.hpp>
#include <Slimage/Parallel.h>
#include <eigen3/Eigen/Dense>
#include <boost/graph/adjacency_list.hpp>
#include <vector>
#include <cmath>

namespace dasp
{
	extern std::map<std::string,slimage::ImagePtr> sDebugImages;

	struct ClusterGroupInfo
	{
		Histogram<float> hist_thickness;
		Histogram<float> hist_area_quotient;
		Histogram<float> hist_coverage_error;
	};

	void SetRandomNumberSeed(unsigned int seed);

	struct Partition
	{
		std::vector<std::vector<unsigned int>> segments;

		unsigned int numSegments() const {
			return segments.size();
		}

		unsigned int segmentSize(unsigned int i) const {
			return segments[i].size();
		}

		const std::vector<unsigned int>& segmentPixelIds(unsigned int i) const {
			return segments[i];
		}
	};

	class Superpixels
	{
	public:
		slimage::ThreadingOptions threadopt;

		Parameters opt;

		slimage::Image3ub color_raw;

		ImagePoints points;

		Eigen::MatrixXf density;

		Eigen::MatrixXf saliency;

		std::vector<Cluster> cluster;

		std::vector<Seed> seeds_previous;
		std::vector<Seed> seeds;

		std::size_t clusterCount() const {
			return cluster.size();
		}

		unsigned int width() const {
			return points.width();
		}

		unsigned int height() const {
			return points.height();
		}

		Superpixels();

		std::vector<Seed> getClusterCentersAsSeeds() const;

		std::vector<Eigen::Vector2f> getClusterCentersAsPoints() const;

		void CreatePoints(const slimage::Image3ub& image, const slimage::Image1ui16& depth, const slimage::Image3f& normals=slimage::Image3f());

//		/** Find super pixel clusters */
//		void ComputeSuperpixels(const slimage::Image1f& edges);

		std::vector<int> ComputePixelLabels() const;

		slimage::Image1i ComputeLabels() const;

		Partition ComputePartition() const;

		void ComputeSuperpixels(const std::vector<Seed>& seeds);

		void ConquerEnclaves();

		void ConquerMiniEnclaves();

		std::vector<Seed> FindSeeds();

		slimage::Image1f ComputeEdges();

		void ImproveSeeds(std::vector<Seed>& seeds, const slimage::Image1f& edges);

		void CreateClusters(const std::vector<Seed>& seeds);

		void PurgeInvalidClusters();

		void MoveClusters();

		/**
		 * Signature of F :
		 * void F(unsigned int cid, const dasp::Cluster& c, unsigned int pid, const dasp::Point& p)
		 */
		template<typename F>
		void ForPixelClusters(F f) const {
			for(unsigned int i=0; i<cluster.size(); i++) {
				const Cluster& c = cluster[i];
				for(unsigned int p : c.pixel_ids) {
					f(i, c, p, points[p]);
				}
			}
		}

		template<typename F>
		void ForClustersNoReturn(F f) {
			for(unsigned int i=0; i<cluster.size(); i++) {
				f(cluster[i]);
			}
		}

		template<typename F>
		auto ForClusters(F f) -> std::vector<decltype(f(cluster[0]))> {
			std::vector<decltype(f(cluster[0]))> data(cluster.size());
			for(unsigned int i=0; i<cluster.size(); i++) {
				data[i] = f(cluster[i]);
			}
			return data;
		}

		template<typename F>
		void ForClusterCentersNoReturn(F f) {
			for(unsigned int i=0; i<cluster.size(); i++) {
				f(cluster[i].center);
			}
		}

		template<typename F>
		auto ForClusterCenters(F f) -> std::vector<decltype(f(cluster[0].center))> {
			std::vector<decltype(f(cluster[0].center))> data(cluster.size());
			for(unsigned int i=0; i<cluster.size(); i++) {
				data[i] = f(cluster[i].center);
			}
			return data;
		}

		void ComputeExt() {
			return ForClustersNoReturn([this](Cluster& c) { c.ComputeExt(points, opt); });
		}

		ClusterGroupInfo ComputeClusterGroupInfo(unsigned int n, float max_thick);

		Eigen::Vector3f ColorToRGB(const Eigen::Vector3f& c) const;

		Eigen::Vector3f ColorFromRGB(const Eigen::Vector3f& c) const;

	};

	Superpixels ComputeSuperpixels(const slimage::Image3ub& color, const slimage::Image1ui16& depth, const Parameters& opt);

	void ComputeSuperpixelsIncremental(Superpixels& clustering, const slimage::Image3ub& color, const slimage::Image1ui16& depth);


}

#endif
