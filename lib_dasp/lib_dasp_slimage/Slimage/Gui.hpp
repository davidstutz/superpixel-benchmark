/*
 * Gui.hpp
 *
 *  Created on: Mar 29, 2012
 *      Author: david
 */

#ifndef SLIMAGE_GUI_HPP_
#define SLIMAGE_GUI_HPP_

#include "Slimage.hpp"
#include "IO.hpp"
#include "Types.hpp"
#include "Convert.hpp"
#include <string>

namespace slimage
{
namespace gui
{

	inline void Show(const std::string& caption, const Image3ub& img, unsigned int delay=200) {
		cv::Mat3b img_cv(img.height(), img.width());
		for(unsigned int y=0; y<img.height(); y++) {
			for(unsigned int x=0; x<img.width(); x++) {
				img_cv[y][x][0] = img(x,y)[2];
				img_cv[y][x][1] = img(x,y)[1];
				img_cv[y][x][2] = img(x,y)[0];
			}
		}
		cv::imshow(caption.c_str(), img_cv);
		if(delay > 0) {
			cv::waitKey(delay);
		}
	}

	inline void Show(const std::string& caption, const Image4ub& img, unsigned int delay=200) {
		cv::Mat3b img_cv(img.height(), img.width());
		for(unsigned int y=0; y<img.height(); y++) {
			for(unsigned int x=0; x<img.width(); x++) {
				img_cv[y][x][0] = img(x,y)[2];
				img_cv[y][x][1] = img(x,y)[1];
				img_cv[y][x][2] = img(x,y)[0];
			}
		}
		cv::imshow(caption.c_str(), img_cv);
		if(delay > 0) {
			cv::waitKey(delay);
		}
	}

	inline void Show(const std::string& caption, const Image1ub& img, unsigned int delay=200) {
		cv::Mat1b img_cv(img.height(), img.width());
		for(unsigned int y=0; y<img.height(); y++) {
			for(unsigned int x=0; x<img.width(); x++) {
				img_cv[y][x] = img(x,y);
			}
		}
		cv::imshow(caption.c_str(), img_cv);
		if(delay > 0) {
			cv::waitKey(delay);
		}
	}

	inline void Show(const std::string& caption, const Image1ui16& img, unsigned int min, unsigned int max, unsigned int delay=200) {
		cv::Mat1b img_cv(img.height(), img.width());
		for(unsigned int y=0; y<img.height(); y++) {
			for(unsigned int x=0; x<img.width(); x++) {
				img_cv[y][x] = (255 * (static_cast<unsigned int>(img(x,y)) - min)) / (max - min);
			}
		}
		cv::imshow(caption.c_str(), img_cv);
		if(delay > 0) {
			cv::waitKey(delay);
		}
	}

	inline void Show(const std::string& caption, const Image1f& img, float scl=1.0f, unsigned int delay=200) {
		Image1ub vis;
		conversion::Convert(slimage::Pixel1f{scl} * img, vis);
		Show(caption, vis, delay);
	}

	inline void Wait(unsigned int delay=200) {
		cv::waitKey(delay);
	}

	inline void WaitForKeypress() {
		while(cv::waitKey(50) == -1);
	}

}
}

#endif
