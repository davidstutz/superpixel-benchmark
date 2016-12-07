/*
 * Segmentation.hpp
 *
 *  Created on: Mar 26, 2012
 *      Author: david
 */

#ifndef GRAPHSEG_LABELING_HPP_
#define GRAPHSEG_LABELING_HPP_

#include "Common.hpp"
#include "as_range.hpp"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <vector>
#include <iostream>

namespace graphseg
{

	/** Node labeling using unique consecutive unsigned integers */
	struct GraphLabeling
	{
		// label for each node
		std::vector<int> labels;

		// number of unique labels
		unsigned int num_labels;

		/** Computes consecutive labels starting with 0 */
		void relabel();

		/** Computes labeling from non-consecutive labels */
		static GraphLabeling CreateClean(const std::vector<int>& labels);

	};

	/** Using connected components to compute graph segments */
	template<typename Graph>
	GraphLabeling ComputeSegmentLabels_ConnectedComponents(const Graph& graph, float threshold);

	/** Using UCM algorithm to compute graph segments */
	template<typename Graph>
	GraphLabeling ComputeSegmentLabels_UCM(const Graph& graph, float threshold);

	namespace ComputeSegmentLabelsStrategies
	{
		enum type {
			ConnectedComponents,
			UCM
		};
	}
	typedef ComputeSegmentLabelsStrategies::type ComputeSegmentLabelsStrategy;

	/** Computes graph segments separated by edges with big weight
	 * Connects all vertices which are connected by edges with a weight smaller than the threshold.
	 * Graph requires edge_weight_t.
	 */
	template<typename WeightedGraph>
	GraphLabeling ComputeSegmentLabels(const WeightedGraph& graph, float threshold, ComputeSegmentLabelsStrategy strategy=ComputeSegmentLabelsStrategies::ConnectedComponents) {
		switch(strategy) {
		default: case ComputeSegmentLabelsStrategies::ConnectedComponents:
			return ComputeSegmentLabels_ConnectedComponents(graph, threshold);
		case ComputeSegmentLabelsStrategies::UCM:
			return ComputeSegmentLabels_UCM(graph, threshold);
		}
	}

	template<typename WeightedGraph>
	GraphLabeling ComputeSegmentLabels_ConnectedComponents(const WeightedGraph& graph, float threshold)
	{
		// create a graph will all edges with costs >= threshold
		SpectralGraph cropped(boost::num_vertices(graph));
		for(auto eid : as_range(boost::edges(graph))) {
			float weight = graph[eid];
			// take only edges with weight < threshold
			if(weight <= threshold) {
				boost::add_edge(boost::source(eid, graph), boost::target(eid, graph), cropped);
			}
		}
		// compute connected components
		std::vector<int> cluster_labels(boost::num_vertices(cropped));
		unsigned int num_labels = boost::connected_components(cropped, &cluster_labels[0]);
		// return result;
		return GraphLabeling{ cluster_labels, num_labels };
	}

	template<typename WeightedGraph>
	GraphLabeling ComputeSegmentLabels_UCM(const WeightedGraph& graph, float threshold)
	{
		// every superpixel is one region
		std::vector<int> cluster_labels(boost::num_vertices(graph));
		for(unsigned int i=0; i<cluster_labels.size(); i++) {
			cluster_labels[i] = i;
		}
		// get edge data
		struct Edge {
			unsigned int a, b;
			float weight;
		};
		std::vector<Edge> edges;
		for(auto eid : as_range(boost::edges(graph))) {
			edges.push_back(Edge{
				static_cast<unsigned int>(boost::source(eid, graph)),
				static_cast<unsigned int>(boost::target(eid, graph)),
				graph[eid]
			});
		}
		// sort edges by weight
		std::sort(edges.begin(), edges.end(), [](const Edge& x, const Edge& y){ return x.weight < y.weight; });
		// cut one edge after another and merge segments
		for(unsigned int k=0; k<edges.size(); k++) {
			if(edges[k].weight >= threshold) {
				break;
			}
			// join segments connected by edge
			int l_old = cluster_labels[edges[k].a];
			int l_new = cluster_labels[edges[k].b];
			std::replace(cluster_labels.begin(), cluster_labels.end(), l_old, l_new);
	#ifdef SEGS_DBG_SHOWGUI
			if(k % 10 == 0) {
				std::cout << "Removed edge with weight " << edges[k].cost << std::endl;
				// show ucm
				relabel();
				slimage::Image3ub vis = computeLabelImage(clusters);
				slimage::gui::Show("ucm", vis);
			}
	#endif
		}
		// create continuous labels
		return GraphLabeling::CreateClean(cluster_labels);
	}

	namespace impl
	{
		struct Edge
		{
			unsigned int a, b;
			float weight;
		};

