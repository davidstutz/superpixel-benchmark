// merge_superpixel:
// Merging superpixels for good
// Usage:
// sp_seg = merge_superpixel2(Image, Superpixels, edgelet_sp, edgelet_val, num_edgelets,num_desired,sigma, edge_modifier,color_modifier, starting_threshold);
// where:
// Image: uint8 input image
// Superpixels: uint32 initial superpixel tessellation
// edgelet_sp: special data structure containing the edgelet pairs
// edgelet_val: special data structure containing the edgelet values
// num_edgelets: total number of edgelets (length of edgelet_sp and edgelet_val)
// num_desired: number of requested superpixels
// sigma: controls the size of each superpixel, the larger sigma is, the smaller each superpixel would be
// edge_modifier: controls the step size on edge weights. The larger the step size is, the faster the algorithm, but performance might be lowered slightly.
// color_modifier: controls the step size on color differences. Similar to edge_modifier.
// starting_threshold: Superpixels with color differences and edge differences below starting_threshold will be automatically merged, speeding up the algorithm

#include <stdint.h>
#include <vector>
#include <utility>
#include <list>
#include <algorithm>
#include <queue>
#include <math.h>
#include <mex.h>

using namespace std;

typedef struct{
	// Pixel indices
	uint32_t pix1;
	uint32_t pix2;
	// Superpixels they belong to
	uint32_t sup1;
	uint32_t sup2;
	// Edge values between them
	float edgeval;
}borderpix;

typedef struct{
	uint32_t sup1;
	uint32_t sup2;
	// Maximal edge value
	float max_edgeval;
	// After merging, there could be multiple starting points for an edgelet
	vector<uint32_t> bndry_start;
}edgelet;

typedef struct{
	uint32_t id;
	uint32_t count;
}vertex;

bool borderpix_comp(borderpix p1, borderpix p2)
{
	if (p1.sup1 < p2.sup1)
		return true;
	if (p1.sup1 > p2.sup1)
		return false;
	if (p1.sup2 < p2.sup2)
		return true;
	if (p1.sup2 > p2.sup2)
		return false;
	if (p1.pix1 < p2.pix1)
		return true;
	if (p1.pix1 > p2.pix1)
		return false;
	if (p1.pix2 < p2.pix2)
		return true;
	if (fabs(p1.edgeval) < fabs(p2.edgeval))
		return true;
	return false;
}

bool edgelet_comp(edgelet e1, edgelet e2)
{
	if (e1.sup1 < e2.sup1)
		return true;
	if (e1.sup1 > e2.sup1)
		return false;
	if (e1.sup2 < e2.sup2)
		return true;
	if (e1.sup2 > e2.sup2)
		return false;
	if (e1.max_edgeval < e2.max_edgeval)
		return true;
        return false;
}

bool vertex_comp(vertex p1, vertex p2 )
{
	if (p2.count < p1.count)
		return true;
	if (p2.count > p1.count)
		return false;
	if (p2.id > p1.id)
		return true;
	return false;
}

class SuperPixelMerger
{
protected:
// These 2 pointers are supposed to be maintained outside this class! Please don't delete them when the class instance is not deleted
	const uint8_t *I;
	const float *bndry_map;
	uint8_t I_dims;
	uint32_t width;
	uint32_t height;
	uint32_t max_sup;
	vector<uint32_t> sup_sizes;
	vector<float> r_color;
	vector<float> g_color;
	vector<float> b_color;
	vector<borderpix> bndry_pairs;
	vector<edgelet> edgelet_sp;
	vector<float> edgelet_matrix;
	void collect_info_from_image();
	void collect_edgelets();
	void semi_real_merging(vector<uint32_t> &merge_destination, const vector<bool> &merge_list);
	float compute_small_color_dist(uint32_t loc, uint32_t max_visits);
	void merge_sups(vector<uint32_t> &merge_destination, double thres, double thres2, double sigma, double edge_modifier, double color_modifier, uint16_t iter, double desired_size, bool normal_or_small);
	static void resolve_merge_tree(vector<uint32_t> &merge_destination);
	void find_merge_cliques(vector<uint32_t> &merge_destination, const vector<bool> &merge_list);	
public:
	// NOTE no copying on sp_seg will be performed, it will directly modify the version sent to it in-place
	uint32_t *sp_seg;
	SuperPixelMerger(const uint8_t *I, uint32_t *sp_seg, const float *bndry_map, uint32_t width, uint32_t height, uint8_t I_dims = 3);
	SuperPixelMerger(const uint8_t *I, uint32_t *sp_seg, uint32_t *input_edgelets, float *input_vals, uint32_t num_edgelets, uint32_t width, uint32_t height, uint8_t I_dims = 3);
	// Remove these 2 pointers to prevent them being collected by some garbage collector
	~SuperPixelMerger(){I = NULL; bndry_map = NULL;};
	void Merge(double thres = 0.05, double thres2 = 0.05,double sigma=2.5,double edge_modifier =1.3, double color_modifier = 0.5,  uint32_t num_desired = 32);
};

