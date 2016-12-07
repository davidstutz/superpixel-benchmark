/*
 * Graph.hpp
 *
 *  Created on: May 18, 2012
 *      Author: david
 */

#ifndef DASP_GRAPH_HPP_
#define DASP_GRAPH_HPP_

#include "Point.hpp"
#include "as_range.hpp"
#include "graphseg.hpp"
#include <boost/graph/adjacency_list.hpp>
#include <Eigen/Dense>

namespace dasp
{
	struct NeighbourhoodProperties
	{
		unsigned int num_border_pixels;
		std::vector<unsigned int> border_pixel_ids;
	};

	/** Undirected graph */
	typedef boost::adjacency_list<
		boost::vecS, boost::vecS, boost::undirectedS,
		boost::no_property,
		NeighbourhoodProperties
	> NeighbourhoodGraph;

	/** Undirected, weighted (type=float) graph */
	typedef graphseg::SpectralGraph UndirectedWeightedGraph;

	/** Weighted graph of superpoints */
	typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
		Point,
		float
	> DaspGraph;

}

#endif
