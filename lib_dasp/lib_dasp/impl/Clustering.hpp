/*
 * Clustering.hpp
 *
 *  Created on: Apr 4, 2012
 *      Author: david
 */

#ifndef DASP_CLUSTERING_HPP_
#define DASP_CLUSTERING_HPP_

#include "../Point.hpp"
#include "../Parameters.hpp"
#include "../Metric.hpp"
#include <Slimage/Slimage.hpp>
#include <Eigen/Dense>
#include <iostream>

namespace dasp
{

	template<typename METRIC>
	slimage::Image1f ComputeEdges(const ImagePoints& points, const METRIC& mf)
	{
		const unsigned int width = points.width();
		const unsigned int height = points.height();
		slimage::Image1f edges(width, height, slimage::Pixel1f{1e6f});
		// compute edges strength
		for(unsigned int y=1; y<height-1; y++) {
			for(unsigned int x=1; x<width-1; x++) {
				float v;
				const Point& p0 = points(x, y);
				const Point& px1 = points(x-1, y);
				const Point& px2 = points(x+1, y);
				const Point& py1 = points(x, y-1);
				const Point& py2 = points(x, y+1);
				if(!p0.is_valid || !px1.is_valid || !px2.is_valid || !py1.is_valid || !py2.is_valid) {
					v = 1e6; // dont want to be here
				}
				else {
					float dx = mf(px1, px2);
					float dy = mf(py1, py2);
					v = dx + dy;
				}
				edges(x,y) = v;
			}
		}
		return edges;
	}

	template<typename METRIC>
	slimage::Image1i IterateClusters(const std::vector<Cluster>& clusters, const ImagePoints& points, const Parameters& opt, const METRIC& mf)
	{
		slimage::Image1i labels(points.width(), points.height(), slimage::Pixel1i{-1});
		std::vector<float> v_dist(points.size(), 1e9);
		// for each cluster check possible points
		for(unsigned int j=0; j<clusters.size(); j++) {
			const Cluster& c = clusters[j];
			int cx = c.center.px;
			int cy = c.center.py;
			int R = static_cast<int>(c.center.cluster_radius_px * opt.coverage + 0.5f);
			R = std::max<int>(2,R);
			const unsigned int xmin = std::max<int>(0, cx - R);
			const unsigned int xmax = std::min<int>(static_cast<int>(points.width())-1, cx + R);
			const unsigned int ymin = std::max<int>(0, cy - R);
			const unsigned int ymax = std::min<int>(static_cast<int>(points.height())-1, cy + R);
			for(unsigned int y=ymin; y<=ymax; y++) {
				for(unsigned int x=xmin; x<=xmax; x++/*, pnt_index++*/) {
					unsigned int pnt_index = points.index(x, y);
					const Point& p = points[pnt_index];
					if(!p.is_valid) {
						// omit invalid points
						continue;
					}
					float dist = mf(p, c.center);
					float& v_dist_best = v_dist[pnt_index];
					if(dist < v_dist_best) {
						v_dist_best = dist;
						labels[pnt_index] = j;
					}
				}
			}
		}
		return labels;
	}

}

#endif
