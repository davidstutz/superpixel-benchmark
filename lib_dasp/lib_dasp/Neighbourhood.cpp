/*
 * Neighbourhood.cpp
 *
 *  Created on: May 18, 2012
 *      Author: david
 */

#include "Neighbourhood.hpp"
#include "Metric.hpp"
#include <Slimage/Convert.hpp>
#include <boost/graph/copy.hpp>
#include <iostream>

namespace dasp
{

DaspGraph CreateDaspNeighbourhoodGraph(const Superpixels& superpixels)
{
	NeighbourhoodGraph gnb = CreateNeighborhoodGraph(superpixels, NeighborGraphSettings::NoCut());
	DaspGraph result;
	boost::copy_graph(gnb, result,
		boost::vertex_copy(
			[&superpixels,&gnb,&result](UndirectedWeightedGraph::vertex_descriptor src, DaspGraph::vertex_descriptor dst) {
				result[dst] = superpixels.cluster[src].center;
			}
		)
		.edge_copy(
			[&gnb,&result](UndirectedWeightedGraph::edge_descriptor src, DaspGraph::edge_descriptor dst) {
				result[dst] = 1.0f;
			}
		)
	);
	return result;
}

DaspGraph CreateDaspGraph(const Superpixels& superpixels, const UndirectedWeightedGraph& weighted_graph)
{
	DaspGraph result;
	boost::copy_graph(weighted_graph, result,
		boost::vertex_copy(
			[&superpixels,&weighted_graph,&result](UndirectedWeightedGraph::vertex_descriptor src, DaspGraph::vertex_descriptor dst) {
				result[dst] = superpixels.cluster[src].center;
			}
		)
		.edge_copy(
			[&weighted_graph,&result](UndirectedWeightedGraph::edge_descriptor src, DaspGraph::edge_descriptor dst) {
				result[dst] = weighted_graph[src];
			}
		)
	);
	return result;
}

DaspGraph ConvertToSimilarityGraph(const DaspGraph& source, const float sigma) {
	DaspGraph result;
	boost::copy_graph(source, result,
			boost::edge_copy([&source,&result,sigma](typename DaspGraph::edge_descriptor src, DaspGraph::edge_descriptor dst) {
				const float d = source[src];
				const float s = std::exp(-d/sigma);
				result[dst] = s;
	}));
	return result;
}

std::vector<unsigned int> ComputeAllBorderPixels(const Superpixels& superpixels)
{
	slimage::Image1i labels = superpixels.ComputeLabels();
	std::set<unsigned int> u;
	int face_neighbours[] = { -1, +1, -labels.width(), +labels.width() };
	for(unsigned int y=1; y<labels.height()-1; y++) {
		for(unsigned int x=1; x<labels.width()-1; x++) {
			int q = labels.index(x,y);
			unsigned int lc = labels[q];
			for(unsigned int i=0; i<4; i++) {
				if(labels[q + face_neighbours[i]] != lc) {
					u.insert(q);
				}
			}
		}
	}
	return std::vector<unsigned int>(u.begin(), u.end());
}

//std::vector<unsigned int> ComputeBorderPixels(const std::vector<unsigned int>& pixel_ids, unsigned int desired_neighbour_cid, const slimage::Image1i& labels)
//{
//	const int w = static_cast<int>(labels.width());
//	const int h = static_cast<int>(labels.height());
//	const int d[4] = { -1, +1, -w, +w };
//	std::vector<unsigned int> border;
//	for(unsigned int pid : pixel_ids) {
//		int x = pid % w;
//		int y = pid / w;
//		if(1 <= x && x+1 <= w && 1 <= y && y+1 <= h) {
//			for(int i=0; i<4; i++) {
//				int label = labels[pid + d[i]];
//				if(label == static_cast<int>(desired_neighbour_cid)) {
//					border.push_back(pid);
//				}
//			}
//		}
//	}
//	return border;
//}

struct BorderPixel
{
	unsigned int pixel_id;
	unsigned int label;
};

std::vector<BorderPixel> ComputeBorderLabels(unsigned int cid, const Superpixels& spc, const slimage::Image1i& labels)
{
	const int w = static_cast<int>(labels.width());
	const int h = static_cast<int>(labels.height());
	const int d[4] = { -1, +1, -w, +w };
	std::vector<BorderPixel> border;
	for(unsigned int pid : spc.cluster[cid].pixel_ids) {
		int x = pid % w;
		int y = pid / w;
		if(1 <= x && x+1 < w && 1 <= y && y+1 < h) {
			for(int i=0; i<4; i++) {
				int label = labels[pid + d[i]];
				if(label != cid && label != -1) {
					border.push_back(BorderPixel{pid, label});
				}
			}
		}
	}
	return border;
}

std::vector<std::vector<BorderPixel>> ComputeBorderLabels(const Superpixels& spc)
{
	slimage::Image1i labels = spc.ComputeLabels();
	std::vector<std::vector<BorderPixel>> border_pixels(spc.cluster.size());
	for(unsigned int cid=0; cid<spc.cluster.size(); cid++) {
		border_pixels[cid] = ComputeBorderLabels(cid, spc, labels);
	}
	return border_pixels;
}

std::vector<unsigned int> FindCommonBorder(const std::vector<std::vector<BorderPixel>>& border, unsigned int cid, unsigned cjd) {
	const std::vector<BorderPixel>& bi = border[cid];
	const std::vector<BorderPixel>& bj = border[cjd];
	std::vector<unsigned int> common;
	common.reserve(bi.size() + bj.size());
	for(const BorderPixel& q : bi) {
		if(q.label == cjd) {
			common.push_back(q.pixel_id);
		}
	}
	for(const BorderPixel& q : bi) {
		if(q.label == cjd) {
			common.push_back(q.pixel_id);
		}
	}
	return common;
}

NeighbourhoodGraph CreateNeighborhoodGraph(const Superpixels& superpixels, NeighborGraphSettings settings)
{
	// create one node for each superpixel
	NeighbourhoodGraph neighbourhood_graph(superpixels.clusterCount());
	// compute superpixel borders
	std::vector<std::vector<BorderPixel> > border = ComputeBorderLabels(superpixels);
	// connect superpixels
	const float spatial_distance_threshold = settings.spatial_distance_mult_threshold * superpixels.opt.base_radius;
	const float pixel_distance_mult_threshold = settings.pixel_distance_mult_threshold;
	for(unsigned int i=0; i<superpixels.cluster.size(); i++) {
		for(unsigned int j=i+1; j<superpixels.cluster.size(); j++) {
			const Point& c_i = superpixels.cluster[i].center;
			const Point& c_j = superpixels.cluster[j].center;
			// early test if the two superpixels are even near to each other
			if(settings.cut_by_spatial) {
				// spatial distance
				float d = (c_i.position - c_j.position).norm();
				// only test if distance is smaller than threshold
				if(d > spatial_distance_threshold) {
					continue;
				}
			}
			else {
				// pixel distance on camera image plane
				float d = std::sqrt(static_cast<float>(metric::PixelDistanceSquared(c_i,c_j)));
				// only test if pixel distance is smaller then C * pixel_radius
				float r = std::max(c_i.cluster_radius_px, c_j.cluster_radius_px);
				if(d > pixel_distance_mult_threshold * r) {
					continue;
				}
			}
			// compute intersection of border pixels
			std::vector<unsigned int> common_border = FindCommonBorder(border, i, j);
			// test if superpixels have a common border
			unsigned int common_border_size = common_border.size();
			if(common_border_size < settings.min_abs_border_overlap) {
				continue;
			}
			float p = static_cast<float>(common_border_size) / static_cast<float>(std::min(border[i].size(), border[j].size()));
			if(p < settings.min_border_overlap) {
				continue;
			}
			// add edge
			NeighbourhoodGraph::edge_descriptor eid;
			bool ok;
			boost::tie(eid,ok) = boost::add_edge(i, j, neighbourhood_graph); // FIXME correctly convert superpixel_id to vertex descriptor
			assert(ok);
			neighbourhood_graph[eid].num_border_pixels = common_border_size;
			neighbourhood_graph[eid].border_pixel_ids = common_border;
		}
	}
	return neighbourhood_graph;
}

}