SuperPixelMerger::SuperPixelMerger(const uint8_t *I, uint32_t *sp_seg, const float *bndry_map, uint32_t width, uint32_t height, uint8_t I_dims)
{
	this->I = I;
	this->sp_seg = sp_seg;
	this->bndry_map = bndry_map;
	this->width = width;
	this->height = height;
	this->I_dims = I_dims;
	
// First step is a loop over the image to 1) Get superpixel size; 2) Get average color; 3) Get the neighborhood pixel pairs
	collect_info_from_image();
// Get neighborhood superpixel pairs
	collect_edgelets();
}

// When we already have bndry_pairs, bndry_vals, edgelet_sp and edgelet_val
SuperPixelMerger::SuperPixelMerger(const uint8_t *I, uint32_t *sp_seg, uint32_t *input_edgelets, float *input_vals, uint32_t num_edgelets, uint32_t width, uint32_t height, uint8_t I_dims)
{
	uint32_t i,j,loc;
	this->I = I;
	this->sp_seg = sp_seg;
	this->bndry_map = NULL;
	this->width = width;
	this->height = height;
	this->I_dims = I_dims;
	
	collect_info_from_image();
	edgelet_matrix.assign(max_sup * max_sup, 0.0);
	loc = 0;
	for (i=0;i<num_edgelets * 2;i+=2)
	{
		edgelet new_edgelet;
		uint32_t sup1 = input_edgelets[i];
		uint32_t sup2 = input_edgelets[i+1];
//		assert(sup1 < sup2);
		new_edgelet.sup1 = sup1;
		new_edgelet.sup2 = sup2;
		for (j=loc;j<bndry_pairs.size();j++)
			if (bndry_pairs[j].sup1 == sup1 && bndry_pairs[j].sup2 == sup2)
				break;
		new_edgelet.max_edgeval = input_vals[i/2];
		new_edgelet.bndry_start.push_back(j);
		loc = j + 1;
		edgelet_sp.push_back(new_edgelet);
		edgelet_matrix[sup1 - 1 + (sup2 - 1)* max_sup] = new_edgelet.max_edgeval;
		edgelet_matrix[sup2 - 1 + (sup1 - 1)* max_sup] = new_edgelet.max_edgeval;
	}
}

// Find the final destination of each merge in a merge tree
void SuperPixelMerger::resolve_merge_tree(vector<uint32_t> &merge_destination)
{
	uint32_t i,j;
	for (i=0;i<merge_destination.size();i++)
	{
		j = i;
		while(merge_destination[j])
		{
			if (j == merge_destination[j] - 1)
				break;
			j = merge_destination[j] - 1;
		}
		merge_destination[i] = j + 1;
	}
}

