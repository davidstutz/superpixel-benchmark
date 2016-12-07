/*
 * spectral_impl.hpp
 *
 *  Created on: Jan 29, 2012
 *      Author: david
 */

#ifndef GRAPHSEG_SPECTRAL_SPECTRALIMPL_HPP_
#define GRAPHSEG_SPECTRAL_SPECTRALIMPL_HPP_

#include "../Common.hpp"
#include "../as_range.hpp"
#include <boost/graph/adjacency_list.hpp>
#include <functional>
#include <iostream>
#include <fstream>
#include <vector>
#ifdef SEGS_DBG_PRINT
#include <boost/format.hpp>
#include <fstream>
#endif

namespace graphseg { namespace detail {

constexpr float c_D_min = 0.001f;

template<typename K>
struct DenseGev
{
	Eigen::Matrix<K,-1,-1> L;
	Eigen::Matrix<K,-1,1> D;
};

template<typename K, typename Graph, typename EdgeWeightMap>
DenseGev<K> dense_graph_to_gev(const Graph& graph, EdgeWeightMap edge_weights)
{
	typedef Eigen::Matrix<K,-1,-1> matrix_t;
	typedef Eigen::Matrix<K,-1,1> vector_t;
	const unsigned int dim = boost::num_vertices(graph);
	// creating matrices
	matrix_t W = matrix_t::Zero(dim,dim);
	vector_t D = vector_t::Zero(dim);

#ifdef SPECTRAL_VERBOSE
	std::cout << "DEBUG: Number of vertices = " << boost::num_vertices(graph) << std::endl; 
	std::cout << "DEBUG: Number of edges = " << boost::num_edges(graph) << std::endl; 
#endif

	for(auto eid : as_range(boost::edges(graph))) {
		unsigned int ea = boost::source(eid, graph);
		unsigned int eb = boost::target(eid, graph);
		K ew = edge_weights[eid];
		if(std::isnan(ew)) {
			std::cerr << "ERROR: Weight for edge (" << ea << "," << eb << ") is nan!" << std::endl;
			continue;
		}
		if(ew < 0) {
			std::cerr << "ERROR: Weight for edge (" << ea << "," << eb << ") is negative!" << std::endl;
			continue;
		}
		W(ea, eb) = ew;
		W(eb, ea) = ew;
		D[ea] += ew;
		D[eb] += ew;
	}
	// connect disconnected segments to everything
	// FIXME why is this necessary?
#ifdef SPECTRAL_VERBOSE
	std::vector<int> nodes_with_no_connection;
#endif
	for(unsigned int i=0; i<dim; i++) {
		K& di = D[i];
		if(di < c_D_min) {
#ifdef SPECTRAL_VERBOSE
			nodes_with_no_connection.push_back(i);
#endif
			// connect the disconnected cluster to all other clusters with a very small weight
			di = static_cast<K>(1);
			K q = di / static_cast<K>(dim-1);
			for(unsigned int j=0; j<dim; j++) {
				if(j == i) continue;
				W(i,j) = q;
				W(j,i) = q;
			}
		}
	}
#ifdef SPECTRAL_VERBOSE
	if(!nodes_with_no_connection.empty()) {
		std::cout << "DEBUG: Nodes without connections (#=" << nodes_with_no_connection.size() << "): ";
		for(int i : nodes_with_no_connection) {
			std::cout << i << ", ";
		}
		std::cout << std::endl;
	}
#endif	
	// compute matrix L = D - W
	matrix_t L = -W;
	for(unsigned int i=0; i<dim; i++) {
		L(i,i) += D[i];
	}
	// ready
	return { L, D };
}

template<typename K>
struct DenseGevTransformed
{
	Eigen::Matrix<K,-1,-1> A;
	Eigen::Matrix<K,-1,1> D_inv_sqrt;
};

template<typename K>
DenseGevTransformed<K> dense_gev_to_ev(const DenseGev<K>& gev)
{
	typedef Eigen::Matrix<K,-1,-1> matrix_t;
	typedef Eigen::Matrix<K,-1,1> vector_t;
	// The general eigenvalue problem
	//     (D - W) x = \lambda D x
	// can be transformed as follows:
	// <=> D^{-1/2} (D - W) x = \lambda D^{1/2} x
	// using z := D^{1/2} x i.e. x = D^{-1/2} z
	// <=> D^{-1/2} (D - W) D^{-1/2} z = \lambda z
	// Thus we have a "normal" eigenvalue problem A z = \lambda z with
	//     A := D^{-1/2} (D - W) D^{-1/2}
	// Using L := D - W this gives for coefficients:
	//		a_ij = l_ij / \sqrt(d_i * d_j)
	// where d_i := D_ii
	const matrix_t& L = gev.L;
	const vector_t& D = gev.D;
	assert(L.cols() == L.rows());
	assert(D.rows() == L.rows());
	const unsigned int N = L.rows();
	vector_t D_inv_sqrt = D.array().sqrt().inverse().matrix();
	matrix_t A(N,N);
	for(unsigned int i=0; i<N; i++) {
		A.col(i) = D_inv_sqrt[i] * L.col(i).cwiseProduct(D_inv_sqrt);
	}
	return { A, D_inv_sqrt };
}

template<typename K>
struct SparseGEVT
{
	SparseMatrix A;
	Eigen::VectorXf D_inv_sqrt;
};

template<typename K, typename Graph, typename EdgeWeightMap>
SparseGEVT<K> sparse_graph_entries(const Graph& graph, EdgeWeightMap edge_weights)
{
	// We want to solve the EV problem: (D - W) x = \lamda D x.
	// Each edge of the graph defines two entries into the symmetric matrix W.
	// The diagonal matrix D is defined via d_i = sum_j{w_ij}.

	// As D is a diagonal matrix the the general problem can be easily transformed
	// into a normal eigenvalue problem by decomposing D = L L^t, which yields L = sqrt(D).
	// Thus the EV problem is: L^{-1} (D - W) L^{-T} y = \lambda y.
	// Eigenvectors can be transformed using x = L^{-T} y.

	// The dimension of the problem
	const int n = boost::num_vertices(graph);

	// Each edge defines two entries (one in the upper and one in the lower).
	// In addition all diagonal entries are non-zero.
	// Thus the number of non-zero entries in the lower triangle is equal to
	// the number of edges plus the number of nodes.
	// This is not entirely true as some connections are possibly rejected.
	// Additionally some connections may be added to assure global connectivity.
	const int nnz_guess = boost::num_edges(graph) + n;

	// collect all non-zero elements
	SparseGEVT<K> sgevt;
	sgevt.A.dim = n;

	std::vector<SparseEntry>& entries = sgevt.A.entries;
	entries.reserve(nnz_guess);

	// also collect diagonal entries
	Eigen::VectorXf& diag = sgevt.D_inv_sqrt;
	diag = Eigen::VectorXf(n);

	// no collect entries
	for(auto eid : as_range(boost::edges(graph))) {
		int ea = static_cast<int>(boost::source(eid, graph));
		int eb = static_cast<int>(boost::target(eid, graph));
		K ew = edge_weights[eid];
		// assure correct edge weight
		if(std::isnan(ew)) {
			std::cerr << "ERROR: Weight for edge (" << ea << "," << eb << ") is nan!" << std::endl;
			continue;
		}
		if(ew < 0) {
			std::cerr << "ERROR: Weight for edge (" << ea << "," << eb << ") is negative!" << std::endl;
			continue;
		}
		// assure that no vertices is connected to self
		if(ea == eb) {
			std::cerr << "ERROR: Vertex " << ea << " is connected to self!" << std::endl;
			continue;
		}
		// In the lower triangle the row index i is bigger or equal than the column index j.
		// The next statement fullfills this requirement.
		if(ea < eb) {
			std::swap(ea, eb);
		}
		entries.push_back(SparseEntry{ea, eb, ew});
		diag[ea] += ew;
		diag[eb] += ew;
	}

	// do the conversion to a normal ev problem
	// assure global connectivity
	for(unsigned int i=0; i<diag.size(); i++) {
		K& v = diag[i];
		if(v == 0) {
			// connect the disconnected cluster to all other clusters with a very small weight
			v = static_cast<K>(1);
			K q = static_cast<K>(1) / static_cast<K>(n-1);
			for(unsigned int j=0; j<i; j++) {
				auto it = std::find_if(entries.begin(), entries.end(), [i, j](const SparseEntry& e) { return e.i == i && e.j == j; });
				if(it == entries.end()) {
					entries.push_back(SparseEntry{i, j, q});
				}
			}
			for(unsigned int j=i+1; j<n; j++) {
				auto it = std::find_if(entries.begin(), entries.end(), [j, i](const SparseEntry& e) { return e.i == j && e.j == i; });
				if(it == entries.end()) {
					entries.push_back(SparseEntry{j, i, q});
				}
			}
			std::cerr << "ERROR: Diagonal is 0! (i=" << i << ")" << std::endl;
		}
		else {
			v = static_cast<K>(1) / std::sqrt(v);
		}
	}

	// a_ij for the transformed "normal" EV problem
	//		A x = \lambda x
	// is computed as follow from the diagonal matrix D and the weight
	// matrix W of the general EV problem
	//		(D - W) x = \lambda D x
	// as follows:
	//		a_ij = - w_ij / sqrt(d_i * d_j) if i != j
	//		a_ii = 1
	for(SparseEntry& e : entries) {
		e.weight = - e.weight * diag[e.i] * diag[e.j];
	}
	for(unsigned int i=0; i<n; i++) {
		entries.push_back(SparseEntry{i, i, static_cast<K>(1)});
	}

	// sort entries to form a lower triangle matrix
	std::sort(entries.begin(), entries.end(), [](const SparseEntry& a, const SparseEntry& b) {
		return (a.j != b.j) ? (a.j < b.j) : (a.i < b.i);
	});

	return sgevt;
}

template<typename K>
void transform_gev_solution(const Eigen::Matrix<K,-1,1>& D_inv_sqrt, std::vector<EigenComponent>& ec)
{
	// We have x = D^{-1/2} z (see dense_gev_to_ev)
	// thus x_i = z_i / \sqrt(d_i)
	const unsigned int dim = D_inv_sqrt.rows();
	for(std::size_t i=0; i<ec.size(); i++) {
		ec[i].eigenvector = ec[i].eigenvector.cwiseProduct(D_inv_sqrt);
	}
}

/** Assembles edge weights from eigenvalues and eigenvectors */
template<typename Graph>
Eigen::VectorXf ev_to_graph_weights(const Graph& graph, const std::vector<EigenComponent>& solution)
{
	typedef Eigen::Matrix<float,-1,-1> matrix_t;
	typedef Eigen::Matrix<float,-1,1> vector_t;
	vector_t edge_weight = vector_t::Zero(boost::num_edges(graph));
//	// later we weight by eigenvalues
//	// find a positive eigenvalue (need to do this because of ugly instabilities ...
//	Real ew_pos = -1.0f;
//	for(unsigned int i=0; ; i++) {
//		if(solver.eigenvalues()[i] > 0) {
//			// F IXME magic to get a not too small eigenvalue
////			unsigned int x = (n_used_ew + i)/2;
//			unsigned int x = i + 5;
//			ew_pos = solver.eigenvalues()[x];
//			break;
//		}
//	}
//	// compute normalized weights from eigenvalues
//	Vec weights = Vec::Zero(n_used_ew);
//	for(unsigned int k=0; k<n_used_ew; k++) {
//		Real ew = solver.eigenvalues()[k + 1];
//		if(ew <= ew_pos) {
//			ew = ew_pos;
//		}
//		weights[k] = 1.0f / std::sqrt(ew);
//	}
//	std::cout << "Weights = " << weights.transpose() << std::endl;
	// look into first eigenvectors
	// skip first component
	for(unsigned int k=0; k<solution.size(); k++) {
		const EigenComponent& eigen = solution[k];
		// omit if eigenvalue is not positive
		float ew = eigen.eigenvalue;
		// FIXME this is due to numerical instabilities
		if(ew <= 0.0001f) {
			continue;
		}
		// weight by eigenvalue
		float w = 1.0f / std::sqrt(ew);
		// get eigenvector and normalize
		vector_t ev = eigen.eigenvector;
		ev = (ev - ev.minCoeff() * vector_t::Ones(ev.rows())) / (ev.maxCoeff() - ev.minCoeff());
		// for each edge compute difference of eigenvector values
		vector_t e_k = vector_t::Zero(edge_weight.rows());
		// FIXME proper edge indexing
		unsigned int eid_index = 0;
		for(auto eid : as_range(boost::edges(graph))) {
			e_k[eid_index] = std::abs(ev[boost::source(eid, graph)] - ev[boost::target(eid, graph)]);
			eid_index++;
		}
#ifdef SPECTRAL_VERBOSE
		std::cout << "DEBUG w=" << w << " e_k.maxCoeff()=" << e_k.maxCoeff() << std::endl;
#endif
//		e_k /= e_k.maxCoeff();
//		for(unsigned int i=0; i<e_k.rows(); i++) {
//			e_k[i] = std::exp(-e_k[i]);
//		}
		e_k *= w;

#ifdef SEGS_DBG_PRINT
		{
			std::ofstream ofs((boost::format("/tmp/edge_weights_%03d.txt") % k).str());
			for(unsigned int i=0; i<e_k.rows(); i++) {
				ofs << e_k[i] << std::endl;
			}
		}
#endif
		//
		edge_weight += e_k;
	}
	return edge_weight;
}

template<typename K, int ROWS, int COLS>
inline void print_matrix(std::ostream& os, const Eigen::Matrix<K,ROWS,COLS>& m)
{
	for(unsigned int j=0; j<m.rows(); j++) {
		for(unsigned int i=0; i<m.cols(); i++) {
			os << m(j,i);
			if(i+1 == m.cols()) {
				os << "\n";
			}
			else {
				os << "\t";
			}
		}
	}
}

template<typename Graph, typename EdgeWeightMap>
std::vector<EigenComponent> solve_dense(const Graph& graph, EdgeWeightMap edge_weights,
	const std::function<std::vector<EigenComponent>(const Eigen::MatrixXf&)>& solver)
{
	typedef float K;

	DenseGev<K> gev = dense_graph_to_gev<K>(graph, edge_weights);
#ifdef SEGS_DBG_PRINT
		{	std::ofstream ofs("/tmp/L.tsv"); print_matrix(ofs, gev.L); }
		{	std::ofstream ofs("/tmp/D.tsv"); print_matrix(ofs, gev.D); }
#endif

	DenseGevTransformed<K> gevt = dense_gev_to_ev(gev);
#ifdef SEGS_DBG_PRINT
		{	std::ofstream ofs("/tmp/A.tsv"); print_matrix(ofs, gevt.A); }
		{	std::ofstream ofs("/tmp/D_inv_sqrt.tsv"); print_matrix(ofs, gevt.D_inv_sqrt); }
#endif

	std::vector<EigenComponent> v_ec = solver(gevt.A);

	transform_gev_solution(gevt.D_inv_sqrt, v_ec);

	return v_ec;
}

template<typename Graph, typename EdgeWeightMap>
std::vector<EigenComponent> solve_sparse(const Graph& graph, EdgeWeightMap edge_weights,
	const std::function<std::vector<EigenComponent>(const SparseMatrix&)>& solver)
{
	typedef float K;
	SparseGEVT<K> sgevt = sparse_graph_entries<K>(graph, edge_weights);
	std::vector<EigenComponent> v_ec = solver(sgevt.A);
	transform_gev_solution(sgevt.D_inv_sqrt, v_ec);
	return v_ec;
}

}}

#endif
