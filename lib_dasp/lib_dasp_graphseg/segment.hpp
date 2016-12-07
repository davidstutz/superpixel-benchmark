/*
 * segment.hpp
 *
 *  Created on: Jan 29, 2012
 *      Author: david
 */

#ifndef GRAPHSEG_SEGMENT_HPP
#define GRAPHSEG_SEGMENT_HPP

#include "Common.hpp"

namespace graphseg
{

	namespace detail
	{
		extern bool cVerbose;
	}

	enum class SpectralMethod {
		Eigen,
		Lapack,
		Magma,
		ArpackPP,
		Ietl
	};

	/** Applies spectral graph theory fu to a weighted, undirected graph */
	SpectralGraph SolveSpectral(const SpectralGraph& graph, unsigned int num_ev, SpectralMethod method);

	/** Applies MCL graph segmentation to a weighted, undirected graph */
	SpectralGraph SolveMCL(const SpectralGraph& graph, float p, unsigned int iterations);

}

#endif
