#include "../Common.hpp"
#include <cuda_runtime_api.h>
#include <cublas.h>
#include <magma.h>
#include <magma_lapack.h>
#include <iostream>

namespace graphseg { namespace detail {

struct MagmaSpectralSolver
{
	MagmaSpectralSolver() {
		if( CUBLAS_STATUS_SUCCESS != cublasInit() ) {
			std::cerr << "MAGMA ERROR: cublasInit failed\n";
			exit(-1);
		}
		printout_devices();
	}

	template<typename K>
	K* malloc(size_t size) {
		K* ptr;
		if(MAGMA_SUCCESS !=
		    magma_malloc_cpu( (void**)&ptr, size*sizeof(K) )) {
			std::cerr << "MAGMA ERROR: malloc failed\n";
		    exit(-1);
		}
		return ptr;
	}

	template<typename K>
	K* hostmalloc(size_t size) {
		K* ptr;
		if(MAGMA_SUCCESS !=
		    magma_malloc_pinned( (void**)&ptr, size*sizeof(K) )) {
			std::cerr << "MAGMA ERROR: magma_malloc_pinned failed\n";
		    exit(-1);
		}
		return ptr;
	}

	template<typename K>
	void free(K* ptr) {
		magma_free_cpu(ptr);
	}

	template<typename K>
	void hostfree(K* ptr) {
		magma_free_pinned(ptr);
	}

	~MagmaSpectralSolver() {
		cublasShutdown();
	}
};

std::vector<EigenComponent> solver_magma(const Eigen::MatrixXf& A, unsigned int num_ev)
{
	static MagmaSpectralSolver magma;

	magma_int_t N = A.rows();
	std::cout << "MAGMA Solver N=" << N << std::endl;

	magma_timestr_t start, end;
	float gpu_time;
	start = get_current_time();

	magma_int_t info;

	const float *h_A = A.data();

	float *h_R, *h_work;
	float *w1;
	magma_int_t *iwork;

	const char *uplo = MagmaLowerStr;
	const char *jobz = MagmaVectorsStr;

	/* Query for workspace sizes */
	float      aux_work[1];
	magma_int_t aux_iwork[1];
	std::cout << "Querying workspace size" << std::endl;
	magma_ssyevd( jobz[0], uplo[0],
	              N, h_R, N, w1,
	              aux_work,  -1,
	              aux_iwork, -1,
	              &info );
	magma_int_t lwork  = (magma_int_t) aux_work[0];
	magma_int_t liwork = aux_iwork[0];
	std::cout << lwork << " " << liwork << std::endl;

	std::cout << "Allocating" << std::endl;
	w1     = magma.malloc<float>(N  );
	h_R    = magma.hostmalloc<float>(N*N);
	h_work = magma.hostmalloc<float>(lwork);
	iwork  = magma.malloc<magma_int_t>(liwork);

	std::cout << "Copying" << std::endl;
	slacpy_( MagmaUpperLowerStr, &N, &N, h_A, &N, h_R, &N );

	std::cout << "Solving" << std::endl;
	magma_ssyevd(jobz[0], uplo[0],
	         N, h_R, N, w1,
	         h_work, lwork,
	         iwork, liwork,
	         &info);

	std::cout << "Collecting" << std::endl;
	// save eigenvectors and eigenvalues
	std::vector<EigenComponent> solution(std::min<int>(N, num_ev));
	for(unsigned int i=0; i<solution.size(); i++) {
		solution[i].eigenvalue = w1[i+1];
		Eigen::VectorXf ev(N);
		for(unsigned int j=0; j<N; j++) {
			ev[j] = *(h_R + i*N + j);
		}
		solution[i].eigenvector = ev;
	}

	std::cout << "Freeing" << std::endl;
	magma.free(w1);
	magma.hostfree(h_R);
	magma.hostfree(h_work);
	magma.free(iwork);

	end = get_current_time();

	gpu_time = GetTimerValue(start,end)/1000.;
	std::cout << "Time: " << gpu_time << std::endl;

	return solution;
}

}}
