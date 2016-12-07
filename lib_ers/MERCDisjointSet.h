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
#ifndef m_disjoint_set_h_
#define m_disjoint_set_h_

#include <iostream>
#include <vector>
#include <cmath>
#include <stack>
#include "MList.h"


class MERCDisjointSet
{
public:
	MERCDisjointSet(int nElements);
	~MERCDisjointSet();
	virtual void Release();

	// set the pixel x with label l
	virtual void Set(int x,int l);

	// find the cluster ID for a given vertex
	virtual int Find(int x);

	// joint the cluster contains vertex y and the cluster contains vertex x
	virtual int Join(int x,int y);

	// return the cluster size containing the vertex x
	int rSize(int x) {return size_[Find(x)];};
	
	// return the number of connected components in the set
	int rNumSets() const {return nElements_;};

	// return the total number of vertices in the set
	int rNumVertices() const {return nVertices_;};

private:

	int *p_;
	int *size_;
	MList<int> *lists_;
	int nElements_;
	int nVertices_;
};




#endif

