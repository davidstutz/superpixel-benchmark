/*
 * Array.hpp
 *
 *  Created on: Dec 31, 2012
 *      Author: david
 */

#ifndef DASP_ARRAY_HPP_
#define DASP_ARRAY_HPP_

#include <vector>

namespace dasp
{
	/** 2D data structure implemented with an std::vector
	 * - data storage order is row major
	 * - optimal access order is
	 *     for(int i=0; i<cols; i++)
	 *       for(int j=0; j<rows; j++)
	 *         arra(j,i) = 42.0f;
	 */
	template<typename T, typename Index=int>
	struct Array
	{
	public:
		typedef std::vector<T> Container;
		typedef typename Container::iterator it;
		typedef typename Container::const_iterator const_it;
		typedef typename Container::size_type size_t;
		Array() : rows_(0), cols_(0) {}
		Array(Index rows, Index cols) : rows_(rows), cols_(cols), data_(rows*cols) { }
		Array(Index rows, Index cols, const T& t) : rows_(rows), cols_(cols), data_(rows*cols, t) { }
		Container& data() { return data_; }
		const Container& data() const { return data_; }
		it begin() { return data_.begin(); }
		const_it begin() const { return data_.begin(); }
		it end() { return data_.end(); }
		const_it end() const { return data_.end(); }
		size_t size() const { return rows_*cols_; }
		T& operator[](size_t i) { return data_[i]; }
		const T& operator[](size_t i) const { return data_[i]; }
		Index rows() const { return rows_; }
		Index cols() const { return cols_; }
		bool isValid(Index i, Index j) const { return (0 <= i && i < rows_ && 0 <= j && j < cols_); }
		size_t index(Index i, Index j) const { return i + j*rows_; }
		T& operator()(Index i, Index j) { return data_[index(i,j)]; }
		const T& operator()(Index i, Index j) const { return data_[index(i,j)]; }
		 /** DEPRECATED */ Index width() const { return rows_; }
		 /** DEPRECATED */ Index height() const { return cols_; }
	private:
		Index rows_, cols_;
		std::vector<T> data_;
	};
}

#endif
