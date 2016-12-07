/*
 * Mipmap.hpp
 *
 *  Created on: Mar 18, 2012
 *      Author: david
 */

#ifndef MIPMAP_HPP_
#define MIPMAP_HPP_

#include "Slimage.hpp"
#include <algorithm>

namespace slimage
{

	template<typename K>
	Image<Traits<K,3>> ConvertToOpenGl(const Image<Traits<K,3>>& img)
	{
		unsigned int size = 1;
		unsigned int w = img.width();
		unsigned int h = img.height();
		while(size < w || size < h) {
			size <<= 1;
		}
		Image<Traits<K,3>> glImg(size, size);
		for(unsigned int i=0; i<size; i++) {
			K* dst = glImg.scanline(i);
			const K* src = img.scanline(i);
			unsigned int a;
			if( i < h ) {
				// copy first part of line with src data
				a = 3 * sizeof(K) * img.width();
				std::copy(src, src + a, dst);
			} else {
				// first part is empty because no src data for this line
				a = 0;
			}
			// fill rest of line with zeros
			std::fill(dst + a, dst + 3 * sizeof(K) * glImg.width(), static_cast<unsigned char>(0));
		}
		return glImg;
	}

}

#endif
