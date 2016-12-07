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
#ifndef _m_erc_edge_h_
#define _m_erc_edge_h_

#include <iostream>
#include <fstream>

using namespace std;

class MERCEdge
{
public:
	int a_;
	int b_;
	double w_;
	double gain_;

	inline MERCEdge &operator=(const MERCEdge &rhs);
	inline bool operator==(const MERCEdge &other) const;
	inline bool operator!=(const MERCEdge &other) const;
	inline bool operator>=(const MERCEdge &other) const;
	inline bool operator<=(const MERCEdge &other) const;
	inline bool operator>(const MERCEdge &other) const;
	inline bool operator<(const MERCEdge &other) const;

	inline friend ostream &operator<<(ostream &output,MERCEdge &e);
};



ostream &operator<<(ostream &output,MERCEdge &e)
{
	output<< "(" <<  e.a_ <<", " << e.b_ <<", " << e.w_ <<", " << e.gain_ <<")";
    return output;
}


MERCEdge &MERCEdge::operator=(const MERCEdge &rhs)
{
	a_ = rhs.a_;
	b_ = rhs.b_;
	w_ = rhs.w_;
	gain_ = rhs.gain_;
	return *this;
}

bool MERCEdge::operator==(const MERCEdge &other) const
{
	return gain_ == other.gain_;
}

bool MERCEdge::operator!=(const MERCEdge &other) const
{
	return gain_ != other.gain_;
}

bool MERCEdge::operator>=(const MERCEdge &other) const
{
	return gain_ >= other.gain_;
}

bool MERCEdge::operator<=(const MERCEdge &other) const
{
	return gain_ <= other.gain_;
}

bool MERCEdge::operator>(const MERCEdge &other) const
{
	return gain_ > other.gain_;
}

bool MERCEdge::operator<(const MERCEdge &other) const
{
	return gain_ < other.gain_;
}

#endif