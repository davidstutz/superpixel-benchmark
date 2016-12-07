#ifndef INCLUDED_DASP_IO_HPP_
#define INCLUDED_DASP_IO_HPP_

#include "Graph.hpp"
#include <Eigen/Dense>
#include <string>
#include <vector>

namespace dasp
{
	class Superpixels;

	/** Saves superpixels to a file
	 * Writes rgb (color), xyz (position) and uvw (normal) in that order for each superpixel.
	 * Binary mode: writes raw data (9 x bytes forming a float)
	 * Text mode: writes floats as strings with 3 digits, one superpixel per line
	 */
	void SaveSuperpixels(const Superpixels& superpixels, const std::string& filename, const bool binary=false);

//	void LoadSuperpixels(Superpixels& superpixels, const std::string& filename, bool binary=true);

	void SaveData(const std::vector<std::vector<float>>& data, const std::string& filename, const bool binary=false);

	std::vector<std::vector<float>> LoadData(const std::string& filename, const bool binary=false);

	void SaveDaspGraph(const DaspGraph& graph, const std::string& fn_dasp, const std::string& fn_edges);

	DaspGraph LoadDaspGraph(const std::string& fn_dasp, const std::string& fn_edges);


}

#endif
