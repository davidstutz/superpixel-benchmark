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
#ifndef _m_submodular_heap_h_
#define _m_submodular_heap_h_
#include "MMFunctions.h"
#include "MHeap.h"
#include "MERCDisjointSet.h"
#include "MERCFunctions.h"

using namespace std;

template <class T>
class MSubmodularHeap: public MHeap<T> 
{
public:

	MSubmodularHeap(int length);
	MSubmodularHeap(T *inarr,int length);

	// An easy update version for tree structure
	void EasyPartialUpdateTree(MERCDisjointSet *u,double balancingTerm,double *loop);


private:

	MERCDisjointSet *u_;
	double *loop_;
	double balancingTerm_;

	// An easy update version for tree structure
	int EasyUpdateValueTree(int i);
	void EasySubmodularMaxHeapifyTree();

};


template <class T>
MSubmodularHeap<T>::MSubmodularHeap(int length): MHeap<T>(length) {}

template <class T>
MSubmodularHeap<T>::MSubmodularHeap(T *inarr,int length): MHeap<T>(inarr,length) {}

//==============================================================================
// Fast update for tree structure
//==============================================================================
template <class T>
void MSubmodularHeap<T>::EasyPartialUpdateTree(MERCDisjointSet *u,double balancingTerm,double *loop)
{
	// access to the disjoint set structure
	u_ = u;
	// keep track the loop value
	loop_ = loop;
	// copy the balancing parameter value.
	balancingTerm_ = balancingTerm;
	
	// A special heap update structure that utilize the submodular property.
	EasySubmodularMaxHeapifyTree();
}

template <class T>
void MSubmodularHeap<T>::EasySubmodularMaxHeapifyTree()
{
	//If the root node value is not updated, then update it
	//If the root node value is updated, then it is the maximum value in the current heap.
	//We don't need to update the other nodes because the dimnishing return property guarantees that the value can only be smaller.
	while(EasyUpdateValueTree(1)==0)
	{
		// If the edge form a loop, remove it from the loop and update the heap.
		if(this->array_[1].gain_ == 0)
			this->HeapExtractMax();
		// Let insert the value into some correct place in the heap.
		else
			this->MaxHeapify(1); // find the maximum one through maxheapify
	}
}


template <class T>
int MSubmodularHeap<T>::EasyUpdateValueTree(int i)
{
	double erGain,bGain;
	// store the old gain
	double oldGain = this->array_[i].gain_;

	int a,b;
	a = u_->Find(  this->array_[i].a_ );
	b = u_->Find(  this->array_[i].b_ );
	
	

	// If the edge forms a cycle, makes the gain zero.
	// Later, we will remove the zero edges from the heap.	
	if( a==b )
	{
		this->array_[i].gain_ = 0;
	}
	else
	{
		// recompute the entropy rate gain
		erGain = MERCFunctions::ComputeERGain( this->array_[i].w_, loop_[this->array_[i].a_]-this->array_[i].w_, loop_[this->array_[i].b_]-this->array_[i].w_);

		// recomptue the balancing gain
		bGain = MERCFunctions::ComputeBGain(u_->rNumVertices(), u_->rSize(a), u_->rSize(b) );
		

		// compute the overall gain
		this->array_[i].gain_ = erGain+balancingTerm_*bGain;
		//array_[i].erGain_ = erGain;
		//array_[i].bGain_ = bGain;
	}

	// If the value is uptodate, we return one. (It will exit the while loop.)
	if(oldGain==this->array_[i].gain_)
		return 1;
	
	// If it is not, then we return zero. (It will trigger another MaxHeapify.)
	return 0;
}



#endif