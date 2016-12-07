// @authors:     Fuxin Li
// @contact:     fli@cc.gatech.edu
// @affiliation: Georgia Institute of Technology
// @date:        Fall 2013 - Summer 2014

#include <algorithm>
#include <vector>
#include <numeric>
#include <cmath>
#include "stdint.h"
#include "mex.h"
#include "math.h"

using namespace std;

uint32_t convert_structure(const double *val, const double *edgelet_ids, bool *output_edge, uint32_t num_edgelet, uint32_t max_val, vector<double> *all_vals, double *the_rest)
{
	uint32_t i, count = 0;
	for (i=0;i<num_edgelet;i++)
	{
		uint32_t idx = floor(edgelet_ids[i]) - 1;
		all_vals[idx].push_back(val[i]);
		// Only turn true for nonzeros and length more than one
		if (!output_edge[idx] && val[i] > 0.0)
			output_edge[idx] = true, count++;
	}
	// Prune: one nonzero has been set to true. In this case count was added in the previous loop, undo it
	for (i=0;i<max_val;i++)
	{
		if (all_vals[i].size() == 1 && all_vals[i][0] > 0.0)
			output_edge[i] = false, count--, the_rest[i] = all_vals[i][0];
		else
		{
			// Sort here in order to make const in the next function
			sort(all_vals[i].begin(),all_vals[i].end());
			the_rest[i] = 0.0;
		}
	}
	return count;
}

// Assume all_val has already been sorted
double compute_percentile(const vector<double> &all_val, double percent)
{
	double locval = percent * all_val.size() / 100 + 0.5;
	int32_t n_low = floor(locval) - 1;
	int32_t n_high = ceil(locval) - 1;
	if (n_low < 0)
		n_low = 0;
	if (n_high >= all_val.size())
		n_high = all_val.size()-1;
	// Coincidentally to be just that item..
	if (n_low == n_high)
		return all_val[n_low];
	// Interpolate between n_low and n_high
	locval -= floor(locval);
	return all_val[n_low] * (1.0 - locval) + all_val[n_high] * locval;
}

void compute_mean_medians(const vector<double> *all_vals, const bool *output_edge, uint32_t max_val, double *feats)
{
	uint32_t i, j, counter = 0;
	for (i=0;i<max_val;i++)
	{
		if(!output_edge[i])
			continue;
		for(j=0;j<10;j++)
			feats[counter * 12 + j] = compute_percentile(all_vals[i], (j+1)*10.0);
		feats[counter * 12 + 10] = all_vals[i].size();
		// Average
		feats[counter * 12 + 11] = accumulate(all_vals[i].begin(),all_vals[i].end(),0)/double(all_vals[i].size());
		counter++;
	}
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) 
{
	if (nlhs > 3)
		mexErrMsgTxt("Don't yet support over 3 outputs.");
	if (nrhs < 2)
		mexErrMsgTxt("Need 2 inputs, boundary values and edgelet_ids (both double)");
	double *val = mxGetPr(prhs[0]);
	double *edgelet_ids = mxGetPr(prhs[1]);
	uint32_t num_edgelet = mxGetM(prhs[1]) > mxGetN(prhs[1]) ? mxGetM(prhs[1]) : mxGetN(prhs[1]);
	if (val==0 || edgelet_ids == 0)
		mexErrMsgTxt("Some parameter is not set correctly! Need 2 inputs, boundary values and edgelet_ids (both double)");
	uint32_t max_val = 0;
	for (uint32_t i=0;i<num_edgelet;i++)
	{
		if (edgelet_ids[i] > max_val)
			max_val = floor(edgelet_ids[i]);
	}
	plhs[1] = mxCreateLogicalMatrix(max_val,1);
	bool *output_edge = (bool *) mxGetData(plhs[1]);
	vector<double> *all_vals = new vector<double> [max_val];
	plhs[2] = mxCreateDoubleMatrix(max_val, 1, mxREAL);
	double *the_rest = mxGetPr(plhs[2]);
	uint32_t num_sel = convert_structure(val, edgelet_ids, output_edge, num_edgelet, max_val, all_vals,the_rest);
	plhs[0] = mxCreateDoubleMatrix(12, num_sel,mxREAL);
	double *feats = (double *) mxGetPr(plhs[0]);
	compute_mean_medians(all_vals, output_edge, max_val, feats);
}
