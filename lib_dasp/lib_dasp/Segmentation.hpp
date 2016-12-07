/*
 * Segmentation.hpp
 *
 *  Created on: Mar 26, 2012
 *      Author: david
 */

#ifndef SEGMENTATION_HPP_
#define SEGMENTATION_HPP_

#include "Superpixels.hpp"
#include "Graph.hpp"
#include <Labeling.hpp>
#include <graphseg.hpp>
#include <Slimage/Slimage.hpp>
#include <Eigen/Dense>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/copy.hpp>
#include <vector>
#include <iostream>

namespace dasp
{

/** Computes 0/1 segment boundaries from superpixel segment labels using edge detection */
slimage::Image1ub CreateBoundaryImageFromLabels(const Superpixels& clusters, const graphseg::GraphLabeling& cluster_labels);

/** Creates colors for labeling */
std::vector<slimage::Pixel3ub> ComputeSegmentColors(const Superpixels& clusters, const graphseg::GraphLabeling& labeling);

/** Creates an image where each superpixel is colored with the corresponding label color */
slimage::Image3ub CreateLabelImage(const Superpixels& clusters, const graphseg::GraphLabeling& labeling, const std::vector<slimage::Pixel3ub>& colors);

/** Performs spectral graph segmentation */
template<typename SuperpixelGraph, typename WeightMap>
UndirectedWeightedGraph SpectralSegmentation(const SuperpixelGraph& graph, WeightMap weights)
{
	// create graph for spectral solving
	graphseg::SpectralGraph spectral;
	boost::copy_graph(graph, spectral,
			boost::edge_copy(
				[&spectral,&weights](typename SuperpixelGraph::edge_descriptor src, typename graphseg::SpectralGraph::edge_descriptor dst) {
					spectral[dst] = boost::get(weights, src);
				}
	));
	// do spectral graph foo
	graphseg::SpectralGraph solved = graphseg::SolveSpectral(spectral, 24);
//	graphseg::SpectralGraph solved = graphseg::SolveMCL(spectral, 1.41f, 50);
	// create superpixel neighbourhood graph with edge strength
	UndirectedWeightedGraph result;
	boost::copy_graph(solved, result,
			boost::edge_copy(
				[&solved,&result](typename graphseg::SpectralGraph::edge_descriptor src, typename UndirectedWeightedGraph::edge_descriptor dst) {
					result[dst] = solved[src];
				}
	));
	return result;
}

}

#endif
