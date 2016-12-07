/*
Copyright 2011, Ming-Yu Liu

All Rights Reserved 

Permission to use, copy, modify, and distribute this software and 
its documentation for any non-commercial purpose is hereby granted 
without fee, provided that the above copyright notice appear in 
all copies and that both that copyright notice and this permission 
notice appear in supporting documentation, and that the name of 
the author not be used in advertising or publicity pertaining to 
distribution of the software without specific, written prior 
permission. 

THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
ANY PARTICULAR PURPOSE. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR 
ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN 
AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING 
OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 
*/
#ifndef _m_erc_functions_h_
#define _m_erc_functions_h_

#include "MERCInput.h"
#include <iostream>
#include <cmath>

// Note the potential numerical error. Don't use a very small sigma value.
class MERCFunctions
{
public:

	// compute the similarity scores
	static void ComputeSimilarity(MERCInput &edges,double sigma,int kernel=0);

	// compute the similarity scores with the Gaussian kernel.
	static void ComputeSimilarityGaussian(MERCInput &edges,double sigma);

	// compute the initial loop weight
	static double* ComputeLoopWeight(int nVertices,MERCInput &edges);

	// compute the total weight in the graph
	static double ComputeTotalWeight(double *loop,int nVertices);

	// normalize the edge similarity scores
	static void NormalizeEdgeWeight(MERCInput &edges,double *loop,double wT);

	// compute the edge gain in the entropy rate
	static double ComputeERGain(double wij,double ci,double cj);

	// compute the edge gain in the balancing term
	static double ComputeBGain(int nVertices, int si,int sj);

};

#endif 