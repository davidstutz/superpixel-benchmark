#include "eval.hpp"
#include <Superpixels.hpp>
#include <boost/math/constants/constants.hpp>
#include <iostream>

namespace dasp {
namespace eval {

float Area(const Superpixels& u)
{
	// compute labels
	slimage::Image1i labels = u.ComputeLabels();
	std::vector<unsigned int> sp_area(u.clusterCount());
	for(std::size_t i=0; i<labels.size(); i++) {
		int label = labels[i];
		if(label == -1) {
			continue;
		}
		// compute pixel area
		sp_area[label] ++;
	}
	// mean
	return static_cast<float>(std::accumulate(sp_area.begin(), sp_area.end(), 0.0f)) / static_cast<float>(sp_area.size());
}

float Area3D(const Superpixels& u)
{
	// compute labels
	slimage::Image1i labels = u.ComputeLabels();
	std::vector<float> sp_area(u.clusterCount());
	for(std::size_t i=0; i<labels.size(); i++) {
		int label = labels[i];
		if(label == -1) {
			continue;
		}
		// 3D 
		const Point& p = u.points[i];
		float size_of_a_px = p.depth() / u.opt.camera.focal;
		float area_of_a_px = size_of_a_px*size_of_a_px / p.computeCircularity();
		// compute pixel area
		sp_area[label] += area_of_a_px;
	}
	// mean
	return std::accumulate(sp_area.begin(), sp_area.end(), 0.0f) / static_cast<float>(sp_area.size());
}

slimage::Image1i MarkBoundary(const slimage::Image1i& labels)
{
	slimage::Image1i boundary(labels.dimensions(), slimage::Pixel1i{1});
	for(unsigned int y=1; y+1<boundary.height(); y++) {
		for(unsigned int x=1; x+1<boundary.width(); x++) {
			int q = labels(x, y);
			int qp0 = labels(x+1, y  );
			int qm0 = labels(x-1, y  );
			int q0p = labels(x  , y+1);
			int q0m = labels(x  , y-1);
			if(q == qp0 && q == qm0 && q == q0p && q == q0m) {
				boundary(x,y) = 0;
			}
		}
	}
	return boundary;
}

template<typename K>
std::pair<float,std::vector<float>> IsoperimetricQuotientImpl(const std::vector<std::pair<K,K>>& sp_area_length)
{
	// per cluster: v_i = 4*pi*A_i/L_i^2
	// total: sum_i v_i*A_i
	std::vector<float> ipq(sp_area_length.size());
	float ipq_total = 0.0f;
	float num_total = 0.0f;
	for(std::size_t i=0; i<ipq.size(); i++) {
		const auto& q = sp_area_length[i];
		float area = static_cast<float>(q.first);
		float len = static_cast<float>(q.second);
		float x = 4.0f*boost::math::constants::pi<float>() * area / (len*len);
		ipq[i] = x;
		ipq_total += x*area;
		num_total += area;
//		std::cout << "area=" << area << ", len=" << len << ", ipq=" << x << std::endl;
	}
	return {ipq_total / num_total, ipq};
}

std::pair<float,std::vector<float>> IsoperimetricQuotient(const Superpixels& u)
{
	// compute labels
	slimage::Image1i labels = u.ComputeLabels();
	// compute boundary image
	slimage::Image1i boundary = MarkBoundary(labels);
	// compute total pixel count and number of boundary pixels for each cluster
	std::vector<std::pair<int,int>> ipq_els(u.clusterCount(), {0,0});
	for(std::size_t i=0; i<labels.size(); i++) {
		int label = labels[i];
		if(label == -1) {
			continue;
		}
		if(boundary[i] == 1) {
			ipq_els[label].second ++;
		}
		else {
			ipq_els[label].first ++;	
		}
	}
	// finalize
	return IsoperimetricQuotientImpl(ipq_els);
}

std::pair<float,std::vector<float>> IsoperimetricQuotient3D(const Superpixels& u)
{
	// compute labels
	slimage::Image1i labels = u.ComputeLabels();
	// compute boundary image
	slimage::Image1i boundary = MarkBoundary(labels);
	// compute total pixel count and number of boundary pixels for each cluster
	std::vector<std::pair<float,float>> ipq_els(u.clusterCount(), {0.0f,0.0f});
//	std::vector<std::vector<std::size_t>> seg_bnd(u.clusterCount());
	for(std::size_t i=0; i<labels.size(); i++) {
		int label = labels[i];
		if(label == -1) {
			continue;
		}
		// 3D 
		const Point& p = u.points[i];
		float size_of_a_px = p.depth() / u.opt.camera.focal;
		float gamma = p.computeCircularity();
		float px_area = size_of_a_px*size_of_a_px / gamma;
		float px_len = (1.571 - 0.571 * gamma) * size_of_a_px;
		// compute pixel area
		ipq_els[label].first += px_area;
		// compute pixel diameter
		if(boundary[i] == 1) {
//			seg_bnd[label].push_back(i);
			ipq_els[label].second += px_len;
		}
	}
// 	// compute segment boundary length
// 	unsigned int w = u.width();
// 	for(unsigned int k=0; k<seg_bnd.size(); k++) {
// 		float L = 0.0f;
// 		const std::vector<std::size_t>& B = seg_bnd[k];
// 		for(unsigned int i=0; i<B.size(); i++) {
// 			unsigned int ii = B[i];
// 			int iy = ii/w;
// 			int ix = ii - iy*w;
// 			for(unsigned int j=i+1; j<B.size(); j++) {
// 				unsigned int jj = B[j];
// 				int jy = jj/w;
// 				int jx = jj - jy*w;
// 				int dx = std::abs(ix-jx);
// 				int dy = std::abs(iy-jy);
// 				if((dx == 1 && dy == 0) || (dx == 0 && dy == 1)) {
// 					L += (u.points[ii].position - u.points[jj].position).norm();
// 				}
// 			}
// 		}
// //		std::cout << k << ": " << ipq_els[k].second << " -> " << L << std::endl;
// 		ipq_els[k].second = L;
// 	}
	// finalize
	return IsoperimetricQuotientImpl(ipq_els);
}

}}
