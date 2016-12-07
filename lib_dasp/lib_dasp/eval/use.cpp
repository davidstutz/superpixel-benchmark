/*
 * use.cpp
 *
 *  Created on: Mar 26, 2012
 *      Author: david
 */

#include <Slimage/Slimage.hpp>
#include <boost/assert.hpp>
#include <iostream>
#include <map>
#include <set>
#include <cmath>
#include <vector>

//#define DASP_DEBUG_GUI

#ifdef DASP_DEBUG_GUI
 #include <Slimage/Gui.hpp>
#endif

namespace dasp {
namespace eval {

std::vector<std::pair<int,int>> UndersegmentationErrorImpl(const slimage::Image1i& labels_relevant, const slimage::Image1i& labels_retrieved)
{
	constexpr float GAMMA = 0.05f;
	BOOST_ASSERT(labels_relevant.dimensions() == labels_retrieved.dimensions());
	// area of labels
	std::map<int, unsigned int> retrieved_area;
	std::map<int, unsigned int> relevant_area;
	// given relevant label: store how often each retrieved labels occurrs
	std::map<int, std::map<int,unsigned int> > label_intersection;
	unsigned int total_retrieved = 0;
	// generate data
	for(unsigned int i=0; i<labels_relevant.size(); i++) {
		int label_retrieved = labels_retrieved[i];
		int label_relevant = labels_relevant[i];
		total_retrieved ++;
		retrieved_area[label_retrieved] ++;
		relevant_area[label_relevant] ++;
		label_intersection[label_relevant][label_retrieved] ++;
	}
	// compute error
	std::vector<std::pair<int,int>> error;
	for(auto it=label_intersection.begin(); it!=label_intersection.end(); ++it) {
#ifdef DASP_DEBUG_GUI
		std::cout << "Relevant label " << it->first << std::endl;
#endif
		// sum up the area of all retreived labels which intersect the relevant label
		unsigned int common_area = 0;
		for(auto p : it->second) {
			// total area of retreived label
			// treat unassigned pixels as own label for each relevant label
			unsigned int area_retrieved = (p.first == -1) ? p.second : retrieved_area[p.first];
			// area of intersection of relevant and retrieved label
			unsigned int area_intersect = p.second;
			// only count if overlap is big enough with respect to retreived segment size
			float percentage = static_cast<float>(area_intersect)/static_cast<float>(area_retrieved);
			if(percentage > GAMMA)
				common_area += area_retrieved;
		}
#ifdef DASP_DEBUG_GUI
		std::cout << "label=" << it->first << ", A_g=" << g_area << ", S_g=" << s_area << ", error=" << q << std::endl;
#endif
		error.push_back({
			static_cast<int>(common_area),
			static_cast<int>(relevant_area[it->first])
		});
	}

#ifdef DASP_DEBUG_GUI
	{
		for(auto p : relevant_superpixel_labels) {
			slimage::Image3ub vis(labels_relevant.dimensions(), slimage::Pixel3ub{{0,0,0}});
			unsigned int label = p.first;
			std::set<int> ids = p.second;
			for(unsigned int i=0; i<vis.size(); i++) {
				bool is_ground = (labels_relevant[i] == label);
				bool is_super = (ids.find(labels_retrieved[i]) != ids.end());
				if(is_ground && is_super) {
					vis[i] = slimage::Pixel3ub{{255,255,255}};
				}
				else if(is_ground) {
					vis[i] = slimage::Pixel3ub{{255,0,0}};
				}
				else if(is_super) {
					vis[i] = slimage::Pixel3ub{{0,128,255}};
				}
			}
			std::cout << "label=" << label << std::endl;
			slimage::gui::Show("undersegmentation error", vis);
			slimage::gui::WaitForKeypress();
		}
	}
#endif

	return error;
}

float UndersegmentationError(const slimage::Image1i& labels_relevant, const slimage::Image1i& labels_retrieved)
{
	std::vector<std::pair<int,int>> error = UndersegmentationErrorImpl(labels_relevant, labels_retrieved);
#ifdef DASP_DEBUG_GUI
	std::cout << "USE: " << std::endl;
	for(auto p : error) {
		std::cout << p.first << " / " << p.second << std::endl;
	}
#endif
	float total1 = std::accumulate(error.begin(), error.end(), 0.0f,
		[](float a, const std::pair<int,int>& p) { return a + p.first; });
	float total2 = std::accumulate(error.begin(), error.end(), 0.0f,
		[](float a, const std::pair<int,int>& p) { return a + p.second; });
	return total1 / total2 - 1.0f;
}

}}
