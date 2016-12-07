#ifndef DASP_SPECTRAL_ASRANGE_HPP_
#define DASP_SPECTRAL_ASRANGE_HPP_

//#include <boost/graph/adjacency_list.hpp>
#include <tuple>

namespace detail
{
	template<class Iter>
	struct iter_pair_range
	: std::pair<Iter,Iter>
	{
		iter_pair_range(const std::pair<Iter,Iter>& x)
		: std::pair<Iter,Iter>(x)
		{}

		Iter begin() const {
			return this->first;
		}

		Iter end() const {
			return this->second;
		}
	};
}

template<class Iter>
inline detail::iter_pair_range<Iter> as_range(const std::pair<Iter,Iter>& x) {
	return detail::iter_pair_range<Iter>(x);
}

// template<class Graph>
// inline detail::iter_pair_range<typename Graph::edge_iterator> edges(const Graph& graph) {
// 	return as_range(boost::edges(graph));
// }

// template<class Graph>
// inline detail::iter_pair_range<typename Graph::vertex_iterator> vertices(const Graph& graph) {
// 	return as_range(boost::vertices(graph));
// }

#endif