		/** Prepare labels
		 * Assigns new labels to unsupervised vertices.
		 * Returns the maximal label for supervised vertices.
		 */
		template<typename Graph, typename VertexLabelMap>
		std::vector<int> PrepareLabels(
			const Graph& graph, VertexLabelMap vertex_label_map,
			int* supervised_max_id
		) {
			// find maximal supervised label
			int label_max = -1;
			for(auto vid : as_range(boost::vertices(graph))) {
				const int label = boost::get(vertex_label_map, vid);
				label_max = std::max(label_max, label);
			}
			*supervised_max_id = label_max;
			// give unsupervised vertices a unique label
			std::vector<int> v_labels(boost::num_vertices(graph));
			for(auto vid : as_range(boost::vertices(graph))) {
				int label = boost::get(vertex_label_map, vid);
				if(label < 0) {
					// unsupervised cluster -> need label
					label = ++label_max;
				}
				v_labels[vid] = label;
			}
			return v_labels;
		}

		inline
		void MergeLabels(std::vector<int>& labels, int a, int b) {
			// keep the smaller label (this is the potentially supervised label)
			if(a < b) {
				std::swap(a, b);
			}
			// join segments connected by edge
			std::replace(labels.begin(), labels.end(), a, b);
		}
	}

	/** Supervised labeling
	 * All edges with a weight lower than the threshold are merged.
	 * Edges are not merged if they connect clusters which already have
	 * a label which is not identical for both vertices. 
	 */
	template<typename Graph, typename EdgeWeightMap, typename VertexLabelMap>
	inline std::vector<int> ComputeSegmentLabels_UCM_Supervised(
		const Graph& graph,
		EdgeWeightMap edge_weight_map,
		VertexLabelMap vertex_label_map,
		float threshold)
	{
		// find supervised maximal label and give unique labels to unsupervised vertices
		int label_supervised_threshold;
		std::vector<int> cluster_labels = impl::PrepareLabels(graph, vertex_label_map, &label_supervised_threshold);
		// get all edges with a weight smaller than the threshold
		typedef impl::Edge Edge;
		std::vector<Edge> edges;
		edges.reserve(boost::num_edges(graph));
		for(auto eid : as_range(boost::edges(graph))) {
			float w = boost::get(edge_weight_map, eid);
			if(w < threshold) {
				edges.push_back(Edge{
					static_cast<unsigned int>(boost::source(eid, graph)),
					static_cast<unsigned int>(boost::target(eid, graph)),
					w
				});
			}
		}
		// sort edges by weight
		std::sort(edges.begin(), edges.end(), [](const Edge& x, const Edge& y) { return x.weight < y.weight; });
		// cut one edge after another and merge labels
		for(const Edge& edge : edges) {
			// get cluster labels
			const int label_a = cluster_labels[edge.a];
			const int label_b = cluster_labels[edge.b];
			if(label_a == label_b) {
				// clusters are already part of the same segment -> nothing to do
				continue;
			}
			// check if cluster segments are supervised
			const bool is_supervised_a = (label_a <= label_supervised_threshold);
			const bool is_supervised_b = (label_b <= label_supervised_threshold);
			// do not merge if both clusters are supervised
			if(is_supervised_a && is_supervised_b) {
				continue;
			}
			// merge labels
			impl::MergeLabels(cluster_labels, label_a, label_b);
		}

		// return raw labels
		return cluster_labels;
	}

	namespace impl
	{

		struct ucm_sd_debug {
			std::vector<Edge> force_merge;
			std::vector<Edge> normal_merge;
			std::vector<Edge> reject_merge;
			std::vector<Edge> keep_merge;
			std::vector<Edge> no_merge;
		};
	}