void SuperPixelMerger::collect_info_from_image()
{
	uint32_t i, j;
	// Initialize max_sup to be at least this size
	max_sup = sp_seg[width*height-1];
	// 1st run over the image
	sup_sizes.resize(max_sup);
	r_color.resize(max_sup);
	g_color.resize(max_sup);
	b_color.resize(max_sup);
	for (i=0;i<width;i++)
		for (j=0;j<height;j++)
		{
			uint32_t idx = i * height + j;
			uint32_t idx_I = (i * height + j) * I_dims;
			uint32_t loc = sp_seg[idx] - 1;
			if (sp_seg[idx] >= max_sup)
			{
				// Get the memory in place before resizing the arrays
				uint32_t reserve_size = std::max(max_sup * 2, sp_seg[idx]);
				sup_sizes.reserve(reserve_size);
				r_color.reserve(reserve_size);
				g_color.reserve(reserve_size);
				b_color.reserve(reserve_size);
				sup_sizes.resize(sp_seg[idx]);
				r_color.resize(sp_seg[idx]);
				g_color.resize(sp_seg[idx]);
				b_color.resize(sp_seg[idx]);
				max_sup = sp_seg[idx];
			}
			// Get the superpixel size
			sup_sizes[loc]++; 
			// Get the RGB color averages
			r_color[loc] += I[idx_I];
			g_color[loc] += I[idx_I+1];
			b_color[loc] += I[idx_I+2];
			// Check whether it's a boundary to another superpixel
			if (j < height - 1 && sp_seg[idx + 1] != sp_seg[idx])
			{
				borderpix newp;
				newp.pix1 = idx;
				newp.pix2 = idx+1;
				if (sp_seg[idx + 1] > sp_seg[idx])
				{
					newp.sup1 = sp_seg[idx];
					newp.sup2 = sp_seg[idx + 1];
					if (bndry_map)
						newp.edgeval = bndry_map[idx] - bndry_map[idx+1];
					else
						newp.edgeval = 0.0;
				}
				else
				{
					newp.sup1 = sp_seg[idx + 1];
					newp.sup2 = sp_seg[idx];
					if (bndry_map)
						newp.edgeval = bndry_map[idx+1] - bndry_map[idx];
					else
						newp.edgeval = 0.0;
				}
				bndry_pairs.push_back(newp);
			}
			if (i < width - 1 && sp_seg[idx + height] != sp_seg[idx])
			{
				borderpix newp;
				newp.pix1 = idx;
				newp.pix2 = idx+height;
				if (sp_seg[idx + height] > sp_seg[idx])
				{
					newp.sup1 = sp_seg[idx];
					newp.sup2 = sp_seg[idx + height];
					if (bndry_map)
						newp.edgeval = bndry_map[idx] - bndry_map[idx+height];
					else
						newp.edgeval = 0.0;
				}
				else
				{
					newp.sup1 = sp_seg[idx + height];
					newp.sup2 = sp_seg[idx];
					if (bndry_map)
						newp.edgeval = bndry_map[idx+height] - bndry_map[idx];
					else
						newp.edgeval = 0.0;
				}
				bndry_pairs.push_back(newp);
			}
		}
	// Clean-up, divide by total for the colors
	for (i = 0;i< max_sup;i++)
	{
		r_color[i] /= (float) sup_sizes[i];
		g_color[i] /= (float) sup_sizes[i];
		b_color[i] /= (float) sup_sizes[i];
	}
	// Sort the boundary pairs
	sort(bndry_pairs.begin(), bndry_pairs.end(), borderpix_comp);
}

void SuperPixelMerger::collect_edgelets()
{
	uint32_t i,j;
	uint32_t sup1 = 0;
	uint32_t sup2 = 0;
	edgelet_matrix.assign(max_sup * max_sup, 0.0);
	// Get edges between superpixels, compute the max edge strength in the meanwhile	
	for (i=0;i<bndry_pairs.size();i++)
	{
		// New pair
		if (bndry_pairs[i].sup1 != sup1 || bndry_pairs[i].sup2 != sup2)
		{
			edgelet new_edgelet;
			new_edgelet.sup1 = bndry_pairs[i].sup1;
			new_edgelet.sup2 = bndry_pairs[i].sup2;
			new_edgelet.max_edgeval = fabs(bndry_pairs[i].edgeval);
			// Add a small number for 0 edge values to create a distinction between unconnected
			if (new_edgelet.max_edgeval == 0)
				new_edgelet.max_edgeval = 1e-10;
			new_edgelet.bndry_start.push_back(i);
			edgelet_sp.push_back(new_edgelet);
			sup1 = bndry_pairs[i].sup1;
			sup2 = bndry_pairs[i].sup2;
			edgelet_matrix[sup1 - 1 + (sup2 - 1)* max_sup] = new_edgelet.max_edgeval;
			edgelet_matrix[sup2 - 1 + (sup1 - 1)* max_sup] = new_edgelet.max_edgeval;
		}
		else if (edgelet_sp.back().max_edgeval < fabs(bndry_pairs[i].edgeval))
			edgelet_sp.back().max_edgeval = fabs(bndry_pairs[i].edgeval);
	}
}

