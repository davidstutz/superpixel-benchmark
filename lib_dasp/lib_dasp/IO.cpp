#include "IO.hpp"
#include "Superpixels.hpp"
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>

namespace dasp
{
	void SaveSuperpixels(const Superpixels& superpixels, const std::string& filename, const bool binary)
	{
		std::vector<std::vector<float>> data;
		data.reserve(superpixels.cluster.size());
		for(const Cluster& c : superpixels.cluster) {
			const Point& p = c.center;
			std::vector<float> v {
				static_cast<float>(p.px), static_cast<float>(p.py),
				p.position.x(), p.position.y(), p.position.z(),
				p.color.x(), p.color.y(), p.color.z(),
				p.normal.x(), p.normal.y(), p.normal.z()
			};
			data.push_back(v);
		}
		SaveData(data, filename, binary);
	}

	// void LoadSuperpixels(Superpixels& superpixels, const std::string& filename, bool binary)
	// {
	// 	throw "Not implemented";
	// 	std::vector<std::vector<float>> data = LoadData(filename, binary);
	// 	superpixels.cluster.clear();
	// 	superpixels.cluster.reserve(data.size());
	// 	for(const std::vector<float>& v : data) {
	// 		//
	// 	}
	// }

	void SaveData(const std::vector<std::vector<float>>& data, const std::string& filename, const bool binary)
	{
		std::ofstream ofs;
		if(binary) {
			ofs.open(filename, std::fstream::out | std::fstream::binary);
		}
		else {
			ofs.open(filename, std::fstream::out);
		}
		if(!ofs.is_open()) {
			throw std::runtime_error("Could not open file '" + filename + "'!");
		}
		if(data.size() == 0) {
			return;
		}
		ofs.precision(4); // 4 is sub mm for position - 3 would be enough for color and normal
		const unsigned int size = data[0].size();
		for(const std::vector<float>& v : data) {
			// if(v.size() != size) {
			// 	throw "Invalid data!";
			// }
			const float* p = &v[0];
			if(binary) {
				ofs.write(reinterpret_cast<const char*>(p), size*sizeof(float));
			}
			else {
				for(unsigned int i=0; i<size; i++) {
					ofs << p[i];
					if(i == size-1)
						ofs << "\n";
					else
						ofs << "\t";
				}
			}
		}
	}

	std::vector<std::vector<float>> LoadData(const std::string& filename, const bool binary)
	{
		std::ifstream ifs;
		if(binary) {
			ifs.open(filename, std::fstream::in | std::fstream::binary);
		}
		else {
			ifs.open(filename, std::fstream::in);
		}
		if(!ifs.is_open()) {
			throw std::runtime_error("Could not open file '" + filename + "'!");
		}
		std::vector<std::vector<float>> data;
		if(binary) {
			// FIXME implement
			throw std::logic_error("Not implemented!");
		}
		else {
			boost::escaped_list_separator<char> separator('\\','\t','\"');
			typedef boost::tokenizer<decltype(separator)> Tokenizer;
			std::string line;
			std::vector<float> v;
			while(std::getline(ifs,line))
			{
				v.clear();
				Tokenizer tok(line, separator);
				for(Tokenizer::iterator beg=tok.begin(); beg!=tok.end(); ++beg) {
					float x = boost::lexical_cast<float>(*beg);
					v.push_back(x);
				}
				data.push_back(v);
			}
		}
		return data;
	}

	void SaveDaspGraph(const DaspGraph& graph, const std::string& fn_dasp, const std::string& fn_edges)
	{
		const std::string sep = "\t";
		// write data
		std::ofstream ofs_v(fn_dasp);
		if(!ofs_v.is_open()) {
			throw std::runtime_error("Could not open file '" + fn_dasp + "'!");
		}
		for(auto vid : as_range(boost::vertices(graph))) {
			const Point& p = graph[vid];
			ofs_v
				<< p.px << sep << p.py << sep
				<< p.position.x() << sep << p.position.y() << sep << p.position.z() << sep
				<< p.color.x() << sep << p.color.y() << sep << p.color.z() << sep
				<< p.normal.x() << sep << p.normal.y() << sep << p.normal.z() << std::endl;
		}
		// write edges
		std::ofstream ofs_e(fn_edges);
		if(!ofs_e.is_open()) {
			throw std::runtime_error("Could not open file '" + fn_edges + "'!");
		}
		for(auto eid : as_range(boost::edges(graph))) {
			ofs_e
				<< boost::source(eid, graph) << sep
				<< boost::target(eid, graph) << sep
				<< graph[eid] << std::endl;
		}
	}

	DaspGraph LoadDaspGraph(const std::string& fn_dasp, const std::string& fn_edges)
	{
		const std::string separator = "\t";
		constexpr unsigned int num_values_per_vertex = 11;
		constexpr unsigned int num_values_per_edge_min = 2;
		constexpr unsigned int num_values_per_edge_max = 3;

		DaspGraph g;

		// load superpixel vertices
		{
			std::ifstream ifs(fn_dasp);
			if(!ifs.is_open()) {
				throw std::runtime_error("Could not open file '" + fn_dasp + "'!");
			}
			std::string line;
			std::vector<std::string> tokens;
			std::vector<float> values;
			while(std::getline(ifs,line)) {
				// split line into tokens
				tokens.clear();
				boost::split(tokens, line, boost::algorithm::is_any_of(separator));
				if(tokens.size() == 0) {
					continue;
				}
				if(tokens.size() != num_values_per_vertex) {
					throw std::runtime_error("Invalid dasp vertex line!");
				}
				// convert to float
				values.resize(tokens.size());
				std::transform(tokens.begin(), tokens.end(), values.begin(),
					[](const std::string& str) { return boost::lexical_cast<float>(str); });
				// add vertex to graph
				auto vid = boost::add_vertex(g);
				Point& p = g[vid];
				p.px = values[0];
				p.py = values[1];
				p.position	= Eigen::Vector3f(values[2], values[3], values[4]);
				p.color		= Eigen::Vector3f(values[5], values[6], values[7]);
				p.normal	= Eigen::Vector3f(values[8], values[9], values[10]);
			}
		}

		// load superpixel edges
		{
			std::ifstream ifs(fn_edges);
			if(!ifs.is_open()) {
				throw std::runtime_error("Could not open file '" + fn_edges + "'!");
			}
			std::vector<std::string> tokens;
			std::string line;
			while(std::getline(ifs,line)) {
				// split line into tokens
				tokens.clear();
				boost::split(tokens, line, boost::algorithm::is_any_of(separator));
				if(tokens.size() == 0) {
					continue;
				}
				if(tokens.size() < num_values_per_edge_min || num_values_per_edge_max < tokens.size()) {
					throw std::runtime_error("Invalid dasp edge line!");
				}
				// convert to indices
				unsigned int e_source = boost::lexical_cast<unsigned int>(tokens[0]);
				unsigned int e_target = boost::lexical_cast<unsigned int>(tokens[1]);
				float weight = (tokens.size() == 3) ? boost::lexical_cast<float>(tokens[2]) : 0.0f;
				// add edge to graph
				auto result = boost::add_edge(e_source, e_target, g);
				// set edge weight
				g[result.first] = weight;
			}
		}		

		return g;
	}

}

