ARG BASE_IMAGE=debian:bullseye-20250520-slim
FROM ${BASE_IMAGE}

RUN export DEBIAN_FRONTEND="noninteractive"
ENV APP_HOME=/app

RUN apt-get update && \
	apt-get install -qq -y \
		autotools-dev \
		automake \
		build-essential \
		cimg-dev \
		cimg-doc \
		cimg-examples \
		cmake \
		ffmpeg \
		git \
		graphicsmagick-imagemagick-compat \
		potrace \
		libboost-all-dev \
		libeigen3-dev \
		libopencv-dev \
		libpng-dev \
		libpng++-dev \
		libgoogle-glog-dev \
		libpcl-dev \
		libusb-1.0-0 \
		libopenni2-0 \
		libpcap0.8 \
	&& apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN mkdir -p $APP_HOME/

RUN cd /usr/local/include && \
	ln -sf eigen3/Eigen Eigen
	#ln -sf eigen3/unsupported unsupported
# the commented out line above helps depending on the base image. python ones dont need it.

# Clone repo and get it ready for compilation
RUN git clone https://github.com/davidstutz/superpixel-benchmark/ $APP_HOME/super
# patch contains updates for library compatibility. 
# NOTE: not all algorithms are built, but many (most) are. 
COPY docker.patch $APP_HOME/super/
RUN cd $APP_HOME/super && \
	git reset --hard d454f143ceee43811f4604f02661e24d60e6ed91 && \
    git submodule update --init --recursive && \
    git apply docker.patch
    # find . -type f \( -name '*.hpp' -o -name '*.h' -o -name '*.cpp' \) \
    #   -exec sed -i 's|#include <boost/timer.hpp>|#include <boost/timer/timer.hpp>|g' {} + && \
    # find . -type f -name '*.cpp' \
    #   -exec sed -i 's/\bboost::timer\s\+\([a-zA-Z_][a-zA-Z0-9_]*\)/boost::timer::cpu_timer \1/g' {} +

RUN cd $APP_HOME/super && \
	mkdir build && \
	cd build && \
	#cmake .. -DGLOG_ROOT_DIR=/app/glog && \
	cmake .. && \
	make

WORKDIR $APP_HOME
# dumb-init for proper handling of control-signals when interacting with the container
RUN apt-get update && \
	apt-get install -qq -y \
	dumb-init \
	&& apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# adding a non-root user, helps with permissions on mounted volumes
RUN useradd -m super
RUN chown -R super:super $APP_HOME
USER super
ENTRYPOINT ["dumb-init", "--"]
CMD ["bash"]
