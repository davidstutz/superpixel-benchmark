/* Rendering.hpp
 *
 * Created on: Dec 30, 2012
 * AUthor: david
 */

#include <GL/glew.h>
#include <graphseg/as_range.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <Eigen/Dense>

namespace graphseg
{
	namespace detail
	{
		inline void GlVertex(const Eigen::Vector3f& v) {
			glVertex3f(v[0], v[1], v[2]);
		}
		inline void GlColor(const Eigen::Vector3f& v) {
			glColor3f(v[0], v[1], v[2]);
		}
	}

	template<typename Graph, typename VertexRenderFunc>
	void RenderVertices(const Graph& graph, VertexRenderFunc vf) {
		for(const auto& vid : as_range(boost::vertices(graph))) {
			vf(vid);
		}
	}

	template<typename Graph, typename EdgeRenderFunc>
	void RenderEdges(const Graph& graph, EdgeRenderFunc ef) {
		for(const auto& eid : as_range(boost::edges(graph))) {
			ef(eid, boost::source(eid, graph), boost::target(eid, graph));
		}
	}

	/** Render the edges of a graph using vertex coordinates and edge colors */
	template<typename Graph, typename VertexCoordinateFunc, typename EdgeColorFunc>
	void RenderEdges3D(const Graph& graph, VertexCoordinateFunc vxf, EdgeColorFunc ecf) {
		typedef typename Graph::vertex_descriptor vid_t;
		typedef typename Graph::edge_descriptor eid_t;
		glBegin(GL_LINES);
		RenderEdges(graph, 
			[&vxf,&ecf](const eid_t& eid, const vid_t& vida, const vid_t& vidb) {
				detail::GlColor(ecf(eid));
				detail::GlVertex(vxf(vida));
				detail::GlVertex(vxf(vidb));
			}
		);
		glEnd();
	}

	/** Render the edges of a graph using vertex coordinates and vertex colors */
	template<typename Graph, typename VertexCoordinateFunc, typename VertexColorFunc>
	void RenderEdges3DVcol(const Graph& graph, VertexCoordinateFunc vxf, VertexColorFunc vcf) {
		typedef typename Graph::vertex_descriptor vid_t;
		typedef typename Graph::edge_descriptor eid_t;
		glBegin(GL_LINES);
		RenderEdges(graph, 
			[&vxf,&vcf](const eid_t&, const vid_t& vida, const vid_t& vidb) {
				detail::GlColor(vcf(vida));
				detail::GlVertex(vxf(vida));
				detail::GlColor(vcf(vidb));
				detail::GlVertex(vxf(vidb));
			}
		);
		glEnd();
	}


}