// Find the final destination of all the merges
void SuperPixelMerger::find_merge_cliques(vector<uint32_t> &merge_destination, const vector<bool> &merge_list)
{
// First get a vertex ranking based on what gets merged most
	uint32_t i,j,k;
	uint32_t old_size;
	int32_t growth;
	bool phase;
// frequency also contains the links from each vertex
	vector<vertex> frequency(sup_sizes.size());
	vector<list<uint32_t> > links(sup_sizes.size());
	vector<bool> processed(merge_destination.size(),false);

	vector<bool> merge_matrix(sup_sizes.size() * sup_sizes.size(), false);
	for (i=0;i<sup_sizes.size();i++)
	{
		frequency[i].id = i+1;
		frequency[i].count = 0;
	}
	for (i=0;i<merge_list.size();i++)
	{
		if (merge_list[i])
		{
			frequency[edgelet_sp[i].sup1-1].count++;
			links[edgelet_sp[i].sup1-1].push_back(edgelet_sp[i].sup2);
			frequency[edgelet_sp[i].sup2-1].count++;
			links[edgelet_sp[i].sup2-1].push_back(edgelet_sp[i].sup1);
			uint32_t loc = (edgelet_sp[i].sup1-1) * sup_sizes.size() + edgelet_sp[i].sup2 - 1;
			merge_matrix[loc] = true;
			loc = (edgelet_sp[i].sup2-1) * sup_sizes.size() + edgelet_sp[i].sup1 - 1;
			merge_matrix[loc] = true;
		}
	}
	sort(frequency.begin(), frequency.end(), vertex_comp);
// Starting from every item on the frequency list, generate a maximal clique from it
	for (i=0;i<frequency.size();i++)
	{
		if (!frequency[i].count)
			break;
		list<uint32_t> clique;
		list<uint32_t>::iterator j;
		list<uint32_t>::iterator k;
		list<uint32_t>::iterator it_newstart;
		list<uint32_t>::iterator it_laststart;
		uint32_t iter_start = 1;
		growth = 1;
		if (merge_destination[frequency[i].id-1])
			continue;
		// For growth
		phase = false;
		old_size = 1;
		// The push step
		clique.push_back(frequency[i].id);
		processed[frequency[i].id-1] = true;
		it_newstart = clique.begin();
		it_laststart = clique.begin();
		// Keep doing it while the clique is growing
		while (growth)
		{
			uint32_t count = 0;
			j = it_newstart;
			it_newstart = clique.end();
			it_newstart--;
			while(1)
			{
				for (k=links[(*j)-1].begin();k!=links[(*j)-1].end();k++)
				{
					if (!processed[*k-1])
					{
						clique.push_back(*k);
						processed[*k-1] = true;
					}
				}
				if (j==it_newstart)
					break;
				j++;
			}
			// The validation step
			// Work in-place from the list
			it_newstart++;
			count = 0;
			for (j=clique.begin();j!=clique.end();j++, count++)
			{
				uint32_t loc0 = *j;
				if (count >= old_size)
					k = j, k++;
				else
					k = it_newstart;
				for (;k!=clique.end();k++)
				{
					uint32_t loc = (*j-1) * sup_sizes.size() + *k - 1;
					if (*j == *k || (merge_matrix[loc] == false && edgelet_matrix[loc] > 0))
					{
						bool reset_start = false;
						if (k == it_newstart)
							reset_start = true;
						// Erase it from the clique, set the processed queue back to false
						processed[*k-1] = false;
						k = clique.erase(k);
						if (reset_start)
							it_newstart = k;
						k--;
					}
				}
			}
			iter_start = clique.size();
			int32_t size_change = clique.size() - old_size;
			if (size_change > growth && phase == false)
				growth = size_change;
			if (size_change < growth && phase == false)
				growth = size_change, phase = true, it_laststart = it_newstart;
			if (size_change > growth && phase == true)
			{
				// forfeit this addition, also clear the bridge superpixels from the previous addition, break;
				// Check for bridge superpixels
				for (j = it_laststart;j!= it_newstart;j++)
				{
					uint32_t count_old = 0, count_new = 0;
					for (k=clique.begin();k!=it_newstart;k++)
					{
						uint32_t loc = (*j-1) * sup_sizes.size() + *k - 1;
						uint32_t loc2 = (*k-1) * sup_sizes.size() + *j - 1;
						if(*j != *k && (edgelet_matrix[loc] > 0 || edgelet_matrix[loc2] > 0))
							count_old++;
					}
					for(k=it_newstart;k!=clique.end();k++)
					{
						uint32_t loc = (*j-1) * sup_sizes.size() + *k - 1;
						if (edgelet_matrix[loc] > 0)
							count_new++;
					}
					// Bridge superpixel, remove
					if (count_old < count_new)
					{
						merge_destination[*j-1] = 0;
						processed[*j-1] = false;
						j = clique.erase(j);
						j--;
					}
				}
				clique.erase(it_newstart,clique.end());
				break;
			}
			if (size_change < growth && phase == true)
				growth = size_change, it_laststart = it_newstart;
			old_size = clique.size();
			// Add everything up to this moment to merge_destination
			for (k=it_newstart;k!=clique.end();k++)
			{
				if (*k!=frequency[i].id)
					merge_destination[*k-1] = frequency[i].id;
			}
		}
	}
}

