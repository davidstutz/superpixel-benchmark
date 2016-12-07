/*
 * Common.hpp
 *
 *  Created on: Dec 30, 2012
 *      Author: david
 */

#ifndef DASP_SPECTRAL_COMMON_HPP
#define DASP_SPECTRAL_COMMON_HPP

#include <boost/graph/adjacency_list.hpp>
#include <Eigen/Dense>
#include <vector>

namespace graphseg
{
	namespace detail
	{
		struct EigenComponent
		{
			float eigenvalue;
			Eigen::VectorXf eigenvector;
		};

		struct SparseEntry
		{
			unsigned int i; // row index
			unsigned int j; // column index
			float weight; // value
		};

		/** A symmetric lower triangle sparse matrix
		 * Assertion: i >= j
		 * Assertion: sorted by j then by i
		 * Entries are sorted s.t. i
		 */
		struct SparseMatrix
		{
			unsigned int dim;
			std::vector<SparseEntry> entries;
		};
	}
	
	/** A simple weighted undirected graph */
	typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
			boost::no_property,
			float
	> SpectralGraph;

}

#endif
