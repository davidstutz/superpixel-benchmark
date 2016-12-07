/*
 * io.hpp
 *
 *  Created on: Feb 5, 2012
 *      Author: david
 */

#ifndef SLIMAGE_IO_HPP_
#define SLIMAGE_IO_HPP_

#include <string>
#include <stdexcept>

namespace slimage
{

	struct IoException
	: public std::runtime_error
	{
	public:
		IoException(const std::string& filename, const std::string& msg)
		: std::runtime_error("Error with file '" + filename + "': " + msg) {}
	};

}

#if defined SLIMAGE_IO_QT
	#include "detail/Qt.hpp"
#elif defined SLIMAGE_IO_OPENCV
	#include "detail/OpenCv.hpp"
#endif

#if defined SLIMAGE_IO_GZ
#include <Danvil/IO/Z.h>
#endif

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>

namespace slimage
{

#if defined SLIMAGE_IO_QT

	inline void Save(const ImagePtr& img, const std::string& filename) {
		qt::Save(img, filename);
	}

	inline ImagePtr Load(const std::string& filename) {
		return qt::Load(filename);
	}

#elif defined SLIMAGE_IO_OPENCV

	inline void Save(const ImagePtr& img, const std::string& filename) {
		opencv::Save(img, filename);
	}

	inline ImagePtr Load(const std::string& filename) {
		return opencv::Load(filename);
	}

#endif

	/** Loads a 16 bit 1-channel image from an ASCII PGM file */
	inline Image1ui16 Load1ui16(const std::string& filename) {
		if(!boost::algorithm::ends_with(filename, ".pgm")) {
			throw IoException(filename, "Load1ui16 can only handle PGM files");
		}
		std::ifstream ifs(filename);
		if(!ifs.is_open()) {
			throw IoException(filename, "Could not open file");
		}
		std::string line;
		std::vector<std::string> tokens;
		// read magic line
		getline(ifs, line);
		boost::split(tokens, line, boost::is_any_of(" "));
		if(tokens.size() != 1 || tokens[0] != "P2") {
			throw IoException(filename, "Wrong PGM file header (P2 id)");
		}
		// read dimensions line
		getline(ifs, line);
		boost::split(tokens, line, boost::is_any_of(" "));
		unsigned int w, h;
		if(tokens.size() != 2) {
			throw IoException(filename, "Wrong PGM file header (width/height)");
		}
		try {
			w = boost::lexical_cast<unsigned int>(tokens[0]);
			h = boost::lexical_cast<unsigned int>(tokens[1]);
		} catch(...) {
			throw IoException(filename, "Wrong PGM file header (width/height)");
		}
		// read max line
		getline(ifs, line);
		boost::split(tokens, line, boost::is_any_of(" "));
		if(tokens.size() != 1 || tokens[0] != "65535") {
			throw IoException(filename, "Wrong PGM file header (max value)");
		}
		// read data
		Image1ui16 img(w, h);
		unsigned int y = 0;
		while(getline(ifs, line)) {
			boost::split(tokens, line, boost::is_any_of(" "));
			if(tokens.back().empty()) {
				tokens.pop_back();
			}
			if(tokens.size() != w) {
				throw IoException(filename, "Width and number of tokens in line do not match");
			}
			for(unsigned int x=0; x<w; x++) {
				img(x,y) = boost::lexical_cast<unsigned int>(tokens[x]);
			}
			y++;
		}
		if(y != h) {
			throw IoException(filename, "Height and number of lines do not match");
		}
		return img;
	}

	/** Saves a 1 channel 16 bit unsigned integer image to an ASCII PGM file */
	inline void Save(const Image1ui16& img, const std::string& filename) {
		if(!boost::algorithm::ends_with(filename, ".pgm")) {
			throw IoException(filename, "Save for 1ui16 images can only handle PGM files");
		}
		std::ofstream ofs(filename);
		ofs << "P2" << std::endl;
		ofs << img.width() << " " << img.height() << std::endl;
		ofs << "65535" << std::endl;
		for(unsigned int y=0; y<img.height(); y++) {
			for(unsigned int x=0; x<img.width(); x++) {
				ofs << img(x,y);
				if(x+1 < img.width()) {
					ofs << " ";
				}
			}
			if(y+1 < img.height()) {
				ofs << std::endl;
			}
		}
	}

#if defined SLIMAGE_IO_GZ
	inline Image1ui16 Load16BitGZ(const std::string& filename) {
		// custom gzip loading!
		PTR(Danvil::ZIO::Handle) h = Danvil::ZIO::FactorReadHandle(filename);
		size_t width, height, channels, type;
		h->read(width);
		h->read(height);
		h->read(channels);
		h->read(type);
		assert(channels == 1);
		assert(type == 3);
		Image1ui16 img(width, height);
		h->read_raw(static_cast<void*>(img.begin()), width*height*2);
		return img;
	}
#endif

	inline void Save(const Image1ub& img, const std::string& filename) {
		Save(Ptr(img), filename);
	}

	inline void Save(const Image3ub& img, const std::string& filename) {
		Save(Ptr(img), filename);
	}

	inline void Save(const Image4ub& img, const std::string& filename) {
		Save(Ptr(img), filename);
	}

	inline Image1ub Load1ub(const std::string& filename) {
		return Ref<unsigned char, 1>(Load(filename));
	}

	inline Image3ub Load3ub(const std::string& filename) {
		return Ref<unsigned char, 3>(Load(filename));
	}

	inline Image4ub Load4ub(const std::string& filename) {
		return Ref<unsigned char, 4>(Load(filename));
	}

}

//#endif

#endif
