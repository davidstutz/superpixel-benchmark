/*
 * lapack.hpp
 *
 *  Created on: Jan 19, 2012
 *      Author: david
 */

#include "solver.hpp"
#include <iostream>
#include <fstream>
#include <vector>

extern "C" int ssyevr_(
		char*,char*,char*,
		int*,float*,int*,
		float*,float*,int*,int*,
		float*,
		int*,float*,float*,int*,int*,
		float*,int*,int*,int*,
		int*);

namespace graphseg { namespace detail {

std::vector<EigenComponent> solver_lapack(const Eigen::MatrixXf& Ain, unsigned int num_ev)
{
	Eigen::MatrixXf A = Ain;

	// calling lapack!
	std::cout << "LAPACK: Start!" << std::endl;

	int N = A.rows();
	std::cout << "N=" << N << std::endl;
	float* data = A.data();
	float vl, vu;
	int il = 1, iu = num_ev;
	float accuracy = 0.00001f;
	int result_num_ew_found;
	float* result_ew = new float[N];
	float* result_ev = new float[N*N];
	int* result_isuppz = new int[2*N];
	int work_dim = -1;
	float* work = new float[1];
	int iwork_dim = -1;
	int* iwork = new int[1];
	int info;

	// ssyevr_(
	// 	"N", // JOBZ eigenvalues + eigenvectors
	// 	"A", // RANGE only some eigenvalues
	// 	"U", // UPLO upper triangle is stored
	// 	&N, // N order of A
	// 	data, // A upper triangle of A
	// 	&N, // LDA leading dimension of A
	// 	&vl, &vu, // VL,VU not used
	// 	&il, &iu, // IL, IU  range of eigenvalues returned
	// 	&accuracy, // ABSTOL accuracy
	// 	&result_num_ew_found, // M number of eigenvalues found
	// 	result_ew, // W computed eigenvalues
	// 	result_ev, // Z computed eigenvectors
	// 	&N, // LDZ leading dimension of Z
	// 	result_isuppz, // ISUPPZ
	// 	work, // WORK
	// 	&work_dim, // LWORK
	// 	iwork, // IWORK
	// 	&iwork_dim, // LIWORK
	// 	&info // INFO
	// 	);
	// work_dim = (int)*work;
	// iwork_dim = *iwork;

	work_dim = N*N;
	iwork_dim = N*N;

	delete[] work;
	delete[] iwork;
	std::cout << "LAPACK: work_dim (opt) =" << work_dim << std::endl;
	std::cout << "LAPACK: iwork_dim (opt) =" << iwork_dim << std::endl;
	work = new float[work_dim];
	iwork = new int[iwork_dim];

	ssyevr_(
		"V", // JOBZ eigenvalues + eigenvectors
		"A", // RANGE only some eigenvalues
		"U", // UPLO upper triangle is stored
		&N, // N order of A
		data, // A upper triangle of A
		&N, // LDA leading dimension of A
		0, 0, // VL,VU not used
		&il, &iu, // IL, IU  range of eigenvalues returned
		&accuracy, // ABSTOL accuracy
		&result_num_ew_found, // M number of eigenvalues found
		result_ew, // W computed eigenvalues
		result_ev, // Z computed eigenvectors
		&N, // LDZ leading dimension of Z
		result_isuppz, // ISUPPZ
		work, // WORK
		&work_dim, // LWORK
		iwork, // IWORK
		&iwork_dim, // LIWORK
		&info // INFO
		);

	std::cout << "LAPACK: Finished!" << std::endl;
	std::cout << "Number of eigenvalues: " << result_num_ew_found << std::endl;
	std::cout << "Info: " << info << std::endl;

	// return eigenvectors and eigenvalues
	std::vector<EigenComponent> solution(std::min<unsigned int>(num_ev, result_num_ew_found));
	for(std::size_t i=0; i<solution.size(); i++) {
		solution[i].eigenvalue = result_ew[i];
#ifdef SPECTRAL_VERBOSE
		std::cout << "SpectralSegmentation: eigenvalue #" << i << "=" << solution[i].eigenvalue << std::endl;
#endif
		solution[i].eigenvector = Eigen::VectorXf(N);
		auto& ev = solution[i].eigenvector;
		for(unsigned int j=0; j<N; j++) {
			ev[j] = result_ev[i*N + j]; // FIXME correct order?
		}
#ifdef SPECTRAL_VERBOSE
		std::cout << ev.transpose() << std::endl;
#endif
	}

	delete[] result_ew;
	delete[] result_ev;
	delete[] result_isuppz;
	delete[] work;
	delete[] iwork;

	return solution;
}

}}
