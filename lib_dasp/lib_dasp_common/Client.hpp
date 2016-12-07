#ifndef COMMON_CLIENT_HPP
#define COMMON_CLIENT_HPP

#include <Slimage/Slimage.hpp>
#include <boost/function.hpp>
#include <string>

class Client
{
public:
	virtual ~Client() {}

	virtual void OnRender() = 0;

	virtual void OnTick() = 0;

	typedef boost::function<void(const std::string& caption, const slimage::Image3ub& img)> ShowImageFnc;

	void SetShowImageFunction(const ShowImageFnc& f) {
		on_show_image_ = f;
	}

	void ShowImage(const std::string& caption, const slimage::Image3ub& img) const {
		on_show_image_(caption, img);
	}

private:
	ShowImageFnc on_show_image_;

};

#endif
