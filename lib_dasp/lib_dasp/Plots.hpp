/*
 * Plotting.hpp
 *
 *  Created on: Feb 22, 2012
 *      Author: david
 */

#ifndef PLOTTING_HPP_
#define PLOTTING_HPP_
//----------------------------------------------------------------------------//
#include "Superpixels.hpp"
#include "Graph.hpp"
#include <GL/glew.h>
#include <Slimage/Paint.hpp>
#include <Danvil/Color.h>
#include <vector>
//----------------------------------------------------------------------------//
namespace dasp {
namespace plots {
//----------------------------------------------------------------------------//

/** Get point color */
inline slimage::Pixel3ub RgbColor(const Point& p) {
	return {{
		static_cast<unsigned char>(std::min(255.0f, std::max(0.0f, 255.0f*p.color[0]))),
		static_cast<unsigned char>(std::min(255.0f, std::max(0.0f, 255.0f*p.color[1]))),
		static_cast<unsigned char>(std::min(255.0f, std::max(0.0f, 255.0f*p.color[2])))
	}};
}

/** Color visualization of gradient */
inline slimage::Pixel3ub GradientColor(const Eigen::Vector2f& g)
{
	float x = std::max(0.0f, std::min(1.0f, 0.5f + g[0]));
	float y = std::max(0.0f, std::min(1.0f, 0.5f + g[1]));
	return slimage::Pixel3ub{{
			static_cast<unsigned char>(255.0f*0.5f*(1.0f - x + y)),
			static_cast<unsigned char>(255.0f*0.5f*(2.0f - x - y)),
			static_cast<unsigned char>(255.0f*0.5f*(x + y))}};
}

/** Color visualization of kinect depth */
inline slimage::Pixel3ub DepthColor(float d)
{
	// base gradient: blue -> red -> yellow
	static auto cm = Danvil::ContinuousIntervalColorMapping<unsigned char, float>::Factor_Blue_Red_Yellow_Green();
	cm.setRange(0.750f,2.500f);
	if(d == 0) {
		return slimage::Pixel3ub{{0,0,0}};
	}
	else {
		if(d > 2.500f) {
			return slimage::Pixel3ub{{0, static_cast<unsigned char>(255/(1.0f + (d - 2.500f)/0.500f)), 0}};
		}
		else {
			Danvil::Colorub color = cm(d);
			return slimage::Pixel3ub{{color.r, color.g, color.b}};
		}
//		Danvil::Colorub color = cm(d16);
//		unsigned int q = d16 % 25;
//		unsigned char r = std::max(0, int(color.r) - int(q));
//		unsigned char g = std::max(0, int(color.g) - int(q));
//		unsigned char b = std::max(0, int(color.b) - int(q));
//		return slimage::Pixel3ub{{r,g,b}};
	}
}

/** Color visualization of intensity */
inline slimage::Pixel3ub IntensityColor(float x, float min=0.0f, float max=1.0f)
{
	static auto cm = Danvil::ContinuousIntervalColorMapping<unsigned char, float>::Factor_Blue_Red_Yellow_White();
	cm.setRange(min, max);
	Danvil::Colorub color = cm(x);
	return slimage::Pixel3ub{{color.r,color.g,color.b}};
}

inline slimage::Pixel3ub IntensityColorBW(float x, float min=0.0f, float max=1.0f)
{
	float q = std::max(0.0f, std::min(1.0f, (x - min) / (max - min)));
	unsigned char g = static_cast<unsigned char>(255.0f * q);
	return slimage::Pixel3ub{{ g, g, g }};
}

inline slimage::Pixel3ub DistanceColor(float x, float min=0.0f, float max=2.0f)
{
	return IntensityColor((max - x)/(max - min));
}

/** Color visualization of intensity */
inline slimage::Pixel3ub PlusMinusColor(float x, float range=1.0f)
{
	static auto cm = Danvil::ContinuousIntervalColorMapping<unsigned char, float>::Factor_MinusPlus();
	cm.setRange(-range, +range);
	Danvil::Colorub color = cm(x);
	return slimage::Pixel3ub{{color.r,color.g,color.b}};
}

std::vector<slimage::Pixel3ub> CreateRandomColors(unsigned int cnt);

void PlotClusterPoints(const slimage::Image3ub& img, const Cluster& cluster, const ImagePoints& points, const slimage::Pixel3ub& color);

void PlotClusters(const slimage::Image3ub& img, const Superpixels& clustering, const std::vector<slimage::Pixel3ub>& colors);

void PlotClusterEllipse(const slimage::Image3ub& img, const Cluster& cluster, const slimage::Pixel3ub& color, bool filled);

void PlotEdges(const slimage::Image3ub& img, const slimage::Image1i& labels, const slimage::Pixel3ub& color, unsigned int size=1, bool internal=false);
void PlotEdges(const slimage::Image1ub& img, const slimage::Image1i& labels, const slimage::Pixel1ub& color, unsigned int size=1, bool internal=false);
void PlotEdges(const slimage::Image1f& img, const slimage::Image1i& labels, const slimage::Pixel1f& color, unsigned int size=1, bool internal=false);

void PlotSeeds(const slimage::Image1ub& img, const std::vector<Seed>& seeds, unsigned char grey=0, int size=1);

void PlotSeeds(const slimage::Image3ub& img, const std::vector<Seed>& seeds, const slimage::Pixel3ub& color=slimage::Pixel3ub{{0,0,0}}, int size=1);

void RenderClusterDisc(const Cluster& cluster, float r, const slimage::Pixel3ub& color);

void RenderClusterNorm(const Cluster& cluster, const ImagePoints& points, float r, const slimage::Pixel3ub& color);

enum ColorMode {
	UniBlack,
	UniWhite,
	Valid,
	Color,
	Depth,
	Gradient,
	GradientNZ,
	ClusterRadius,
	Density,
	Thickness,
	Eccentricity,
	Shape0,
	ShapeX,
	ShapeY,
	ShapeXY,
	ShapeXX,
	ShapeYY,
	AreaQuotient,
	CoverageError
};

//std::vector<slimage::Pixel3ub> ComputePixelColors(const Clustering& c, ColorMode ccm);

slimage::Image3ub PlotPoints(const Superpixels& c, ColorMode pcm);

struct ClusterSelection
{
	std::vector<bool> selection;

