/*
 * Plotting.cpp
 *
 *  Created on: Feb 22, 2012
 *      Author: david
 */

#include "Superpixels.hpp"
#include "Plots.hpp"
#if defined DASP_HAS_CANDY
#	include <Candy/Primitives.h>
#	include <Candy/GlHelpers.h>
#	include <Danvil/LinAlg/Eigen.hpp>
#endif
#include <Slimage/Paint.hpp>
#include <Slimage/Convert.hpp>
#include <Danvil/Color.h>
#include <Danvil/Color/LAB.h>
#include <Danvil/Color/HSV.h>
#include <Eigen/Geometry>
#include <cmath>
#include <iostream>

/*
* Selbstvergleich mit Parametern (R, T)
* Compression Error
* Static image -> supervoxel -> slices -> BR, USE
* "Isoperimetric/3D", Homogenitaet, Groesse
*/

//----------------------------------------------------------------------------//
namespace dasp {
namespace plots {
//----------------------------------------------------------------------------//

std::vector<slimage::Pixel3ub> CreateRandomColors(unsigned int cnt)
{
	cnt = (cnt / 3 + 1) * 3;
	std::vector<slimage::Pixel3ub> colors(cnt);
	for(unsigned int i=0; i<cnt; i+=3) {
		slimage::Pixel3ub& c1 = colors[i];
		Danvil::convert_hsv_2_rgb((i * 255) / cnt, 255, 255, c1[0], c1[1], c1[2]);
		slimage::Pixel3ub& c2 = colors[i+1];
		Danvil::convert_hsv_2_rgb((i * 255) / cnt, 255, 171, c2[0], c2[1], c2[2]);
		slimage::Pixel3ub& c3 = colors[i+2];
		Danvil::convert_hsv_2_rgb((i * 255) / cnt, 255, 85, c3[0], c3[1], c3[2]);
	}
	std::random_shuffle(colors.begin(), colors.end());
	return colors;
}

void PlotClusterPoints(const slimage::Image3ub& img, const Cluster& cluster, const ImagePoints& points, const slimage::Pixel3ub& color)
{
	assert(cluster.isValid());
	// plot all pixels belonging to the cluster in the color of the cluster center
	for(unsigned int i : cluster.pixel_ids) {
		const Point& p = points[i];
		img(p.px, p.py) = color;
	}
//	// plot the cluster center (using some kind of inverse color)
//	int cx = cluster.center.spatial_x();
//	int cy = cluster.center.spatial_y();
//	if(0 <= cx && cx < int(points.width()) && 0 <= cy && cy < int(points.height())) {
//		unsigned char* col = img.pointer(cx, cy);
//		col[0] = 255 - color[0];
//		col[1] = 255 - color[1];
//		col[2] = 255 - color[2];
//	}
}

void PlotClusters(const slimage::Image3ub& img, const Superpixels& clustering, const std::vector<slimage::Pixel3ub>& colors)
{
	assert(clustering.cluster.size() == colors.size());
	for(unsigned int i=0; i<clustering.cluster.size(); i++) {
		PlotClusterPoints(img, clustering.cluster[i], clustering.points, colors[i]);
	}
}

void PlotClusterEllipse(const slimage::Image3ub& img, const Cluster& cluster, const slimage::Pixel3ub& color, bool filled)
{
	int cx = cluster.center.px;
	int cy = cluster.center.py;

	Eigen::Vector2f g0 = cluster.center.computeGradientDirection();
	float sp_0 = cluster.center.cluster_radius_px;
	float sp_small = sp_0 * cluster.center.computeCircularity();
	int p1x = static_cast<int>(sp_small * g0[0]);
	int p1y = static_cast<int>(sp_small * g0[1]);
	int p2x = static_cast<int>(- sp_0 * g0[1]);
	int p2y = static_cast<int>(+ sp_0 * g0[0]);
//	std::cout << cluster.center.gradient.transpose() << " --- " << sp_0 << " --- " << sp_small << std::endl;
//	std::cout << p1x << "," << p1y << " --- " << p2x << "," << p2y << std::endl;
//	slimage::PaintLine(img, cx, cy, cx + p1x, cy + p1y, color);
//	slimage::PaintLine(img, cx, cy, cx + p2x, cy + p2y, color);
	if(filled) {
		slimage::FillEllipse(img, cx, cy, p1x, p1y, p2x, p2y, color);
	}
	else {
		slimage::PaintEllipse(img, cx, cy, p1x, p1y, p2x, p2y, color);
	}

	//	Eigen::Vector2f p3 = opt.camera.project(cluster.center.world + 0.05f*cluster.center.normal);
	//	int p3x = std::round(p3[0]);
	//	int p3y = std::round(p3[1]);
	//	slimage::PaintLine(img, cx, cy, p3x, p3y, color);
}

template<typename T>
void PlotEdgesImpl(const slimage::Image<T>& img, const slimage::Image1i& labels, const slimage::Pixel<T>& color, unsigned int size, bool internal)
{
	assert(img.dimensions() == labels.dimensions());

	const int dx8[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
	const int dy8[8] = { 0, -1, -1, -1, 0, 1, 1,  1};

	int width = static_cast<int>(img.width());
	int height = static_cast<int>(img.height());

	std::vector<bool> istaken(width*height, false);

	for(int y=0; y<height; y++) {
		for(int x=0; x<width; x++) {
			int i = x + y * width;
			int base_label = labels[i];
			unsigned int np = 0;
			for(unsigned int k=0; k<8; k++) {
				int sx = x + dx8[k];
				int sy = y + dy8[k];
				if(sx >= 0 && sx < width && sy >= 0 && sy < height) {
					int si = sx + sy*width;
					if(internal || istaken[si] == false ) //comment this to obtain internal contours
					{
						if(base_label != labels[si]) {
							np++;
						}
					}
				}
			}
			if(np > size) {
				img[i] = color;
				istaken[i] = true;
			}
		}
	}
}

void PlotEdges(const slimage::Image3ub& img, const slimage::Image1i& labels, const slimage::Pixel3ub& color, unsigned int size, bool internal)
{
	PlotEdgesImpl(img, labels, color, size, internal);
}

void PlotEdges(const slimage::Image1ub& img, const slimage::Image1i& labels, const slimage::Pixel1ub& color, unsigned int size, bool internal)
{
	PlotEdgesImpl(img, labels, color, size, internal);
}

void PlotEdges(const slimage::Image1f& img, const slimage::Image1i& labels, const slimage::Pixel1f& color, unsigned int size, bool internal)
{
	PlotEdgesImpl(img, labels, color, size, internal);
}

template<typename T>
void PlotSeedsImpl(const slimage::Image<T>& img, const std::vector<Seed>& seeds, const slimage::Pixel<T>& color, int size)
{
	for(Seed s : seeds) {
		// round position
		slimage::PaintPoint(img, s.x, s.y, color, size);
	}
}

void PlotSeeds(const slimage::Image1ub& img, const std::vector<Seed>& seeds, unsigned char grey, int size)
{
	PlotSeedsImpl(img, seeds, slimage::Pixel1ub{grey}, size);
}

void PlotSeeds(const slimage::Image3ub& img, const std::vector<Seed>& seeds, const slimage::Pixel3ub& color, int size)
{
	PlotSeedsImpl(img, seeds, color, size);
}

namespace detail
{
	template<int M>
	slimage::Pixel3ub ComputePointColor(const Point&) {
		return {{0,0,0}};
	}

	template<>
	slimage::Pixel3ub ComputePointColor<UniBlack>(const Point& p) {
		return {{0,0,0}};
	}

	template<>
	slimage::Pixel3ub ComputePointColor<UniWhite>(const Point& p) {
		return {{255,255,255}};
	}

	template<>
	slimage::Pixel3ub ComputePointColor<Valid>(const Point& p) {
		return p.is_valid
			? slimage::Pixel3ub{{0,128,0}}
			: slimage::Pixel3ub{{255,128,128}};
	}

	template<>
	slimage::Pixel3ub ComputePointColor<Color>(const Point& p) {
		return RgbColor(p);
	}

	template<>
	slimage::Pixel3ub ComputePointColor<Depth>(const Point& p) {
		return DepthColor(p.depth());
	}

	template<>
	slimage::Pixel3ub ComputePointColor<Gradient>(const Point& p) {
		return p.is_valid
			? GradientColor(p.computeGradient())
			: slimage::Pixel3ub{{0,0,0}};
	}

	template<>
	slimage::Pixel3ub ComputePointColor<GradientNZ>(const Point& p) {
		return p.is_valid
			? IntensityColorBW(std::abs(p.normal[2]),0.0f,1.0f)
			: slimage::Pixel3ub{{0,0,0}};
	}

	template<>
	slimage::Pixel3ub ComputePointColor<ClusterRadius>(const Point& p) {
		return p.is_valid
			? IntensityColor(p.cluster_radius_px, 4.0f, 20.0f)
			: slimage::Pixel3ub{{128,128,128}};
	}

	template<>
	slimage::Pixel3ub ComputePointColor<Density>(const Point& p) {
		float r = p.cluster_radius_px;
		float rho = 1.0f / (3.1415f*r*r);
		return p.is_valid
			? IntensityColor(rho, 0.0f, 0.025f)
			: slimage::Pixel3ub{{128,128,128}};
	}

	template<int M>
	slimage::Pixel3ub ComputeClusterColor(const Cluster& c) {
		return ComputePointColor<M>(c.center);
	}

	template<>
	slimage::Pixel3ub ComputeClusterColor<Thickness>(const Cluster& c) {
		return IntensityColor(c.thickness, 0.0f, 0.05f);
	}

	template<>
	slimage::Pixel3ub ComputeClusterColor<Eccentricity>(const Cluster& c) {
		return IntensityColor(c.eccentricity, 0.0f, 1.0f);
	}

	template<>
	slimage::Pixel3ub ComputeClusterColor<Shape0>(const Cluster& c) {
		return IntensityColor(c.shape_0, 0.5f, 3.5f);
	}

	template<>
	slimage::Pixel3ub ComputeClusterColor<ShapeX>(const Cluster& c) {
		return PlusMinusColor(c.shape_x, 1.0f);
	}

	template<>
	slimage::Pixel3ub ComputeClusterColor<ShapeY>(const Cluster& c) {
		return PlusMinusColor(c.shape_y, 1.0f);
	}

	template<>
	slimage::Pixel3ub ComputeClusterColor<ShapeXY>(const Cluster& c) {
		return PlusMinusColor(c.shape_xy, 10.0f);
	}

	template<>
	slimage::Pixel3ub ComputeClusterColor<ShapeXX>(const Cluster& c) {
		return PlusMinusColor(c.shape_xx, 10.0f);
	}

	template<>
	slimage::Pixel3ub ComputeClusterColor<ShapeYY>(const Cluster& c) {
		return PlusMinusColor(c.shape_yy, 10.0f);
	}

	template<>
	slimage::Pixel3ub ComputeClusterColor<AreaQuotient>(const Cluster& c) {
		float q = std::abs(c.area_quotient - 1.0f);
		return IntensityColor(q, 0.0f, 1.0f);
	}

	template<>
	slimage::Pixel3ub ComputeClusterColor<CoverageError>(const Cluster& c) {
		return IntensityColor(c.coverage_error, 0.0f, 1.0f);
	}

//	template<int M>
//	std::vector<slimage::Pixel3ub> ComputePixelColorsImpl(const dasp::ImagePoints& points)
//	{
//		std::vector<slimage::Pixel3ub> colors(points.size());
//		for(unsigned int i=0; i<points.size(); i++) {
//			colors[i] = ComputePointColor<M>(points[i]);
//		}
//		return colors;
//	}

	template<int M>
	std::vector<slimage::Pixel3ub> ComputeClusterColorsImpl(const Superpixels& clusters, const ClusterSelection& selection)
	{
		std::vector<slimage::Pixel3ub> colors(clusters.cluster.size());
		for(unsigned int i=0; i<clusters.cluster.size(); i++) {
			colors[i] = selection[i] ? ComputeClusterColor<M>(clusters.cluster[i]) : slimage::Pixel3ub{{0,0,0}};
		}
		return colors;
//		if(ccm == Color) {
//			// possibly need to convert back from strange color spaces
//			switch(c.opt.color_space) {
//			case ColorSpaces::RGB: return
//			}
//		}
//		else {
	}

	template<>
	std::vector<slimage::Pixel3ub> ComputeClusterColorsImpl<Color>(const Superpixels& clusters, const ClusterSelection& selection)
	{
		std::vector<slimage::Pixel3ub> colors(clusters.cluster.size());
		for(unsigned int i=0; i<clusters.cluster.size(); i++) {
			if(selection[i]) {
				Eigen::Vector3f color = clusters.ColorToRGB(clusters.cluster[i].center.color);
				slimage::conversion::Convert(slimage::Pixel3f{{color[0], color[1], color[2]}}, colors[i]);
			}
			else {
				colors[i] = slimage::Pixel3ub{{0,0,0}};
			}
		}
		return colors;
	}

	template<int M>
	void PlotPointsImpl(const slimage::Image3ub& img, const Superpixels& c)
	{
		for(size_t i=0; i<img.size(); i++) {
			img[i] = ComputePointColor<M>(c.points[i]);
		}
	}

	template<>
	void PlotPointsImpl<Color>(const slimage::Image3ub& img, const Superpixels& c)
	{
		for(size_t i=0; i<img.size(); i++) {
//			img[i] = c.color_raw[i];
			img[i] = ComputePointColor<Color>(c.points[i]);
		}
	}

}

//#define ComputePixelColors_HELPER(T) case T: return detail::ComputePixelColorsImpl<T>(c.points);
//
//std::vector<slimage::Pixel3ub> ComputePixelColors(const Clustering& c, ColorMode ccm)
//{
//	switch(ccm) {
//	ComputePixelColors_HELPER(UniBlack)
//	ComputePixelColors_HELPER(UniWhite)
//	ComputePixelColors_HELPER(Color)
//	ComputePixelColors_HELPER(Depth)
//	ComputePixelColors_HELPER(Gradient)
//	default: return detail::ComputePixelColorsImpl<-1>(c.points);
//	}
//}

#define ComputeClusterColors_HELPER(T) case T: return detail::ComputeClusterColorsImpl<T>(c, selection);

std::vector<slimage::Pixel3ub> ComputeClusterColors(const Superpixels& c, ColorMode ccm, const ClusterSelection& selection)
{
	switch(ccm) {
	ComputeClusterColors_HELPER(UniBlack)
	ComputeClusterColors_HELPER(UniWhite)
	ComputeClusterColors_HELPER(Color)
	ComputeClusterColors_HELPER(Depth)
	ComputeClusterColors_HELPER(Gradient)
	ComputeClusterColors_HELPER(Thickness)
	ComputeClusterColors_HELPER(Eccentricity)
	ComputeClusterColors_HELPER(Shape0)
	ComputeClusterColors_HELPER(ShapeX)
	ComputeClusterColors_HELPER(ShapeY)
	ComputeClusterColors_HELPER(ShapeXY)
	ComputeClusterColors_HELPER(ShapeXX)
	ComputeClusterColors_HELPER(ShapeYY)
	ComputeClusterColors_HELPER(AreaQuotient)
	ComputeClusterColors_HELPER(CoverageError)
	default: return detail::ComputeClusterColorsImpl<-1>(c, selection);
	}
}

#define PlotPoints_HELPER(T) case T: detail::PlotPointsImpl<T>(img, c); break;

slimage::Image3ub PlotPoints(const Superpixels& c, ColorMode cm)
{
	slimage::Image3ub img(c.width(), c.height());
	img.fill({{0,0,0}});
	switch(cm) {
	PlotPoints_HELPER(UniBlack)
	PlotPoints_HELPER(UniWhite)
	PlotPoints_HELPER(Valid)
	PlotPoints_HELPER(Color)
	PlotPoints_HELPER(Depth)
	PlotPoints_HELPER(Gradient)
	PlotPoints_HELPER(GradientNZ)
	PlotPoints_HELPER(ClusterRadius)
	PlotPoints_HELPER(Density)
	default: detail::PlotPointsImpl<-1>(img, c); break;
	}
	return img;
}

void PlotClusterCenters(const slimage::Image3ub& img, const Superpixels& c, ColorMode ccm, int size, const ClusterSelection& selection)
{
	std::vector<slimage::Pixel3ub> colors = ComputeClusterColors(c, ccm, selection);
	for(size_t i=0; i<c.cluster.size(); i++) {
		if(selection[i]) {
			const Cluster& cluster = c.cluster[i];
			slimage::Pixel3ub color = colors[i];
			if(!cluster.isValid()) {
				color = {{255,0,0}};
			}
			slimage::PaintPoint(img, cluster.center.px, cluster.center.py, color, size);
		}
	}
}

void PlotClusterPoints(const slimage::Image3ub& img, const Superpixels& c, ColorMode ccm, const ClusterSelection& selection)
{
	std::vector<slimage::Pixel3ub> colors = ComputeClusterColors(c, ccm, selection);
	for(size_t i=0; i<c.cluster.size(); i++) {
		if(selection[i]) {
			PlotClusterPoints(img, c.cluster[i], c.points, colors[i]);
		}
	}
}

void PlotClusterEllipses(const slimage::Image3ub& img, const Superpixels& c, ColorMode ccm, const ClusterSelection& selection)
{
	std::vector<slimage::Pixel3ub> colors = ComputeClusterColors(c, ccm, selection);
	for(size_t i=0; i<c.cluster.size(); i++) {
		if(selection[i]) {
			PlotClusterEllipse(img, c.cluster[i], colors[i], false);
		}
	}
}

void PlotClusterEllipsesFilled(const slimage::Image3ub& img, const Superpixels& c, ColorMode ccm, const ClusterSelection& selection)
{
	std::vector<slimage::Pixel3ub> colors = ComputeClusterColors(c, ccm, selection);
	for(size_t i=0; i<c.cluster.size(); i++) {
		if(selection[i]) {
			PlotClusterEllipse(img, c.cluster[i], colors[i], true);
		}
	}
}

void PlotClusters(slimage::Image3ub& img, const Superpixels& c, ClusterMode mode, ColorMode cm, const ClusterSelection& selection)
{
	switch(mode) {
	case ClusterCenter: PlotClusterCenters(img, c, cm, 1, selection); break;
	case ClusterCenterBig: PlotClusterCenters(img, c, cm, 3, selection); break;
	default: case ClusterPoints: PlotClusterPoints(img, c, cm, selection); break;
	case ClusterEllipses: PlotClusterEllipses(img, c, cm, selection); break;
	case ClusterEllipsesFilled: PlotClusterEllipsesFilled(img, c, cm, selection); break;
	}
}

slimage::Image3ub PlotClusters(const Superpixels& c, ClusterMode mode, ColorMode cm, const ClusterSelection& selection)
{
	slimage::Image3ub img(c.width(), c.height());
	img.fill({{0,0,0}});
	PlotClusters(img, c, mode, cm, selection);
	return img;
}

#if defined DASP_HAS_CANDY

void RenderClusterDisc(const Cluster& cluster, float r, const slimage::Pixel3ub& color)
{
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);
	// render circle at position
	Danvil::ctLinAlg::Vec3f pos = Danvil::ctLinAlg::Convert(cluster.center.position);
	const Eigen::Vector3f& n = cluster.center.normal;
	Eigen::Vector3f v = cluster.center.position.cross(n).normalized();
	Eigen::Vector3f u = v.cross(n);
	Danvil::ctLinAlg::Vec3f major = r * Danvil::ctLinAlg::Convert(v);
	Danvil::ctLinAlg::Vec3f minor = r * Danvil::ctLinAlg::Convert(u);
	glColor3ub(color[0], color[1], color[2]);
	Candy::Primitives::RenderEllipseCap(pos, major, minor);
	glColor3f(0.9f, 0.9f, 0.9f);
	Candy::Primitives::RenderSegment(pos, pos + 0.5f * r * Danvil::ctLinAlg::Convert(n));
}

void RenderClusterNorm(const Cluster& cluster, const ImagePoints& points, float r, const slimage::Pixel3ub& color)
{
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_BACK, GL_LINE);
	// render circle
	glColor3ub(color[0], color[1], color[2]);
	float le1 = std::sqrt(cluster.ew(0));
	float le2 = std::sqrt(cluster.ew(1));
	float le3 = std::sqrt(cluster.ew(2));
//	Eigen::Vector3f ec = Eigen::Vector3f::Zero();
//	Eigen::Vector3f ea = cluster.cSigmaScale * le3) * Eigen::Vector3f::Unit(0);
//	Eigen::Vector3f eb = cluster.cSigmaScale * le2) * Eigen::Vector3f::Unit(1);
//	Candy::Primitives::RenderCircle(r);
//	Candy::Primitives::RenderEllipse(
//			Danvil::ctLinAlg::Convert(ec), Danvil::ctLinAlg::Convert(ea), Danvil::ctLinAlg::Convert(eb));
	Candy::Primitives::RenderEllipsoid(le3, le2, le1);
	// construct local transformation
//	Eigen::Vector3f n = cluster.center.normal.normalized();
//	Eigen::Vector3f v = cluster.center.position.cross(n).normalized();
//	Eigen::Vector3f u = v.cross(n);
//	Eigen::Matrix3f R; R << n, v, u;
	auto t =
			Eigen::Translation3f(cluster.center.position)
			* cluster.ev * Eigen::AngleAxisf(M_PI*0.5f, Eigen::Vector3f{0,1,0}).inverse();
	t = t.inverse();
	// render points
	glBegin(GL_LINES);
//	for(unsigned int id : cluster.pixel_ids)
	{	unsigned int id = cluster.pixel_ids[0];
		Eigen::Vector3f p = t * points[id].position;
		glVertex3f(p[0], p[1], p[2]);
		glVertex3f(p[0], p[1], 0);
	}
	glEnd();
	glPointSize(3.0f);
	glBegin(GL_POINTS);
//	for(unsigned int id : cluster.pixel_ids) {
	{	unsigned int id = cluster.pixel_ids[0];
		Eigen::Vector3f p = t * points[id].position;
		glVertex3f(p[0], p[1], p[2]);
	}
	glEnd();
}

#endif

void RenderClusters(const Superpixels& clustering, ColorMode ccm, const ClusterSelection& selection)
{
#if defined DASP_HAS_CANDY
	std::vector<slimage::Pixel3ub> colors = ComputeClusterColors(clustering, ccm, selection);
	for(unsigned int i=0; i<clustering.cluster.size(); i++) {
		if(selection[i]) {
			RenderClusterDisc(clustering.cluster[i], clustering.opt.base_radius, colors[i]);
		}
	}
#else
	std::cerr << "RenderClusters requires Candy 3D engine" << std::endl;
#endif
}

void RenderClusterMap(const Superpixels& clustering, ColorMode ccm, const ClusterSelection& selection)
{
#if defined DASP_HAS_CANDY
	const float cSpacing = 4.0f * clustering.opt.base_radius;
	std::vector<slimage::Pixel3ub> colors = ComputeClusterColors(clustering, ccm, selection);
	unsigned int grid_size = std::ceil(std::sqrt(clustering.cluster.size()));
	for(unsigned int i=0; i<clustering.cluster.size(); i++) {
		if(!selection[i]) {
			continue;
		}
		float x = cSpacing * static_cast<float>(i % grid_size);
		float y = cSpacing * static_cast<float>(i / grid_size);
		glPushMatrix();
		glTranslatef(x, y, 0.0f);
		RenderClusterNorm(clustering.cluster[i], clustering.points, clustering.opt.base_radius, colors[i]);
		glPopMatrix();
	}
#else
	std::cerr << "RenderClusters requires Candy 3D engine" << std::endl;
#endif
}

//----------------------------------------------------------------------------//
}}
//----------------------------------------------------------------------------//
