/*
 * solver.hpp
 *
 *  Created on: Jan 29, 2012
 *      Author: david
 */

#ifndef GRAPHSEG_SPECTRAL_SOLVER_HPP_
#define GRAPHSEG_SPECTRAL_SOLVER_HPP_

#include "../Common.hpp"
#include <vector>

namespace graphseg {
namespace detail {

	std::vector<EigenComponent> solver_eigen(const Eigen::MatrixXf& A, unsigned int num_ev);

	std::vector<EigenComponent> solver_lapack(const Eigen::MatrixXf& A, unsigned int num_ev);

#ifdef USE_SOLVER_MAGMA
	std::vector<EigenComponent> solver_magma(const Eigen::MatrixXf& A, unsigned int num_ev);
#endif

	std::vector<EigenComponent> solver_arpackpp(const SparseMatrix& A, unsigned int num_ev);

#ifdef USE_SOLVER_IETL
	std::vector<EigenComponent> solver_ietl(const SparseMatrix& A, unsigned int num_ev);
#endif

}}

#endif
