#include "Segmentation.hpp"
#include "Neighbourhood.hpp"
#include "Plots.hpp"
#include <Slimage/Convert.hpp>
#include <boost/graph/property_iter_range.hpp>

namespace dasp
{

slimage::Image1ub CreateBoundaryImageFromLabels(const Superpixels& clustering, const graphseg::GraphLabeling& labeling)
{
	// create segment labeling
	slimage::Image1i labels(clustering.width(), clustering.height(), slimage::Pixel1i{-1});
	clustering.ForPixelClusters([&labeling,&labels](unsigned int cid, const dasp::Cluster& c, unsigned int pid, const dasp::Point& p) {
		labels[pid] = labeling.labels[cid];
	});
	// plot segment boundaries
	slimage::Image1ub boundaries_wt = slimage::Image1ub(clustering.width(), clustering.height(), slimage::Pixel1ub{0});
	dasp::plots::PlotEdges(boundaries_wt, labels, slimage::Pixel1ub{255}, 1);
	return boundaries_wt;
}

std::vector<slimage::Pixel3ub> ComputeSegmentColors(const Superpixels& clusters, const graphseg::GraphLabeling& labeling)
{
//	return plots::CreateRandomColors(segment_count);
	struct Pair { Eigen::Vector3f val; unsigned int cnt; };
	std::vector<Pair> segment_center_sum(labeling.num_labels);
	for(unsigned int i=0; i<labeling.labels.size(); i++) {
		Pair& p = segment_center_sum[labeling.labels[i]];
		p.val += clusters.cluster[i].center.color;
		p.cnt ++;
	}
	std::vector<slimage::Pixel3ub> colors(labeling.num_labels);
	for(unsigned int i=0; i<colors.size(); i++) {
		Eigen::Vector3f c = segment_center_sum[i].val / static_cast<float>(segment_center_sum[i].cnt);
		c = clusters.ColorToRGB(c);
		slimage::conversion::Convert(slimage::Pixel3f{{c[0],c[1],c[2]}}, colors[i]);
	}
	return colors;
}

slimage::Image3ub CreateLabelImage(const Superpixels& clusters, const graphseg::GraphLabeling& labeling, const std::vector<slimage::Pixel3ub>& colors)
{
	slimage::Image3ub vis_img(clusters.width(), clusters.height(), slimage::Pixel3ub{{0,0,0}});
	clusters.ForPixelClusters([&labeling,&vis_img,&colors](unsigned int cid, const dasp::Cluster& c, unsigned int pid, const dasp::Point& p) {
		vis_img[pid] = colors[labeling.labels[cid]];
	});
	return vis_img;
}

}
