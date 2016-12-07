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
#ifndef _m_heap_h_
#define _m_heap_h_
#include <stdlib.h>

template <class T>
class MHeap
{
public:
	
	MHeap();
	
	// Construct a heap with $length elements
	MHeap(int length);
	// Construct a heap from an array with $length elements
	MHeap(T *inarr,int length);
	// Destructor
	~MHeap();

	// Extract the top element from a max heap
	T HeapExtractMax();
	// Extract the top element from a min heap
	T HeapExtractMin();

	void MaxHeapify(int i);
	void MinHeapify(int i);

	// Build a max heap
	void BuildMaxHeap();
	// Build a min heap
	void BuildMinHeap();

	void HeapSort();

	int HeapSize() {return nElements_;};

	bool IsEmpty() { return nElements_==0; };

	void PrintHeap(const char *filename);
	void PrintHeapSort(const char *filename);

protected:
	void Release();
	void HeapSortAscend();
	void HeapSortDescend();

	int Left(int i) {return (i<<1);};
	int Right(int i) {return (i<<1)+1;};
	int Parent(int i) {return (i>>1);};
	T* pElement(int i) {return array_+i;};

	// debug function used in developing code
	void CheckMaxHeap();
	// debug function used in developing code
	void CheckMinHeap();

	int maxHeap_;

	T *array_;			// the container
	int length_;		// size of the array
	int nElements_;		// # of elements in the heap
};

template <class T>
MHeap<T>::MHeap() {};

template <class T>
MHeap<T>::MHeap(int length)
{
	maxHeap_ = 1; // max heap by default
	nElements_ = 0;
	length_ = length;
	array_ = new T [length_+1];
}

template <class T>
MHeap<T>::MHeap(T *inarr,int length)
{
	maxHeap_ = 1; // max heap by default
	nElements_ = 0;
	length_ = length;
	array_ = new T [length_+1];
	for(int i=0;i<length_;i++)
		array_[i+1] = inarr[i];
}

template <class T>
MHeap<T>::~MHeap()
{
	Release();
}

template <class T>
T MHeap<T>::HeapExtractMax()
{
	if( HeapSize() < 1)
	{
		std::cout<<"Heap underflow error"<<std::endl;
		exit(0);
	}
	T maxElem = array_[1];
	array_[1] = array_[ HeapSize() ];
	nElements_--;
	MaxHeapify(1);
	return maxElem;
}

template <class T>
T MHeap<T>::HeapExtractMin()
{
	if( HeapSize() < 1)
	{
		std::cout<<"Heap underflow error"<<std::endl;
		exit(0);
	}
	T minElem = array_[1];
	array_[1] = array_[ HeapSize() ];
	nElements_--;
	MinHeapify(1);

	return minElem;
}

template <class T>
void MHeap<T>::HeapSort()
{
	if( maxHeap_ )
		HeapSortAscend();
	else
		HeapSortDescend();
}

template <class T>
void MHeap<T>::HeapSortDescend()
{
	BuildMinHeap();
	for(int i=length_;i>=2;i--)
	{
		// exchange A[1] and A[i]
		T tmp;
		tmp = array_[1];
		array_[1] = array_[i];
		array_[i] = tmp;
		nElements_--;
		MinHeapify(1);
	}
}

template <class T>
void MHeap<T>::HeapSortAscend()
{
	BuildMaxHeap();
	for(int i=length_;i>=2;i--)
	{
		// exchange A[1] and A[i]
		T tmp;
		tmp = array_[1];
		array_[1] = array_[i];
		array_[i] = tmp;
		nElements_--;
		MaxHeapify(1);
	}
}

template <class T>
void MHeap<T>::BuildMinHeap()
{
	maxHeap_ = 0;
	nElements_ = length_;
	int hLength = length_/2;
	for( int i=hLength; i>=1 ; i--)
		MinHeapify(i);
}

template <class T>
void MHeap<T>::BuildMaxHeap()
{
	maxHeap_ = 1;
	nElements_ = length_;
	int hLength = length_/2;
	for( int i=hLength; i>=1 ; i--)
		MaxHeapify(i);
}

template <class T>
void MHeap<T>::MinHeapify(int i)
{
	int left,right,smallest;

	left = Left(i);
	right = Right(i);

	if( left <= HeapSize() && array_[left]<array_[i] )
		smallest = left;
	else
		smallest = i;

	if( right <= HeapSize() && array_[right]<array_[smallest] )
		smallest = right;

	if( smallest != i )
	{
		T tmp;
		tmp = array_[i];
		array_[i] = array_[smallest];
		array_[smallest] = tmp;
		MinHeapify(smallest);
	}
}

template <class T>
void MHeap<T>::MaxHeapify(int i)
{
	int left,right,largest;

	left = Left(i);
	right = Right(i);

	if( left <= HeapSize() && array_[left]>array_[i] )
		largest = left;
	else
		largest = i;

	if( right <= HeapSize() && array_[right]>array_[largest] )
		largest = right;

	if( largest != i )
	{
		T tmp;
		tmp = array_[i];
		array_[i] = array_[largest];
		array_[largest] = tmp;
		MaxHeapify(largest);
	}
}


template <class T>
void MHeap<T>::Release()
{	
	delete [] array_;
}

template <class T>
void MHeap<T>::PrintHeap(const char *filename)
{
	std::ofstream file;
	file.open(filename);
	for(int i=1;i<=HeapSize();i++)
		file<<array_[i]<<std::endl;
	file.close();
}

template <class T>
void MHeap<T>::PrintHeapSort(const char *filename)
{
	std::ofstream file;
	file.open(filename);
	for(int i=1;i<=length_;i++)
		file<<array_[i]<<std::endl;
	file.close();
}

template <class T>
void MHeap<T>::CheckMaxHeap()
{

	for(int i=1;i<=HeapSize();i++)
	{
		// check if the left child is no greater than the parent
		if(Left(i)<=HeapSize())
		{
			if( array_[i] < array_[Left(i)] )
				std::cout<<"Error"<<std::endl;
		}
		// check if the right child is no greater than the parent
		if(Right(i)<=HeapSize())
		{
			if( array_[i] < array_[Right(i)] )
				std::cout<<"Error"<<std::endl;
		}
	}
}

template <class T>
void MHeap<T>::CheckMinHeap()
{

	for(int i=1;i<=HeapSize();i++)
	{
		// check if the left child is no greater than the parent
		if(Left(i)<=HeapSize())
		{
			if( array_[i] > array_[Left(i)] )
				std::cout<<"Error"<<std::endl;
		}
		// check if the right child is no greater than the parent
		if(Right(i)<=HeapSize())
		{
			if( array_[i] > array_[Right(i)] )
				std::cout<<"Error"<<std::endl;
		}
	}
}

#endif