double compute_desired_size(const vector<uint32_t> &sup_sizes, uint32_t num_desired, int32_t num_pixels)
{
	uint32_t i;
	double desired = num_pixels / (double) num_desired;
	double old_desired = desired + 1;
	vector<bool> processed(sup_sizes.size(), false);
	while (old_desired != desired)
	{
		old_desired = desired;
		for (i=0;i<sup_sizes.size();i++)
		{
			if (sup_sizes[i] > desired && !processed[i])
			{
				num_pixels -= sup_sizes[i];
				num_desired--;
				processed[i] = true;
			}
		}
		if (num_desired == 0 || num_pixels <= 0)
			return 0;
		desired = num_pixels / (double) num_desired;
	}
	return desired;
}

// Change most of the vectors in-place, remove some edgelet_sp with a sweep
// Superpixels that get merged will have their sizes set to 0 for an indication
void SuperPixelMerger::semi_real_merging(vector<uint32_t> &merge_destination, const vector<bool> &merge_list)
{
	uint32_t i;
	vector<bool> processing(merge_destination.size(),false);
	find_merge_cliques(merge_destination, merge_list);
	// Update size, color and edgelet_sp
	for (i=0;i<merge_destination.size();i++)
	{
		if (merge_destination[i] && sup_sizes[i])
		{
			uint32_t dest = merge_destination[i] - 1;
			if (!processing[dest])
			{
				// Re-take averages, needs to multiply them
				r_color[dest] *= sup_sizes[dest];
				g_color[dest] *= sup_sizes[dest];
				b_color[dest] *= sup_sizes[dest];
				processing[dest] = true;
			}
			// Merging
			sup_sizes[dest] += sup_sizes[i];
			if (processing[i])
				r_color[dest] += r_color[i], g_color[dest] += g_color[i], b_color[dest] += b_color[i];
			else
				r_color[dest] += r_color[i] * sup_sizes[i],	g_color[dest] += g_color[i] * sup_sizes[i],	b_color[dest] += b_color[i] * sup_sizes[i];
			// Indicator that this superpixel no longer exists
			sup_sizes[i] = 0;
		}
	}
	// Finally divide rgb colors by the new size
	for (i=0;i<merge_destination.size();i++)
	{
		if (processing[i])
		{
			r_color[i] /= sup_sizes[i];
			g_color[i] /= sup_sizes[i];
			b_color[i] /= sup_sizes[i];
		}
	}
	// Update edgelet_sp, just change all the superpixel locations
	for (i=0;i<edgelet_sp.size();i++)
	{
		if (merge_destination[edgelet_sp[i].sup1 - 1])
			edgelet_sp[i].sup1 = merge_destination[edgelet_sp[i].sup1 - 1];
		if (merge_destination[edgelet_sp[i].sup2 - 1])
			edgelet_sp[i].sup2 = merge_destination[edgelet_sp[i].sup2 - 1];
		// Swap the two, always maintain that sup2 > sup1
		if (edgelet_sp[i].sup2 < edgelet_sp[i].sup1)
		{
			uint32_t temp = edgelet_sp[i].sup1;
			edgelet_sp[i].sup1 = edgelet_sp[i].sup2;
			edgelet_sp[i].sup2 = temp;
		}

		// Update edgelet_matrix as well
		if (edgelet_sp[i].sup1 != edgelet_sp[i].sup2)
		{
			uint32_t sup1 = edgelet_sp[i].sup1;
			uint32_t sup2 = edgelet_sp[i].sup2;
			if (edgelet_matrix[sup1 - 1 + (sup2 - 1)* max_sup] < edgelet_sp[i].max_edgeval)
				edgelet_matrix[sup1 - 1 + (sup2 - 1)* max_sup] = edgelet_sp[i].max_edgeval;
			if (edgelet_matrix[sup2 - 1 + (sup1 - 1)* max_sup] < edgelet_sp[i].max_edgeval)
				edgelet_matrix[sup2 - 1 + (sup1 - 1)* max_sup] = edgelet_sp[i].max_edgeval;
		}
	}
	// Re-sort edgelet_sp
	sort(edgelet_sp.begin(), edgelet_sp.end(), edgelet_comp);
	// Finally, prune edgelet_sp so that we retain only the ones with maximal edge values
	int32_t loc = -1;
	uint32_t sup1 = 0;
	uint32_t sup2 = 0;
	for (i=0;i<edgelet_sp.size();i++)
	{
		// An edge to itself, ignore
		if (edgelet_sp[i].sup1 == edgelet_sp[i].sup2)
			continue;
		// New edgelet, put it to loc
		if (edgelet_sp[i].sup1 != sup1 || edgelet_sp[i].sup2 != sup2)
		{
			loc++;
			if (i != loc)
			{
				edgelet_sp[loc] = edgelet_sp[i];
			}
			sup1 = edgelet_sp[i].sup1;
			sup2 = edgelet_sp[i].sup2;
		}
		// Otherwise, note the boundary starting location and possibly the maximal edge value
		else if (loc != i)
		{
			if (edgelet_sp[i].max_edgeval > edgelet_sp[loc].max_edgeval)
				edgelet_sp[loc].max_edgeval = edgelet_sp[i].max_edgeval;
			edgelet_sp[loc].bndry_start.insert(edgelet_sp[loc].bndry_start.end(), edgelet_sp[i].bndry_start.begin(), edgelet_sp[i].bndry_start.end());
		}
	}
	// Get rid of all the rest in edgelet_sp
	if (loc > 0)
		edgelet_sp.resize(loc+1);
	else
		edgelet_sp.erase(edgelet_sp.begin(), edgelet_sp.end());
}

