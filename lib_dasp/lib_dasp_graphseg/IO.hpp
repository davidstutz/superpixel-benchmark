#ifndef INCLUDED_GRAPHSEG_IO_HPP_
#define INCLUDED_GRAPHSEG_IO_HPP_

#include "as_range.hpp"
#include <fstream>
#include <stdexcept>

namespace graphseg
{
	/** Writes edge structure of an undirected graph to a file */
	template<typename Graph>
	void WriteEdges(const std::string& filename, const Graph& graph)
	{
		std::ofstream ofs(filename);
		if(!ofs.is_open()) {
			throw std::runtime_error("Could not open file '" + filename + "' for writing!");
		}
		for(auto eid : as_range(boost::edges(graph))) {
			ofs << static_cast<unsigned int>(boost::source(eid, graph)) << "\t"
				<< static_cast<unsigned int>(boost::target(eid, graph)) << "\n";
		}
	}

	/** Writes edge structure of an undirected, weighted graph to a file */
	template<typename Graph, typename WeightMap>
	void WriteEdges(const std::string& filename, const Graph& graph, WeightMap weights)
	{
		std::ofstream ofs(filename);
		if(!ofs.is_open()) {
			throw std::runtime_error("Could not open file '" + filename + " for writing'!");
		}
		for(auto eid : as_range(boost::edges(graph))) {
			ofs << static_cast<unsigned int>(boost::source(eid, graph)) << "\t"
				<< static_cast<unsigned int>(boost::target(eid, graph)) << "\t"
				<< boost::get(weights, eid) << "\n";
		}
	}

	/** Reads a file with undirected graph edges
	 * Expected file format:
	 *   One line per edge
	 *   Two integers per line giving vertex indices
	 */
	template<typename Graph>
	void ReadEdges(const std::string& filename, Graph& graph)
	{
		std::ifstream ifs(filename);
		if(!ifs.is_open()) {
			throw std::runtime_error("Could not open file '" + filename + " for reading'!");
		}
		unsigned int ea, eb;
		while(ifs) {
			ifs >> ea >> eb;
			boost::add_edge(ea, eb, graph);
		}
	}

	/** Reads a file with undirected graph edges
	 * Expected file format:
	 *   One line per edge
	 *   Two integers per line giving vertex indices and one float for the edge weight
	 */
	template<typename Graph, typename WeightMap>
	void ReadEdges(const std::string& filename, Graph& graph, WeightMap weights)
	{
		std::ifstream ifs(filename);
		if(!ifs.is_open()) {
			throw std::runtime_error("Could not open file '" + filename + " for reading'!");
		}
		unsigned int ea, eb;
		typename WeightMap::value_type weight;
		typename Graph::edge_descriptor eid;
		bool ok;
		while(ifs) {
			ifs >> ea >> eb >> weight;
			boost::tie(eid,ok) = boost::add_edge(ea, eb, graph);
			boost::put(weights, eid, weight);
		}
	}

}

#endif
