/*
 * ReapairDepth.cpp
 *
 *  Created on: Mar 27, 2012
 *      Author: david
 */

#include "RepairDepth.hpp"
#include <Slimage/Parallel.h>
#include <iostream>

namespace dasp {

void RepairDepthFill(const slimage::Image1ui16& depth, const slimage::Image3ub& color)
{
	std::cout << "Repairing depth..." << std::endl;
	for(unsigned int i=0; i<depth.size(); i++) {
		if(depth[i] == 0) {
			depth[i] = 1000;
		}
	}

}

void RepairDepthGrow(const slimage::Image1ui16& depth, const slimage::Image3ub& color)
{
	slimage::Image1ui16 depth_raw = depth.clone();
	int w = depth.width();
	const unsigned int cSamples = 8;
	int neighbor_offsets[cSamples] = {
			-1-w, -w, +1-w,
			-1  ,     +1  ,
			-1+w, +w, +1+w
	};
	int weights[cSamples] = {
			1, 2, 1,
			2,    2,
			1, 2, 1
	};
	for(unsigned int y=1; y<depth.height()-1; y++) {
		for(unsigned int x=1; x<depth.width()-1; x++) {
			unsigned int center_index = x + y * w;
			if(depth[center_index] != 0) {
				continue;
			}
			int sum_w = 0;
			int sum_v = 0;
			for(unsigned int i=0; i<cSamples; i++) {
				uint16_t v = depth_raw[center_index + neighbor_offsets[i]];
				if(v == 0) continue;
				int w = weights[i];
				sum_w += w;
				sum_v += w * static_cast<int>(v);
			}
			depth(x,y) = (sum_w > 0) ? static_cast<uint16_t>(sum_v / sum_w) : 0;
		}
	}
}

struct ExtrapolateDepth
{
	ExtrapolateDepth(const std::vector<uint16_t>& data)
	: data_(data) {
		BOOST_ASSERT(data_.size() > 0);
		solveConst();
//		solveSimple();
//		solveFit();
	}

	uint16_t operator()(unsigned int i) const {
		return static_cast<uint16_t>(lin_t_ + lin_m_ * static_cast<float>(i));
//		return data_[data_.size() - i % std::min<unsigned>(4, data_.size())];
	}

private:
	void solveConst() {
		lin_t_ = 0.0f;
		unsigned int n = std::min<unsigned int>(4,data_.size());
		for(unsigned int i=0; i<n; i++) {
			lin_t_ += static_cast<float>(data_[data_.size() - 1 - i]);
		}
		lin_t_ /= static_cast<float>(n);
		lin_m_ = 0.0f;
	}

	void solveSimple() {
		int i = std::min(10, static_cast<int>(data_.size()));
		float a = data_[data_.size() - i];
		float b = data_.back();
		if(i == 1) {
			lin_m_ = 0;
		}
		else {
			lin_m_ = (b - a) / static_cast<float>(-1-i);
		}
		lin_t_ = b + lin_m_;
	}