// loc is the location in edgelet_sp
float SuperPixelMerger::compute_small_color_dist(uint32_t loc, uint32_t max_visits)
{
	uint32_t sup_small;
	uint32_t sup_big;
	queue<uint32_t> process_queue;
	float new_r, new_g, new_b;
	vector<bool> visited(width*height,false);
	uint32_t num_visits = 0;
	new_r = new_g = new_b = 0.0;
	if (sup_sizes[edgelet_sp[loc].sup1-1] < sup_sizes[edgelet_sp[loc].sup2-1])
		sup_small = edgelet_sp[loc].sup1, sup_big = edgelet_sp[loc].sup2;
	else
		sup_small = edgelet_sp[loc].sup2, sup_big = edgelet_sp[loc].sup1;
	for (uint32_t j=0;j<edgelet_sp[loc].bndry_start.size();j++)
	{
		uint32_t i = edgelet_sp[loc].bndry_start[j];
		uint32_t sup1 = bndry_pairs[i].sup1;
		uint32_t sup2 = bndry_pairs[i].sup2;
		while(bndry_pairs[i].sup1 == sup1 && bndry_pairs[i].sup2 == sup2)
		{
			// Initialize process_queue, just use the ones in sup_big
			if (sp_seg[bndry_pairs[i].pix1] == sup_big)
				process_queue.push(bndry_pairs[i].pix1);
			else
				process_queue.push(bndry_pairs[i].pix2);
			i++;
		}
	}
	// Use a breadth first search to find the pixels that are closest to the superpixel border
	// that belong to the larger superpixel, compute their mean colors
	while (!process_queue.empty() && num_visits < max_visits)
	{
		uint32_t pix = process_queue.front();
		if (visited[pix])
		{
			process_queue.pop();
			continue;
		}
		new_r += I[pix * I_dims];
		new_g += I[pix * I_dims+1];
		new_b += I[pix * I_dims+2];
		num_visits++;
		// Add new offsets to queue
		if (pix >= height && sp_seg[pix - height] == sup_big)
			process_queue.push(pix - height);
		if (pix > 0 && (pix % height) && sp_seg[pix - 1] == sup_big)
			process_queue.push(pix - 1);
		if (pix < (width-1)*height && sp_seg[pix + height] == sup_big)
			process_queue.push(pix + height);
		if (pix < width*height - 1 && ((pix + 1) % height) && sp_seg[pix + 1] == sup_big)
			process_queue.push(pix + 1);
		process_queue.pop();
	}
	new_r /= num_visits;
	new_g /= num_visits;
	new_b /=  num_visits;
	// Euclidean distance between small superpixel and the floodfill region
	return (r_color[sup_small-1] - new_r) * (r_color[sup_small-1] - new_r) + (g_color[sup_small-1] - new_g) * (g_color[sup_small-1] - new_g) + (b_color[sup_small-1] - new_b) * (b_color[sup_small-1] - new_b);
}