	/** Dynamic supervised labeling
	 * Similar to Supervised Labeling, but can also change supervised labels.
	 */
	template<typename Graph, typename EdgeWeightMap, typename VertexLabelMap>
	inline std::vector<int> ComputeSegmentLabels_UCM_Supervised_Dynamic(
		const Graph& graph,
		EdgeWeightMap edge_weight_map,
		VertexLabelMap vertex_label_map,
		float th_low, float th_normal, float th_high,
		impl::ucm_sd_debug* dbg=0)
	{
		// find supervised maximal label and give unique labels to unsupervised vertices
		int label_supervised_threshold;
		std::vector<int> cluster_labels = impl::PrepareLabels(graph, vertex_label_map, &label_supervised_threshold);
		// get all edges
		typedef impl::Edge Edge;
		std::vector<Edge> edges;
		edges.reserve(boost::num_edges(graph));
		for(auto eid : as_range(boost::edges(graph))) {
			edges.push_back(Edge{
				static_cast<unsigned int>(boost::source(eid, graph)),
				static_cast<unsigned int>(boost::target(eid, graph)),
				boost::get(edge_weight_map, eid)
			});
		}
		// sort edges by weight
		std::sort(edges.begin(), edges.end(), [](const Edge& x, const Edge& y) { return x.weight < y.weight; });
		// collect merged edges
		std::vector<Edge> merged_edges;
		// cut one edge after another and merge clusters into segments
		for(const Edge& edge : edges) {
			// get edge data
			int& label_a = cluster_labels[edge.a];
			int& label_b = cluster_labels[edge.b];
			const bool is_supervised_a = (label_a <= label_supervised_threshold);
			const bool is_supervised_b = (label_b <= label_supervised_threshold);
			const float edge_weight = edge.weight;
			// check low weight
			if(edge_weight < th_low) {
				// always merge edge
				merged_edges.push_back(edge);
				if(dbg) dbg->force_merge.push_back(edge);
				// merge labels now only if not both are supervised
				// when merging supervised labels we need to consider more ...
				if(!(is_supervised_a && is_supervised_b)) {
					impl::MergeLabels(cluster_labels, label_a, label_b);
				}
			}
			// check normal weight
			else if(edge_weight < th_normal) {
				// merge edges and labels now if at least one cluster is not supervised
				if(label_a == label_b || !(is_supervised_a && is_supervised_b)) {
					merged_edges.push_back(edge);
					if(dbg) dbg->normal_merge.push_back(edge);
					if(label_a != label_b) {
						impl::MergeLabels(cluster_labels, label_a, label_b);
					}
				}
				else {
					if(dbg) dbg->reject_merge.push_back(edge);
				}
			}
			// check high weight
			else if(edge_weight < th_high) {
				// merge edge if same label
				if(label_a == label_b) {
					merged_edges.push_back(edge);
					if(dbg) dbg->keep_merge.push_back(edge);
				}
				else {
					if(dbg) dbg->no_merge.push_back(edge);
				}
			}
			else {
				if(dbg) dbg->no_merge.push_back(edge);
			}
		}
		// compute label for new clusters
		int label_max = *std::max_element(cluster_labels.begin(), cluster_labels.end());
		label_max = std::max(label_max, label_supervised_threshold);
		// construct a graph with all merged edges
		SpectralGraph cropped(boost::num_vertices(graph));
		for(const Edge& e : merged_edges) {
			auto p = boost::add_edge(e.a, e.b, cropped);
			cropped[p.first] = e.weight;
		}
		// compute connected components
		std::vector<int> cluster_labels_components(boost::num_vertices(cropped));
		unsigned int num_labels = boost::connected_components(cropped, &cluster_labels_components[0]);
		// std::cout << "CC# = " << num_labels << std::endl;
		// check if two components have the same label index
		std::map<int, std::map<unsigned int, unsigned int>> label_component_count;
		for(unsigned int i=0; i<cluster_labels.size(); i++) {
			int label = cluster_labels[i];
			unsigned int cmp_id = cluster_labels_components[i];
			label_component_count[label][cmp_id] ++;
		}
		for(auto p : label_component_count) {
			if(p.second.size() > 1) {
				// std::cout << "Label " << p.first << ": " << std::endl;
				// std::cout << "\tcmp\tcnt" << std::endl;
				unsigned int max_cmp = 0;
				unsigned int max_num = 0;
				for(auto q : p.second) {
					if(q.second >= max_num) {
						max_cmp = q.first;
						max_num = q.second;
					}
				}
				std::map<unsigned int, int> label_remap;
				for(auto q : p.second) {
					if(q.first != max_cmp) {
						label_remap[q.first] = ++label_max;
					}
					else {
						label_remap[q.first] = p.first;
					}
					// std::cout << "\t" << q.first << "\t" << q.second << "\t->" << label_remap[q.first] << std::endl;
				}
				for(unsigned int i=0; i<cluster_labels.size(); i++) {
					if(cluster_labels[i] == p.first) {
						cluster_labels[i] = label_remap[cluster_labels_components[i]];
					}
				}
			}
		}
		// check if one component contains more than one label
		std::map<int, std::map<unsigned int, unsigned int>> component_label_count;
		for(unsigned int i=0; i<cluster_labels.size(); i++) {
			int label = cluster_labels[i];
			unsigned int cmp_id = cluster_labels_components[i];
			component_label_count[cmp_id][label] ++;
		}
		for(auto p : component_label_count) {			
			unsigned int max_label = 0;
			unsigned int max_num = 0;
			for(auto q : p.second) {
				if(q.second >= max_num) {
					max_label = q.first;
					max_num = q.second;
				}
			}
			for(auto q : p.second) {
				impl::MergeLabels(cluster_labels, q.first, max_label);
			}
		}		
		// return raw labels
		return cluster_labels;
	}

}

#endif
