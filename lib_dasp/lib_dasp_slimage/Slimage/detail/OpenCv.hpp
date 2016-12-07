/*
 * OpenCv.hpp
 *
 *  Created on: Mar 2, 2012
 *      Author: david
 */

#ifndef SLIMAGE_DETAIL_OPENCV_HPP_
#define SLIMAGE_DETAIL_OPENCV_HPP_
//----------------------------------------------------------------------------//
#include "../Slimage.hpp"
#include <opencv2/highgui/highgui.hpp>
//----------------------------------------------------------------------------//
namespace slimage {
//----------------------------------------------------------------------------//

namespace opencv {

	inline cv::Mat ConvertToOpenCv(const ImagePtr& pimg) {
		if(pimg->channelCount() == 1) {
			Image1ub img = Ref<unsigned char,1>(pimg);
			cv::Mat mat(img.height(),img.width(),CV_8UC3);
			for(unsigned int y=0; y<img.height(); y++) {
				for(unsigned int x=0; x<img.width(); x++) {
					mat.ptr<unsigned char>(y, x)[0] = img(x,y);
					mat.ptr<unsigned char>(y, x)[1] = img(x,y);
					mat.ptr<unsigned char>(y, x)[2] = img(x,y);
				}
			}
			return mat;
		}
		else {
			Image3ub img = Ref<unsigned char,3>(pimg);
			cv::Mat mat(img.height(),img.width(),CV_8UC3);
			for(unsigned int y=0; y<img.height(); y++) {
				for(unsigned int x=0; x<img.width(); x++) {
					mat.ptr<unsigned char>(y, x)[2] = img(x,y)[0];
					mat.ptr<unsigned char>(y, x)[1] = img(x,y)[1];
					mat.ptr<unsigned char>(y, x)[0] = img(x,y)[2];
				}
			}
			return mat;
		}
	}

	inline ImagePtr ConvertFromOpenCv(const cv::Mat& mat) {
		if(mat.type() == CV_8UC1) {
			Image1ub img(mat.cols, mat.rows);
			for(unsigned int y=0; y<img.height(); y++) {
				for(unsigned int x=0; x<img.width(); x++) {
					img(x,y) = Pixel1ub{mat.ptr<unsigned char>(y, x)[0]};
				}
			}
//			img.copyFrom(mat.begin<unsigned char>());
			return Ptr(img);
		}
		else if(mat.type() == CV_8UC3) {
			Image3ub img(mat.cols, mat.rows);
			for(unsigned int y=0; y<img.height(); y++) {
				for(unsigned int x=0; x<img.width(); x++) {
					img(x,y) = Pixel3ub{{
							mat.ptr<unsigned char>(y, x)[2],
							mat.ptr<unsigned char>(y, x)[1],
							mat.ptr<unsigned char>(y, x)[0] }};
				}
			}
//			img.copyFrom(mat.begin<unsigned char>());
			return Ptr(img);
		}
		else {
			return ImagePtr();
		}
	}

	inline void Save(const ImagePtr& img, const std::string& filename) {
		cv::Mat mat = ConvertToOpenCv(img);
		cv::imwrite(filename, mat);
	}

	inline ImagePtr Load(const std::string& filename) {
		cv::Mat mat = cv::imread(filename);
		if(mat.rows == 0 && mat.cols == 0) {
			throw IoException(filename, "Empty image (does the file exists?)");
		}
		return ConvertFromOpenCv(mat);
	}

}

//----------------------------------------------------------------------------//
}
//----------------------------------------------------------------------------//
#endif
