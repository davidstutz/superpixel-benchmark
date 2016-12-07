#include "solver.hpp"
#include <Eigen/Dense>
#ifdef SPECTRAL_VERBOSE
#	include <iostream>
#endif

namespace graphseg { namespace detail {

std::vector<EigenComponent> solver_eigen(const Eigen::MatrixXf& A, unsigned int num_ev)
{
	// solve eigensystem
	Eigen::SelfAdjointEigenSolver<Eigen::MatrixXf> solver;
	solver.compute(A);
#ifdef SPECTRAL_VERBOSE
	std::cout << "DEBUG: solver_eigen_dense_gev says " << solver.info() << std::endl;
#endif
	// collect eigenvectors and eigenvalues
	std::vector<EigenComponent> solution(std::min<std::size_t>(A.rows(), num_ev));
	for(unsigned int i=0; i<solution.size(); i++) {
		solution[i].eigenvalue = solver.eigenvalues()[i];
#ifdef SPECTRAL_VERBOSE
		std::cout << "DEBUG:\teigenvalue #" << i << "=" << solution[i].eigenvalue << std::endl;
#endif
		solution[i].eigenvector = solver.eigenvectors().col(i);
	}
	return solution;
}

}}
