/*
 * ietl.cpp
 *
 *  Created on: Jan 19, 2012
 *      Author: david
 */

#include "../Common.hpp"
#include "../as_range.hpp"
#include <boost/graph/adjacency_list.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <ietl/interface/ublas.h>
#include <ietl/vectorspace.h>
#include <ietl/lanczos.h>
#include <boost/random.hpp>
#include <boost/limits.hpp>
#include <limits>
#include <iostream>
#include <vector>
#include <cmath>

namespace graphseg { namespace detail {

std::vector<EigenComponent> solver_ietl(const SparseMatrix& A, unsigned int num_ev)
{
	typedef boost::numeric::ublas::symmetric_matrix<
		double, boost::numeric::ublas::lower> Matrix; 
	typedef boost::numeric::ublas::vector<double> Vector;

	int N = A.dim;
	Matrix mat(N, N);
	for(int i=0;i<N;i++)
		for(int j=0;j<=i;j++)
			mat(i,j) = 0;   
	for(auto& e : A.entries)
		mat(e.i,e.j) = e.weight;

	typedef ietl::vectorspace<Vector> Vecspace;
	typedef boost::lagged_fibonacci607 Gen;  

	Vecspace vec(N);
	Gen mygen;
	ietl::lanczos<Matrix,Vecspace> lanczos(mat,vec);

	// Creation of an iteration object:  
	int max_iter = 10*N;  
	double rel_tol = 50*std::numeric_limits<double>::epsilon();
	double abs_tol = 0.00001f;// std::pow(std::numeric_limits<double>::epsilon(),2./3);
	std::cout << "Computation of 2 lowest converged eigenvalues\n\n";
	std::cout << "-----------------------------------\n\n";
	int n_lowest_eigenval = num_ev;
	std::vector<double> eigen;
	std::vector<double> err;
	std::vector<int> multiplicity;  
	ietl::lanczos_iteration_nlowest<double> iter(max_iter, n_lowest_eigenval, rel_tol, abs_tol);
	try{
		lanczos.calculate_eigenvalues(iter,mygen);
		//lanczos.more_eigenvalues(iter); 
		eigen = lanczos.eigenvalues();
		err = lanczos.errors();
		multiplicity = lanczos.multiplicities();
		std::cout<<"number of iterations: "<<iter.iterations()<<"\n";
	}
	catch (std::runtime_error& e) {
		std::cout << e.what() << "\n";
	} 

  // Printing eigenvalues with error & multiplicities:  
	std::cout << "#        eigenvalue            error         multiplicity\n";  
	std::cout.precision(10);
	for (int i=0;i<eigen.size();++i) 
		std::cout << i << "\t" << eigen[i] << "\t" << err[i] << "\t" << multiplicity[i] << "\n";

	// call of eigenvectors function follows:   
	std::cout << "\nEigen vectors computations for lowest eigenvalues:\n\n";  
	auto ew_begin = eigen.begin();
	while(*ew_begin <= 0.0f) ew_begin ++;
	auto ew_end = ew_begin + num_ev;
	std::vector<Vector> eigenvectors; // for storing the eigen vectors. 
	ietl::Info<double> info; // (m1, m2, ma, eigenvalue, residualm, status).

	try {
		lanczos.eigenvectors(ew_begin,ew_end,std::back_inserter(eigenvectors),info,mygen); 
	}
	catch (std::runtime_error& e) {
		std::cout << e.what() << "\n";
	}

	// std::cout << "Printing eigenvectors:\n\n"; 
	// for(std::vector<Vector>::iterator it = eigenvectors.begin();it!=eigenvectors.end();it++){
	// 	std::copy((it)->begin(),(it)->end(),std::ostream_iterator<double>(std::cout,", "));
	// 	std::cout << "\n\n";
	// }
	std::cout << " Information about the eigenvector computations:\n\n";
	for(int i = 0; i < info.size(); i++) {
		std::cout << " m1(" << i+1 << "): " << info.m1(i) << ", m2(" << i+1 << "): "
			<< info.m2(i) << ", ma(" << i+1 << "): " << info.ma(i) << " eigenvalue("
			<< i+1 << "): " << info.eigenvalue(i) << " residual(" << i+1 << "): "
			<< info.residual(i) << " error_info(" << i+1 << "): "
			<< info.error_info(i) << "\n\n";
	}


  	std::vector<EigenComponent> solution(eigenvectors.size());
	for(unsigned int i=0; i<solution.size(); i++) {
		EigenComponent& cmp = solution[i];
		cmp.eigenvalue = *(ew_begin + i);
#ifdef SPECTRAL_VERBOSE
		std::cout << "Eigenvalue " << i << ": " << cmp.eigenvalue << std::endl;
		cmp.eigenvector = Eigen::VectorXf(N);
#endif
		for(unsigned int j=0; j<N; j++) {
			// convert back to generalized eigenvalue problem!
			cmp.eigenvector[j] = eigenvectors[i][j];
		}
	}
#ifdef SPECTRAL_VERBOSE
	std::cout << "Sparse Solver: returning" << std::endl;
#endif
	return solution;
}

}}
