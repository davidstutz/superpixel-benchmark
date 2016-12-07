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
#ifndef _edge_h_
#define _edge_h_

class Edge
{
public:
	int a_;
	int b_;
	double w_;
	double gain_;
	inline Edge &operator=(const Edge &rhs);
	inline bool operator==(const Edge &other) const;
	inline bool operator!=(const Edge &other) const;
	inline bool operator>=(const Edge &other) const;
	inline bool operator<=(const Edge &other) const;
	inline bool operator>(const Edge &other) const;
	inline bool operator<(const Edge &other) const;
};
#endif

Edge &Edge::operator=(const Edge &rhs)
{
	a_ = rhs.a_;
	b_ = rhs.b_;
	w_ = rhs.w_;
	gain_ = rhs.gain_;
	return *this;
}

bool Edge::operator==(const Edge &other) const 
{
	return w_ == other.w_;
}

bool Edge::operator!=(const Edge &other) const
{
	return w_ != other.w_;
}

bool Edge::operator>=(const Edge &other) const
{
	return w_ >= other.w_;
}

bool Edge::operator<=(const Edge &other) const
{
	return w_ <= other.w_;
}

bool Edge::operator>(const Edge &other) const
{
	return w_ > other.w_;
}

bool Edge::operator<(const Edge &other) const
{
	return w_ < other.w_;
}