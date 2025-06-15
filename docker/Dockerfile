ARG BASE_IMAGE=debian:bookworm-20250520-slim
FROM ${BASE_IMAGE} AS builder

ENV DEBIAN_FRONTEND=noninteractive \
    APP_HOME=/app
SHELL ["/bin/bash", "-o", "pipefail", "-c"]

# Install build-time dependencies (caches apt lists between builds)
RUN echo 'Acquire::Retries "5";' > /etc/apt/apt.conf.d/80-retries
RUN apt-get update && \
    apt-get install -yqq --no-install-recommends \
      autotools-dev automake build-essential \
      ca-certificates \
      cimg-dev cimg-doc cimg-examples \
      cmake git \
      libboost-all-dev \
      libeigen3-dev \
      libopencv-dev \
      libpng-dev libpng++-dev \
      libgoogle-glog-dev \
      libpcl-dev libflann-dev \
      libusb-1.0-0-dev libopenni2-0 libpcap0.8 \
      pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Eigen symlink
RUN ln -sf /usr/include/eigen3/Eigen /usr/local/include/Eigen

# Clone + patch (timer-patch.sh is boost::timer updates + a fix to CImg.h for debian:trixie)
WORKDIR $APP_HOME
RUN git clone https://github.com/davidstutz/superpixel-benchmark.git super
COPY docker.patch timer-patch.sh super/

WORKDIR $APP_HOME/super
RUN git reset --hard d454f143ceee43811f4604f02661e24d60e6ed91 && \
    git submodule update --init --recursive && \
    git apply docker.patch && \
    ./timer-patch.sh && \
    sed -i -E \
        's|cmake_minimum_required[[:space:]]*\([[:space:]]*VERSION[[:space:]]*2\.8[[:space:]]*\)|cmake_minimum_required(VERSION 2.8.12...3.25)|' \
        $(find . -name CMakeLists.txt) && \
    find . -name CMakeLists.txt -exec sed -i '/find_package(Boost/i\
        if(POLICY CMP0167)\n  cmake_policy(SET CMP0167 NEW)\nendif()\n' {} + && \
    sed -i \
        's/cimg::cut(116\*fY - 16,0,100)/cimg::cut(116*fY - 16,0.0f,100.0f)/g' \
        /usr/include/CImg.h

# Build
WORKDIR $APP_HOME/super/build
RUN cmake .. && make
RUN strip --strip-unneeded $APP_HOME/super/bin/*

# gather all shared‐lib deps for every binary
RUN mkdir -p /tmp/dep_libs && \
    for bin in $APP_HOME/super/bin/*; do \
        ldd "$bin" \
            | awk '/=>/ && $3 ~ /^\// { print $3 }' \
            | sort -u \
            | xargs -r -I '{}' cp -L '{}' /tmp/dep_libs/ ; \
    done

FROM ${BASE_IMAGE} AS runtime
LABEL org.opencontainers.image.authors="mathematicalmichael (Michael Pilosov) <consistentbayes@gmail.com>" \
      org.opencontainers.image.source="https://github.com/davidstutz/superpixel-benchmark"

ENV DEBIAN_FRONTEND=noninteractive \
    APP_HOME=/app \
    PATH=/app/super/bin:$PATH \
    LD_LIBRARY_PATH=/app/super/lib

# Minimal runtime tools & libs
RUN apt-get update && \
    apt-get install -yqq --no-install-recommends \
    #   ca-certificates \
    #   ffmpeg \
    #   potrace \
    #   graphicsmagick-imagemagick-compat \
      dumb-init \
    && rm -rf /var/lib/apt/lists/*

# Copy superpixel-benchmark binaries and supporting files
# Copying to '/app/super/bin' to support previous 'docker run' syntax
COPY --from=builder $APP_HOME/super/bin $APP_HOME/super/bin
COPY --from=builder /tmp/dep_libs/ $APP_HOME/super/lib

RUN useradd -m super \
    && chown -R super:super $APP_HOME

USER super
WORKDIR $APP_HOME

ENTRYPOINT ["dumb-init", "--"]
CMD ["bash"]