// normal_or_small: false for normal, true for small
// Note this only does semi-real merging, in that it doesn't change sp_seg
void SuperPixelMerger::merge_sups(vector<uint32_t> &merge_destination, double thres, double thres2, double sigma, double edge_modifier, double color_modifier, uint16_t iter, double desired_size, bool normal_or_small)
{
// Go through all of edgelet_sp
	vector<bool> merge_list(edgelet_sp.size(),false);
	for (uint32_t i=0;i<edgelet_sp.size();i++)
	{
		uint32_t min_size = min(sup_sizes[edgelet_sp[i].sup1-1], sup_sizes[edgelet_sp[i].sup2-1]);
		uint32_t max_size = max(sup_sizes[edgelet_sp[i].sup1-1], sup_sizes[edgelet_sp[i].sup2-1]);
		// Check size constraints for normal or small superpixel merging
//		if ((!normal_or_small) && (max_size > min_size * 5 || desired_size > min_size * 5))
//			continue;
		if (normal_or_small && (max_size <= min_size * 5 ))
			continue;
		double thres_modifier = 0.04 * (iter - 1) * exp(- (sigma *min_size) / desired_size);
		if (edgelet_sp[i].max_edgeval < thres + thres_modifier * edge_modifier)
		{
			float color_dist;
			// Only here, we compute the Euclidean distance for color (could be changed to LAB later
			// For normal merging, we use precomputed superpixel color averages
			if (!normal_or_small)
				color_dist = (r_color[edgelet_sp[i].sup1-1] - r_color[edgelet_sp[i].sup2-1]) * (r_color[edgelet_sp[i].sup1-1] - r_color[edgelet_sp[i].sup2-1]) 
				             + (g_color[edgelet_sp[i].sup1-1] - g_color[edgelet_sp[i].sup2-1]) * (g_color[edgelet_sp[i].sup1-1] - g_color[edgelet_sp[i].sup2-1]) 
							 + (b_color[edgelet_sp[i].sup1-1] - b_color[edgelet_sp[i].sup2-1]) * (b_color[edgelet_sp[i].sup1-1] - b_color[edgelet_sp[i].sup2-1]);
			else
			{
				if (sup_sizes[edgelet_sp[i].sup1-1] < sup_sizes[edgelet_sp[i].sup2-1])
					color_dist = compute_small_color_dist(i, sup_sizes[edgelet_sp[i].sup1-1] * 3);
				else
					color_dist = compute_small_color_dist(i, sup_sizes[edgelet_sp[i].sup2-1] * 3);
			}
			color_dist = sqrt(color_dist) / 255.0;
			if (color_dist < thres2 + thres_modifier * color_modifier)
				merge_list[i] = true;
		}
	}
	// Do semi-real merging here, update the sizes and colors, don't update sp_seg as of yet
	semi_real_merging(merge_destination, merge_list);
}

