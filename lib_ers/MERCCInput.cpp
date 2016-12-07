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
#include "MERCInput.h"
#include <stdlib.h>

MERCInput::MERCInput()
{
	edges_ = NULL;
}

MERCInput::~MERCInput()
{
	Release();
}

void MERCInput::Release()
{
	if(edges_)
		delete [] edges_;
	edges_ = NULL;
}

void MERCInput::ReadFromMatlab(double *pI,double *pJ,double *pW,int nEdges,int nNodes)
{
	nEdges_ = nEdges;
	nNodes_ = nNodes;
	edges_ = new Edge [nEdges_];
	for(int i=0;i<nEdges_;i++)
	{
		edges_[i].a_ = (int)pI[i];
		edges_[i].b_ = (int)pJ[i];
		edges_[i].w_ = pW[i];
	}
}

void MERCInput::Read(const char* filename)
{
	cout<<"Read "<<filename<<endl;
	ifstream file;
	file.open(filename);
	if(!file.is_open())
	{
		cerr<<"Cannot open file "<<filename<<endl;
		exit(-1);	
	}
	file >> nNodes_;
	file >> nEdges_;
	edges_ = new Edge [nEdges_];
	for(int i=0;i<nEdges_;i++)
		file>>edges_[i].a_>>edges_[i].b_>>edges_[i].w_;
	file.close();
}


void MERCInput::Write(const char* filename)
{
	std::ofstream file;
	file.open(filename);
	file.precision(6);
	file<<nNodes_<<endl<<nEdges_<<endl;
	for(int i=0;i<nEdges_;i++)
		file<<edges_[i].a_<<" "<<edges_[i].b_<<" "<<std::fixed<<edges_[i].w_<<std::endl;
	file.close();
}