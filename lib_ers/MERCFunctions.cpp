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
#include "MERCFunctions.h"

using namespace std;

double MERCFunctions::ComputeERGain(double wij,double ci,double cj)
{
	double er = ((wij+ci)*log(wij+ci) + (wij+cj)*log(wij+cj) - ci*log(ci)  - cj*log(cj) - 2*wij*log(wij) )/log(2.0);
	if( er!=er )
		return 0;
	else
		return er;
}

double MERCFunctions::ComputeBGain(int nVertices, int si,int sj)
{
	double Si = si*1.0/nVertices;
	double Sj = sj*1.0/nVertices;
	double b = (-(Si+Sj)*log(Si+Sj) + Si*log(Si) + Sj*log(Sj))/log(2.0) + 1.0;
	return b;
}

void MERCFunctions::NormalizeEdgeWeight(MERCInput &edges,double *loop,double wT)
{
	int nEdges = edges.nEdges_;
	int nVertices = edges.nNodes_;
	for(int i=0;i<nEdges;i++)
	{
		edges.edges_[i].w_ /= wT;
	}

	for(int i=0;i<nVertices;i++)
	{
		loop[i] /= wT;
	}
}

double MERCFunctions::ComputeTotalWeight(double *loop,int nVertices)
{
	double wT = 0;
	for(int i=0;i<nVertices;i++)
		wT += loop[i];
	return wT;
}

double* MERCFunctions::ComputeLoopWeight(int nVertices,MERCInput &edges)
{
	int nEdges = edges.nEdges_;
	double *loop = new double [nVertices];
	//memset(loop,0,sizeof(double)*nVertices);	
	for(int i=0;i<nVertices;i++)
		loop[i] = 0;	
	for(int i=0;i<nEdges;i++)
	{
		loop[ edges.edges_[i].a_ ] += edges.edges_[i].w_;
		loop[ edges.edges_[i].b_ ] += edges.edges_[i].w_;
	}
	return loop;
}


void MERCFunctions::ComputeSimilarity(MERCInput &edges,double sigma,int kernel)
{
	switch(kernel)
	{
	case 0:
		ComputeSimilarityGaussian(edges,sigma);
		break;
	//case 1:
	//	ComputeSimilarityLaplacian(edges,sigma);
	//	break;
	//case 2:
	//	ComputeSimilarityCauchy(edges,sigma);
	//	break;
	//case 3:
	//	ComputeSimilarityRationalQuadratic(edges,sigma);
	//	break;
	//case 4:
	//	ComputeSimilarityInverseMultiquadric(edges,sigma);
	//	break;
	}


}

void MERCFunctions::ComputeSimilarityGaussian(MERCInput &edges,double sigma)
{
	//std::cout<<"[KERNEL] Gaussian"<<std::endl;
	int nEdges = edges.nEdges_;

	double twoSigmaSquare = 2*sigma*sigma;

	for(int i=0;i<nEdges;i++)
	{
		edges.edges_[i].w_ = exp( -(edges.edges_[i].w_*edges.edges_[i].w_)/twoSigmaSquare );
	}
}
