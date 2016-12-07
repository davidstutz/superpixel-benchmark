/*
 * arpack.cpp
 *
 *  Created on: Okt 20, 2012
 *      Author: david
 */

#include "solver.hpp"
#include <arpack++/arlssym.h>
#include <iostream>
#include <vector>
#include <cmath>

void MemoryOverflow()
{
	std::cerr << "ArpackError: MEMORY_OVERFLOW" << std::endl;
	throw ArpackError(ArpackError::MEMORY_OVERFLOW);
}

void ArpackError::Set(ArpackError::ErrorCode code, char const* msg)
{
//	ArpackError::code = code;
	std::cerr << "ArpackError: code=" << code << ", msg=" << std::string(msg) << std::endl;
}

namespace graphseg { namespace detail {

std::vector<EigenComponent> solver_arpackpp(const SparseMatrix& A, unsigned int num_ev)
{
	typedef float Real;
	const unsigned int n = A.dim;
	const auto& entries = A.entries;

	// define ARPACK matrix (see p. 119 in ARPACK++ manual)
#ifdef SPECTRAL_VERBOSE
	std::cout << "Sparse Solver: defining matrix" << std::endl;
#endif
	int nnz = entries.size();
	std::vector<Real> nzval(nnz);
	std::vector<int> irow(nnz);
	std::vector<int> pcol;
	pcol.reserve(n + 1);
	std::ofstream ofs("/tmp/sparse.tsv");
	// Assumes that each column has at least one non-zero element.
	int current_col = -1;
	for(unsigned int i=0; i<entries.size(); i++) {
		const SparseEntry& e = entries[i];
		ofs << e.i << "\t" << e.j << "\t" << e.weight << std::endl;
		nzval[i] = e.weight;
		irow[i] = e.i;
		if(e.j == current_col + 1) {
			pcol.push_back(i);
			current_col++;
		}
	}
	ofs.close();
	pcol.push_back(nnz);
//	// check CRC
//	{
//		int i, j, k;
//
//		// Checking if pcol is in ascending order.
//
//		i = 0;
//		while ((i!=n)&&(pcol[i]<=pcol[i+1])) i++;
//		if (i!=n) {
//		  std::cout << "Error 1" << std::endl;
//		  std::cout << i << std::endl;
//		  throw 0;
//		}
//
//		// Checking if irow components are in order and within bounds.
//
//		for (i=0; i!=n; i++) {
//		j = pcol[i];
//		k = pcol[i+1]-1;
//		if (j<=k) {
//		  if ((irow[j]<i)||(irow[k]>=n)) {
//			  std::cout << "Error 2" << std::endl;
//			  std::cout << i << std::endl;
//			  throw 0;
//		  }
//		  while ((j!=k)&&(irow[j]<irow[j+1])) j++;
//		  if (j!=k) {
//			  std::cout << "Error 3" << std::endl;
//			  std::cout << i << ", " << irow[j] << " -> " << irow[j+1] << std::endl;
//			  std::cout << j << " -> " << k << std::endl;
//			  throw 0;
//		  }
//		}
//		}
//	}
	ARluSymMatrix<Real> mat(n, nnz, nzval.data(), irow.data(), pcol.data());

	// solve ARPACK problem (see p. 82 in ARPACK++ manual)
#ifdef SPECTRAL_VERBOSE
	std::cout << "Sparse Solver: solving ..." << std::flush;
#endif
	num_ev = std::min<unsigned int>(num_ev, n);
	if(static_cast<float>(num_ev)/static_cast<float>(n) > 0.1f) {
		std::cout << "Warning: Using sparse eigensolver, but trying to get a huge number of eigenvectors!" << std::endl;
	}
	ARluSymStdEig<Real> solv(num_ev, mat, "SM");
std::vector<Real> v_ew(num_ev);
	std::vector<Real> v_ev(num_ev * n);
	Real* p_ew = v_ew.data();
	Real* p_ev = v_ev.data();
	solv.EigenValVectors(p_ev, p_ew, false);
#ifdef SPECTRAL_VERBOSE
	std::cout << " finished." << std::endl;
	std::cout << "Sparse Solver: collecting results" << std::endl;
#endif
	std::vector<EigenComponent> solution(num_ev);
	for(unsigned int i=0; i<num_ev; i++) {
		EigenComponent& cmp = solution[i];
		cmp.eigenvalue = p_ew[i];
#ifdef SPECTRAL_VERBOSE
		std::cout << "Eigenvalue " << i << ": " << cmp.eigenvalue << std::endl;
#endif
		cmp.eigenvector = Eigen::VectorXf(n);
		for(unsigned int j=0; j<n; j++) {
			cmp.eigenvector[j] = p_ev[i*n + j];
		}
	}
#ifdef SPECTRAL_VERBOSE
	std::cout << "Sparse Solver: returning" << std::endl;
#endif
	return solution;
}

}}
