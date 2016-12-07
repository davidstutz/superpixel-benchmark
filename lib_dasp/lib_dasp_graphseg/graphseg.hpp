/*
 * graphseg.hpp
 *
 *  Created on: Jan 29, 2012
 *      Author: david
 */

#ifndef GRAPHSEG_HPP
#define GRAPHSEG_HPP

#include "Common.hpp"

namespace graphseg
{

	namespace detail
	{
		extern bool cVerbose;
	}

	enum class SpectralMethod
	{
		Eigen
		,ArpackPP
		,Lapack
#ifdef USE_SOLVER_MAGMA
		,Magma
#endif
#ifdef USE_SOLVER_IETL
		,Ietl
#endif
	};

	/** Applies spectral graph theory fu to a weighted, undirected graph */
	SpectralGraph SolveSpectral(const SpectralGraph& graph, unsigned int num_ev, SpectralMethod method);

	/** Like SolveSpectral, but with fastest available solver */
	SpectralGraph SolveSpectral(const SpectralGraph& graph, unsigned int num_ev);

	/** Applies MCL graph segmentation to a weighted, undirected graph */
	SpectralGraph SolveMCL(const SpectralGraph& graph, float p, unsigned int iterations);

}

#endif
