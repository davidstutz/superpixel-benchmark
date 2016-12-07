/*
 * spectral.hpp
 *
 *  Created on: Jan 29, 2012
 *      Author: david
 */

#ifndef GRAPHSEG_SPECTRAL_SPECTRAL_HPP_
#define GRAPHSEG_SPECTRAL_SPECTRAL_HPP_

#include "../graphseg.hpp"
#include "spectral_impl.hpp"
#include "solver.hpp"

namespace graphseg { namespace detail {

	/** Computes n smallest eigenvalues/-vectors for a graph */
	template<typename Graph, typename EdgeWeightMap>
	std::vector<EigenComponent> solve(const Graph& graph, EdgeWeightMap edge_weights, unsigned int num_ev, SpectralMethod method)
	{
	using namespace std::placeholders;
		// pick one eigenvalue more because the first one is omitted
		switch(method) {
			case SpectralMethod::Eigen:
				return detail::solve_dense(graph, edge_weights, std::bind(&solver_eigen, _1, num_ev + 1));
			case SpectralMethod::Lapack:
				return detail::solve_dense(graph, edge_weights, std::bind(&solver_lapack, _1, num_ev + 1));
#ifdef USE_SOLVER_MAGMA
			case SpectralMethod::Magma:
				return detail::solve_dense(graph, edge_weights, std::bind(&solver_magma, _1, num_ev + 1));
#endif
			case SpectralMethod::ArpackPP:
				return detail::solve_sparse(graph, edge_weights, std::bind(&solver_arpackpp, _1, num_ev + 1));
#ifdef USE_SOLVER_IETL
			case SpectralMethod::Ietl:
				return detail::solve_sparse(graph, edge_weights, std::bind(&solver_ietl, _1, num_ev + 1));
#endif
			default:
				std::cerr << "ERROR: Solver not available!" << std::endl;
				return {};
		}
	}

	/** Applies graphseg graph theory fu to a weighted undirected graph */
	template<typename Graph, typename EdgeWeightMap>
	Graph graphseg_spectral(const Graph& graph, EdgeWeightMap edge_weights, unsigned int num_ev, SpectralMethod method)
	{
		std::vector<EigenComponent> solution = solve(graph, edge_weights, num_ev, method);
		Eigen::VectorXf weights = ev_to_graph_weights(graph, solution);
		Graph result = graph;
		// FIXME proper edge indexing
		unsigned int eid_index = 0;
		for(auto eid : as_range(boost::edges(result))) {
			result[eid] = weights[eid_index];
			eid_index++;
		}
		return result;
	}

}}

#endif
