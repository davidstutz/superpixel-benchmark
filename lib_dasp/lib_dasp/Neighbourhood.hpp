/*
 * Neighbourhood.hpp
 *
 *  Created on: May 18, 2012
 *      Author: david
 */

#ifndef DASP_NEIGHBOURHOOD_HPP_
#define DASP_NEIGHBOURHOOD_HPP_

#include "Superpixels.hpp"
#include "Graph.hpp"
#include <Slimage/Slimage.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/copy.hpp>
#include <Eigen/Dense>
#include <vector>

namespace dasp
{

	/** Computes points which lie on the border between segments
	 * @param list of point indices
	 */
	std::vector<unsigned int> ComputeAllBorderPixels(const Superpixels& superpixels);

	struct NeighborGraphSettings
	{
		bool cut_by_spatial;
		float spatial_distance_mult_threshold;
		float pixel_distance_mult_threshold;
		float min_border_overlap;
		unsigned min_abs_border_overlap;

		static NeighborGraphSettings SpatialCut() {
			return NeighborGraphSettings{true, 5.0f, 4.0f, 0.0f, 1};
		}
		static NeighborGraphSettings NoCut() {
			return NeighborGraphSettings{false, 5.0f, 4.0f, 0.0f, 1};
		}
	};

	/** Computes the superpixel neighborhood graph
	 * Superpixels are neighbors if they share border pixels.
	 */
	NeighbourhoodGraph CreateNeighborhoodGraph(const Superpixels& superpixels,
		NeighborGraphSettings settings=NeighborGraphSettings::SpatialCut());

	/** Computes edge weights for a superpixel graph using the given metric
	 * Metric : Point x Point -> float (should be lightweight)
	 */
	template<typename Graph, typename Metric>
	UndirectedWeightedGraph ComputeEdgeWeights(const Superpixels& superpixels, const Graph& graph, const Metric& metric)
	{
		UndirectedWeightedGraph result;
		boost::copy_graph(graph, result,
			boost::edge_copy([&superpixels, &graph, &result, &metric](typename Graph::edge_descriptor src, UndirectedWeightedGraph::edge_descriptor dst) {
				const unsigned int ea = boost::source(src, graph);
				const unsigned int eb = boost::target(src, graph);
				const float w = metric(superpixels.cluster[ea].center, superpixels.cluster[eb].center);
				result[dst] = w;
			}));
		return result;
	}

	template<typename Metric>
	UndirectedWeightedGraph ComputeEdgeWeightsFromMetricWeighted(const Superpixels& superpixels, const NeighbourhoodGraph& graph, const Metric& metric)
	{
		std::vector<unsigned int> cluster_border_length_px(boost::num_vertices(graph));
		for(auto eid : as_range(boost::edges(graph))) {
			const unsigned int ea = boost::source(eid, graph);
			const unsigned int eb = boost::target(eid, graph);
			const unsigned int num = graph[eid].num_border_pixels;
			cluster_border_length_px[ea] += num;
			cluster_border_length_px[eb] += num;
		}
		UndirectedWeightedGraph result;
		boost::copy_graph(graph, result,
			boost::edge_copy([&superpixels, &graph, &result, &metric, &cluster_border_length_px](typename NeighbourhoodGraph::edge_descriptor src, UndirectedWeightedGraph::edge_descriptor dst) {
				const unsigned int ea = boost::source(src, graph);
				const unsigned int eb = boost::target(src, graph);
				const float w = static_cast<float>(graph[src].num_border_pixels)
					/ static_cast<float>(cluster_border_length_px[ea] + cluster_border_length_px[eb]);
				const float d = metric(ea, eb);
				result[dst] = 12.0f * w * d;
			}));
		return result;
	}

	/** Computes a superpixel graph */
	DaspGraph CreateDaspNeighbourhoodGraph(const Superpixels& superpixels);

	/** Computes a weighted superpixel graph with superpixel values as vertex properties
	 */
	DaspGraph CreateDaspGraph(const Superpixels& superpixels, const UndirectedWeightedGraph& weighted_graph);

	/** Copies graph and changes edge weights to exp(-w/sigma) */
	DaspGraph ConvertToSimilarityGraph(const DaspGraph& source, const float sigma);

	/** Computes a border image
	 * All border pixels for each edge are set to the edge weight in the image.
	 * Border pixels are given in index form x + y*w where w must be the same given as parameter.
	 * @param graph a boost graph
	 * @param weights an weight property map of type float
	 * @param border_pixels an edge property map of type std::vector<unsigned int>
	 * @param return image with painted border pixels
	 */
	template<typename Graph, typename WeightMap, typename BorderPixelMap>
	void PlotBorderPixels(Eigen::MatrixXf& mat, const Graph& graph, WeightMap weights, BorderPixelMap border_pixels)
	{
		float* p = mat.data();
		for(auto eid : as_range(boost::edges(graph))) {
			float v = weights[eid];
			for(unsigned int pid : boost::get(border_pixels, eid)) {
				p[pid] = v;
			}
		}
	}

}

#endif