	void solveFit() {
		float c1=0.0f, c2=0.0f, c3=0.0f, c4=0.0f;
		for(int i=-1; i >= -std::min(10, static_cast<int>(data_.size())-1); i--) {
			float y = static_cast<float>(data_[static_cast<int>(data_.size())+i]);
			float x = static_cast<float>(i);
			c1 += x * y;
			c2 += x;
			c3 += y;
			c4 += x*x;
		}
		float q = static_cast<float>(data_.size());
		lin_m_ = (c1 - c2*c3/q) / (c4 - c2*c2/q);
		lin_t_ = c3/q - lin_m_ * c2/q;
	}

private:
	std::vector<uint16_t> data_;
	float lin_m_, lin_t_;
};

void RepairDepthSmart(const slimage::Image1ui16& depth)
{
	// we exploit the specific setup of the kinect camera to close gaps in the depth image
	unsigned int w = depth.width();
	// process all scanlines
	for(unsigned int y=0; y<depth.height(); y++) {
		// go until we find a pixel with valid depth info
		unsigned int x = 0;
		while(x < w && depth(x,y) == 0) x++;
		unsigned int x_left_valid = x;
		// now look right for a hole
		while(x < w && depth(x,y) != 0) x++;
		unsigned int x_left = x;
		// fill holes
		while(true) {
			// now look right for the next valid pixel
			while(x < w && depth(x,y) == 0) x++;
			if(x == w) {
				break;
			}
			unsigned int x_right = x;
			// now run again until we find a hole
			while(x < w && depth(x,y) != 0) x++;
			unsigned int x_right_valid = x;
//			std::cout << y << ": Repairing from " << x_left_valid << "/" << x_left << " to " << x_right << "/" << x_right_valid << std::endl;
			// fill whole from the further away pixel
			if(depth(x_left-1,y) > depth(x_right,y)) {
				// fill left to right
				std::vector<uint16_t> data(depth(x_left_valid,y).pointer(), depth(x_left,y).pointer());
				ExtrapolateDepth xd(data);
				BOOST_ASSERT(x_right > x_left);
				for(unsigned int i=0; i<x_right - x_left; i++) {
					depth(x_left + i, y) = xd(i);
				}
			}
			else {
				// fill right to left
				std::vector<uint16_t> data(x_right_valid - x_right);
				for(unsigned int i=0; i<data.size(); i++) {
					data[i] = depth(x_right_valid-i-1, y);
				}
				ExtrapolateDepth xd(data);
				BOOST_ASSERT(x_right > x_left);
				for(unsigned int i=0; i<x_right - x_left; i++) {
					depth(x_right - i - 1, y) = xd(i);
				}
			}
			x_left_valid = x_right;
			x_left = x_right_valid;
		}
	}
}

void RepairDepth(const slimage::Image1ui16& depth, const slimage::Image3ub& color)
{
	RepairDepthSmart(depth);
}

void SmoothDepth9x9(const slimage::Image1ui16& depth)
{
	slimage::Image1ui16 depth_raw = depth.clone();
	int w = depth.width();
	const unsigned int cSamples = 9;
	int neighbor_offsets[cSamples] = {
			-1-w, -w, +1-w,
			-1  ,  0, +1  ,
			-1+w, +w, +1+w
	};
	int weights[cSamples] = {
			1, 2, 1,
			2, 4, 2,
			1, 2, 1
	};
	for(unsigned int y=1; y<depth.height()-1; y++) {
		for(unsigned int x=1; x<depth.width()-1; x++) {
			unsigned int center_index = x + y * w;
			int sum_w = 0;
			int sum_v = 0;
			for(unsigned int i=0; i<cSamples; i++) {
				uint16_t v = depth_raw[center_index + neighbor_offsets[i]];
				if(v == 0) continue;
				int w = weights[i];
				sum_w += w;
				sum_v += w * static_cast<int>(v);
			}
			depth(x,y) = (sum_w > 0) ? static_cast<uint16_t>(sum_v / sum_w) : 0;
		}
	}
}

inline uint16_t KinectBorderSensitiveSmooth(unsigned int vmm, unsigned int vm, unsigned int v0, unsigned int vp, unsigned int vpp)
{
	bool left_invalid = (vmm == 0 || vm == 0);
	bool right_invalid = (vpp == 0 || vp == 0);
	if(left_invalid && right_invalid) {
		return 0;
	}
	else if(left_invalid) {
		return (4*v0 + 3*vp + vpp)/8;
	}
	else if(right_invalid) {
		return (4*v0 + 3*vm + vmm)/8;
	}
	else {
		if(std::abs(static_cast<int>(vpp) - static_cast<int>(v0)) < std::abs(static_cast<int>(vmm) - static_cast<int>(v0))) {
			return (4*v0 + 3*vp + vpp)/8;
		}
		else {
			return (4*v0 + 3*vm + vmm)/8;
		}
	}
}

void SmoothDepthY(const slimage::Image1ui16& depth)
{
	slimage::Image1ui16 depth_raw = depth.clone();
	int w = depth.width();
	int neighbor_offsets[5] = {
			-2*w,
			-1*w,
			 0,
			+1*w,
			+2*w,
	};
	for(unsigned int y=4; y<depth.height()-4; y++) {
		for(unsigned int x=0; x<depth.width(); x++) {
			unsigned int center_index = x + y * w;
			unsigned int v[5];
			for(unsigned int i=0; i<5; i++) {
				v[i] = depth_raw[center_index + neighbor_offsets[i]];
			}
			depth(x,y) = KinectBorderSensitiveSmooth(v[0], v[1], v[2], v[3], v[4]);
		}
	}
}

void SmoothDepthX(const slimage::Image1ui16& depth)
{
	slimage::Image1ui16 depth_raw = depth.clone();
	int w = depth.width();
	int neighbor_offsets[5] = {
			-2, -1, 0, +1, +2
	};
	for(unsigned int y=0; y<depth.height(); y++) {
		for(unsigned int x=4; x<depth.width()-4; x++) {
			unsigned int center_index = x + y * w;
			unsigned int v[5];
			for(unsigned int i=0; i<5; i++) {
				v[i] = depth_raw[center_index + neighbor_offsets[i]];
			}
			depth(x,y) = KinectBorderSensitiveSmooth(v[0], v[1], v[2], v[3], v[4]);
		}
	}
}

void SmoothDepth(const slimage::Image1ui16& depth, const slimage::Image3ub& color)
{
//	SmoothDepth9x9(depth);
	SmoothDepthY(depth);
	SmoothDepthX(depth);
}

}
