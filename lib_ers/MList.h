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
#ifndef _m_list_h_
#define _m_list_h_

template <class T>
class MListNode
{
public:
	T data_;
	MListNode *next_;
};


template <class T>
class MList
{

public:
	MList() { 
		size_ = 0;
		first_ = NULL;
		last_ = NULL;
	};
	~MList() 
	{ 
		Clear();
	};

	// Clear everything in the list
	void Clear();

	// Push a new node to the end of the list
	void PushBack(T &data);

	// Append a list to the end of this list
	void Append(MList<T> &other);
	//void Append(MList<T> *other);

	// Return the number of elements in the list
	unsigned int rSize() {return size_;};	

	// Copy the list
	inline MList<T> &operator=(const MList<T> &rhs);


	MListNode<T> *first_;
	MListNode<T> *last_;
	unsigned int size_;
};

template <class T>
MList<T>& MList<T>::operator=(const MList<T> &rhs)
{
	MListNode<T> *tmp;
	for(tmp=rhs.first_;tmp!=NULL;tmp=tmp->next_)
	{
		this->PushBack(tmp->data_);
	}
	return *this;
}

template <class T>
void MList<T>::Append(MList<T> &other)
{
	if(size_==0)
	{
		size_ = other.size_;
		first_ = other.first_;
		last_ = other.last_;
	}
	else
	{
		size_ += other.rSize();
		last_->next_ = other.first_;
		last_ = other.last_;
	}
	
	// We should not release the dynamic memory at this place
	// since they are appended to the end of the other list.
	other.first_ = NULL;
	other.last_ = NULL;
	other.size_ = 0;
}


template <class T>
void MList<T>::Clear()
{
	if(size_ == 0 )
		return;

	MListNode<T> *node = first_;
	while(node!=NULL)
	{
		// pointer to the next
		MListNode<T> *tmp = node->next_;
		// delete the current one
		delete node;
		// link to the next
		node = tmp;
	}

	first_ = NULL;
	last_ = NULL;
	size_ = 0;
}

template <class T>
void MList<T>::PushBack(T &data)
{
	// copy the content
	MListNode<T> *node = new MListNode<T> [1];
	node->data_ = data;	
	node->next_ = NULL;

	// if the list is empty
	if(last_==NULL)
	{
		// update the first node
		first_ = node;
		// update the last node
		last_ = node;
		// increase the size by one
		size_++;
	}
	else
	{
		// insert to the end of the list
		last_->next_ = node;
		// update the last node
		last_ = node;
		// increase the size by one
		size_++;
	}
}


#endif