void SuperPixelMerger::Merge(double thres, double thres2, double sigma, double edge_modifier, double color_modifier, uint32_t num_desired)
{
	uint32_t i;
// First try the bottom-right corner for the first estimate of max_superpixel (may not be right)
	uint32_t max_sup = sp_seg[width*height - 1];
// Use this to control where to merge to, instead of performing the actual merging 
	vector<uint32_t> merge_destination;
// Parameter: maximal iterations to be 20 for the moment.
	uint16_t max_iter = 30000;
	uint16_t iter;
	uint32_t count = 0;
	
	merge_destination.assign(sup_sizes.size(), 0);
// Let's first group the ones that are similar in size before we go the small sup elimination routine
	for (iter = 1;iter <= max_iter;iter++)
	{
		count = 0;
		double desired_size = compute_desired_size(sup_sizes, num_desired, width*height);
		merge_sups(merge_destination, thres, thres2, sigma, edge_modifier, color_modifier, iter, desired_size, false);
		// For every 5 iterations, run one small superpixel merging routine
		if (iter % 5 == 0)
		{
			desired_size = compute_desired_size(sup_sizes, num_desired, width*height);
			merge_sups(merge_destination, thres, thres2, sigma, edge_modifier, color_modifier,iter, desired_size, true);
		}
		for (i=0;i<sup_sizes.size();i++)
		{
			if (!merge_destination[i])
				count++;
		}
		if (count <= num_desired * 1.02)
		{
			desired_size = compute_desired_size(sup_sizes, num_desired, width*height);
			merge_sups(merge_destination, thres, thres2, sigma, edge_modifier, color_modifier, iter, desired_size, true);
			break;
		}
	}

	//mexPrintf("Total iterations: %d\n", iter);
	resolve_merge_tree(merge_destination);
// Finally change sp_seg
	for (i = 0;i < width * height;i++)
	{
		if (merge_destination[sp_seg[i]-1])
			sp_seg[i] = merge_destination[sp_seg[i]-1];
	}
}


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) 
{
    mwSize width, height, nsegms;
    mwSize *sizes;
    uint32_t *sp_seg;
	uint32_t *edgelet_sp;
	float *bndry_map;
	uint32_t num_edgelets;
	float *edgelet_val;
	uint8_t *I;
	uint32_t i,j;
	uint8_t I_dims;
	double thres = 0.03, thres2 = 0.03;
	double sigma = 1.5, edge_modifier = 0.8, color_modifier = 0.6;
	uint32_t num_desired = 25;
    
    /* check argument */
    if (nrhs<5) {
		mexPrintf("Usage: merge_superpixel2(Image, Superpixels, edgelet_sp, edgelet_vla, num_edgelets,num_desired,sigma, edge_modifier,color_modifier, starting_threshold);");
		mexPrintf("At least 5 input arguments required (Image, Superpixels, edgelet_sp, edgelet_val, num_edgelets).");
		mexPrintf("num_desired is the number of desired superpixels");
		mexPrintf("sigma controls the size of each superpixel, the larger sigma is, the smaller each superpixel would be.");
		mexPrintf("edge_modifier and color_modifier controls how fast the algorithm aggregates superpixels. They mostly affect the speed of the algorithm rather than the performance.");
		mexErrMsgTxt("starting_threshold indicates that superpixels with color differences and edge differences below starting_threshold will be automatically merged, speeding up the algorithm.");
    }
    if (nlhs>1) {
        mexErrMsgTxt("Too many output arguments");
    }
    /* sizes */
	I_dims = mxGetNumberOfDimensions(prhs[0]);
    sizes = (mwSize *)mxGetDimensions(prhs[0]);
	if (I_dims == 3)
	{
		height = sizes[1];
		width = sizes[2];
	}
	else
	{
		height = sizes[0];
		width = sizes[1];
	}
	if (height == 0 || width == 0)
		mexErrMsgTxt("Empty matrix supplied");

    I = (uint8_t *) mxGetData(prhs[0]);
	if (mxGetClassID(prhs[1]) != mxUINT32_CLASS)
		mexErrMsgTxt("The second argument (superpixels) must be of uint32 category!");
	if (mxGetClassID(prhs[2]) != mxUINT32_CLASS)
		mexErrMsgTxt("The third argument (edgelet_sp) must be of uint32 category!");
	if (mxGetClassID(prhs[3]) != mxSINGLE_CLASS)
		mexErrMsgTxt("The fourth argument (edgelet_val) must be of single precision!");
	sp_seg = (uint32_t *) mxGetData(prhs[1]);
    plhs[0] = mxCreateNumericMatrix(height, width ,mxUINT32_CLASS, mxREAL);
    if (plhs[0]==NULL) {
	    mexErrMsgTxt("Not enough memory for the output matrix");
    }
	// Copy the memory from input to output
    uint32_t *o1 = (uint32_t *) mxGetData(plhs[0]);
	for (i=0;i<width*height;i++)
		o1[i] = sp_seg[i];
	edgelet_sp = (uint32_t *) mxGetData(prhs[2]);
	edgelet_val = (float *) mxGetData(prhs[3]);
	num_edgelets = floor(mxGetScalar(prhs[4]));
	if (nrhs > 5)
		num_desired = floor(mxGetScalar(prhs[5]));
	if (nrhs > 6)
		sigma = mxGetScalar(prhs[6]);
	if (nrhs > 7)
		edge_modifier = mxGetScalar(prhs[7]);
	if (nrhs > 8)
		color_modifier = mxGetScalar(prhs[8]);
        if (nrhs > 9)
                thres = thres2 = mxGetScalar(prhs[9]);
	SuperPixelMerger merger(I, o1, edgelet_sp, edgelet_val, num_edgelets, width, height);
	merger.Merge(thres, thres2, sigma, edge_modifier, color_modifier, num_desired);
}
