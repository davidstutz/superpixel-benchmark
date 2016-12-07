/*
 * Spectral.cpp
 *
 *  Created on: May 19, 2012
 *      Author: david
 */

#include "spectral/spectral.hpp"
#include "Common.hpp"

namespace graphseg
{
	SpectralGraph SolveSpectral(const SpectralGraph& graph, unsigned int num_ev)
	{
		return SolveSpectral(graph, num_ev, SpectralMethod::Eigen);
	}

	SpectralGraph SolveSpectral(const SpectralGraph& graph, unsigned int num_ev, SpectralMethod method)
	{
		return detail::graphseg_spectral(graph, boost::get(boost::edge_bundle, graph), num_ev, method);
	}

	SpectralGraph SolveMCL(const SpectralGraph& graph, float p, unsigned int iterations)
	{
		typedef Eigen::MatrixXf Mat;
		std::cout << "input graph has " << boost::num_edges(graph) << " edges" << std::endl;
		using namespace detail;
		unsigned int dim = boost::num_vertices(graph);
		// get adjacency matrix
		std::cout << "adjacency matrix (size=" << dim << ")" << std::endl;
		Mat W = Mat::Zero(dim, dim);
		for(auto eid : as_range(boost::edges(graph))) {
			unsigned int ea = boost::source(eid, graph);
			unsigned int eb = boost::target(eid, graph);
			float ew = graph[eid];
			W(ea, eb) = ew;
			W(eb, ea) = ew;
		}
		// perform MCL
		for(int i=0; i<iterations; i++) {
			std::cout << "MCL " << i << std::endl;
			Mat M = (W*W).array().pow(p).matrix();
			// column normalization
			for(int i=0; i<dim; i++) {
				const float total = M.col(i).sum();
				if(total > 0.0f) {
					const float scl = 1.0f / total;
					M.col(i) *= scl;
				}
			}
			W = M;
		}
		// create new graph with new edges
		SpectralGraph result(dim);
		for(int y=0; y<dim; y++) {
			for(int x=0; x<dim; x++) {
				float w = W(x,y);
				if(w > 0.001f) {
					bool ok;
					SpectralGraph::edge_descriptor eid;
					boost::tie(eid,ok) = boost::add_edge(x, y, result);
					result[eid] = w;
				}
			}
		}
		std::cout << "result graph has " << boost::num_edges(result) << " edges" << std::endl;
		return result;
	}

}