	const bool operator[](std::size_t i) const {
		return selection.size() == 0 || selection[i];
	}

	std::vector<bool>::reference operator[](std::size_t i) {
		return selection[i];
	}

	unsigned int countEnabled() const {
		return std::accumulate(selection.begin(), selection.end(), 0u, [](unsigned int sum, bool v) { return sum + (v ? 1 : 0); });
	}

	unsigned int countDisabled() const {
		return selection.size() - countEnabled();
	}

	static ClusterSelection All() {
		return ClusterSelection{ {} };
	}

	static ClusterSelection Empty(std::size_t n) {
		return ClusterSelection{ std::vector<bool>(n, false) };
	}

};

void PlotClusterCenters(const slimage::Image3ub& img, const Superpixels& c, ColorMode ccm, int size, const ClusterSelection& selection);

void PlotClusterPoints(const slimage::Image3ub& img, const Superpixels& c, ColorMode ccm, const ClusterSelection& selection);

void PlotClusterEllipses(const slimage::Image3ub& img, const Superpixels& c, ColorMode ccm, const ClusterSelection& selection);

void PlotClusterEllipsesFilled(const slimage::Image3ub& img, const Superpixels& c, ColorMode ccm, const ClusterSelection& selection);

std::vector<slimage::Pixel3ub> ComputeClusterColors(const Superpixels& c, ColorMode ccm, const ClusterSelection& selection);

enum ClusterMode {
	ClusterCenter,
	ClusterCenterBig,
	ClusterPoints,
	ClusterEllipses,
	ClusterEllipsesFilled
};

void PlotClusters(slimage::Image3ub& img, const Superpixels& c, ClusterMode cm, ColorMode ccm, const ClusterSelection& selection=ClusterSelection::All());

slimage::Image3ub PlotClusters(const Superpixels& c, ClusterMode cm, ColorMode ccm, const ClusterSelection& selection=ClusterSelection::All());

void RenderClusters(const Superpixels& clustering, ColorMode ccm, const ClusterSelection& selection=ClusterSelection::All());

void RenderClusterMap(const Superpixels& clustering, ColorMode ccm, const ClusterSelection& selection=ClusterSelection::All());

/** Renders the superpixel graph in 3D */
template<typename Graph>
void RenderGraph(const Superpixels& clustering, const Graph& graph) {
	glBegin(GL_LINES);
	for(auto eid : as_range(boost::edges(graph))) {
		glVertex3fv(clustering.cluster[boost::source(eid, graph)].center.position.data());
		glVertex3fv(clustering.cluster[boost::target(eid, graph)].center.position.data());
	}
	glEnd();
}

template<typename Graph>
void PlotGraphLines(slimage::Image3ub& vis_img, const Superpixels& clustering, const Graph& graph) {
	for(auto eid : as_range(boost::edges(graph))) {
		const Point& a = clustering.cluster[boost::source(eid, graph)].center;
		const Point& b = clustering.cluster[boost::target(eid, graph)].center;
		slimage::PaintLine(vis_img, a.px, a.py, b.px, b.py, slimage::Pixel3ub{{255,255,255}});
	}
}

template<typename Graph, typename F>
void PlotGraphLines(slimage::Image3ub& vis_img, const Superpixels& clustering, const Graph& graph, F edge_color) {
	for(auto eid : as_range(boost::edges(graph))) {
		const Point& a = clustering.cluster[boost::source(eid, graph)].center;
		const Point& b = clustering.cluster[boost::target(eid, graph)].center;
		slimage::PaintLine(vis_img, a.px, a.py, b.px, b.py, edge_color(eid));
	}
}

template<typename Graph, typename F>
void PlotWeightedGraphLines(slimage::Image3ub& vis_img, const Superpixels& clustering, const Graph& graph, F weight_to_color) {
	for(auto eid : as_range(boost::edges(graph))) {
		const Point& a = clustering.cluster[boost::source(eid, graph)].center;
		const Point& b = clustering.cluster[boost::target(eid, graph)].center;
		float weight = graph[eid];
		slimage::PaintLine(vis_img, a.px, a.py, b.px, b.py, weight_to_color(weight));
	}
}

//----------------------------------------------------------------------------//
}}
//----------------------------------------------------------------------------//
#endif
