# Building

For getting started quickly:

* [#Quick Start](quick-start)

For details, there are three main components that need to be built:

* [C++ Components](#c++)
    * [Prerequisites](#prerequisites)
    * [Building Options](#building-options)
    * [Prerequisites for Individual Algorithms](#prerequisites-for-individual-algorithms)
        * [CIS](#cis)
        * [DASP](#dasp)
        * [ERGC](#ergc)
        * [vlSLIC](#vlslic)
* [MatLab Components](#matlab)
* [Java Components (PF)](#java)

## Getting Started

The following quick start was tested on Ubuntu 12.04 and Ubuntu 14.04.
Make sure to clone the repository recursively, then execute:

    $ git clone https://github.com/davidstutz/superpixel-benchmark --recursive
    $ sudo apt-get install build-essential cmake libboost-dev-all libopencv-dev cimg-dev cimg-doc cimg-examples libpng-dev libpng++-dev
    # Alternatively download GLog 0.3.3 to avoid the hard reset!
    $ git clone https://github.com/davidstutz/glog
    $ cd glog
    $ git reset --hard 0b0b022
    $ ./configure
    $ make
    $ cd ..
    $ cd superpixel-benchmark
    $ mkdir build
    $ cd build
    # Better install GLog properly and adapt cmake/FindGlog.cmake!
    $ cmake .. -DGLOG_ROOT_DIR=~/glog
    $ make

Installs [Cmake](https://cmake.org/), [Boost](http://www.boost.org/), [OpenCV](http://opencv.org/), 
[GLog](https://github.com/google/glog), `libpng`, `libpng++` and [CImg](http://cimg.eu/).
This builts the seven recommended algorithms as discussed in the paper as well 
as the benchmark and tools.

Also see `.travis.yml` for details on building on a fresh Ubuntu 14.04 installation!

## C++

Building the C++ components is done using [CMake](https://cmake.org/) and all of them are based
on the `lib_eval` component which holds the benchmark as well as toopls.

### Prerequisites

A C++ compiler supporting C++11 is assumed to be available.
It was tested with gcc >= 4.8.4. Building has been tested with Ubuntu 12.04 and Ubuntu 14.04.

All algorithms depend on the tools in `lib_eval`. Requirements are:

* [CMake](https://cmake.org/), 
* [OpenCV](http://www.boost.org/)
* [Boost](https://github.com/google/glog)
* [GLog](https://github.com/google/glog).

Additionally, the algorithms built by default depend on:

* [PNG](http://www.libpng.org/pub/png/libpng.html)
* [PNG++](http://www.nongnu.org/pngpp/)

Note that the required CMake modules, e.g. for finding GLog, can
be found in `cmake` in case these need to be adapted. 

CMake, OpenCV and Boost can be installed as follows:

    $ sudo apt-get install build-essential cmake libboost-dev-all libopencv-dev

OpenCV can alternatively be installed following [these instructions](http://docs.opencv.org/2.4/doc/tutorials/introduction/linux_install/linux_install.html#linux-installation).
Currently, OpenCV 2.4.11 is supported, in general 2.4.x should work fine. For upgrading
to OpenCV 3.0 and OpenCV 3.1, it might be necessary to update constants (e.g. as used for
color conversion). Some implementations are known to work with OpenCV 3 and OpenCV 3.1 (e.g. reSEEDS).

GLog 0.3.3 should be installed manually. GLog should be downloaded or cloned from [google/glog](https://github.com/google/glog).
If the repository is cloned, make sure to checkout version 0.3.3. Then:

    $ cd glog-0.3.3/
    $ ./configure
    $ make
    $ sudo make install

For further instructions see the issue tracker at [google/glog](https://github.com/google/glog).

Note that GLog can alternatively be installed using

    $ sudo apt-get install libgoogle-glog-dev

However, `cmake/FindGlog.cmake` needs to be adapted and some parts might not working
with newer versions.

For installing PNG and PNG++:

    sudo apt-get install libpng-dev # should already be installed for OpenCV
    sudo apt-get install libpng++-dev

As reference, these are the library versions as installed on Ubuntu 14.04 (checked using `dpkg -l`)
where OpenCV and GLog where installed manually:

    ||/ Name                Version        Architecture  
    +++-===================-==============-==============
    ii  gcc                 4:4.8.2-1ubunt amd64
    ii  cmake               3.2.2-2ubuntu2 amd64
    ii  libboost-dev        1.54.0.1ubuntu amd64
    ii  libpng12-dev        1.2.50-1ubuntu amd64
    ii  libpng++-dev        0.2.5-1        all

### Building Options

After verifying that the requirements are met:

    $ mkdir build
    $ cd build
    $ cmake ..
    $ cmake -LAH

This will list all available CMake options. These options include:

* `-DBUILD_CCS`: build CCS (Off)
* `-DBUILD_CIS`: build CIS (Off), follow [building CIS](BUILDING_CIS.md) for details
* `-DBUILD_CRS`: build CRS (On)
* `-DBUILD_CW`: build CW (Off)
* `-DBUILD_DASP`: build DASP (Off)
* `-DBUILD_ERGC`: build ERGC (On)
* `-DBUILD_ERS`: build ERS (On)
* `-DBUILD_ETPS`: build ETPS (On)
* `-DBUILD_FH`: build FH (Off)
* `-DBUILD_EXAMPLES`: build examples (Off)
* `-DBUILD_LSC`: build LSC (Off)
* `-DBUILD_MSS`: build MSS (Off)
* `-DBUILD_PB`: build PB (On)
* `-DBUILD_PRESLIC`: build SLIC (Off)
* `-DBUILD_REFH`: build reFH (Off)
* `-DBUILD_RESEEDS`: build reSEEDS (On)
* `-DBUILD_SEEDS`: build SEEDS (On)
* `-DBUILD_SLIC`: build SLIC (On)
* `-DBUILD_VC`: build VC (Off)
* `-DBUILD_VCCS`: build VCCS (Off)
* `-DBUILD_VLSLIC`: build vlSLIC (Off)
* `-DBUILD_W`: build W (Off)

Note that the algorithms recommended in the paper are built by default. To change this,
use the options indicated above or edit `CMakeLists.txt` accordingly.

### Prerequisites for Individual Algorithms

In the following, we outline additional prerequisites needed to build selected
superpixel algorithms. Note that when using the default options, no additional
requirements need to be fulfilled.

#### CIS

The CIS source code is not directly included in this repository due to license constraints.
For installing and building CIS, follow [these instructions](BUILDING_CIS.md).

#### DASP

[Eigen3](http://eigen.tuxfamily.org/index.php?title=Main_Page) is required:

    sudo apt-get install libeigen3-dev

For reference, the following version was installed on Ubuntu 14.04:

    ||/ Name                Version        Architecture
    +++-===================-==============-==============
    ii  libeigen3-dev       3.2.0-8        all

#### ERGC

[CImg](http://cimg.eu/) and [LAPACK](http://www.netlib.org/lapack/) is required:

    sudo apt-get install cimg-dev cimg-doc cimg-examples

For reference, the following version was installed on Ubuntu 14.04:

    ||/ Name                Version        Architecture
    +++-===================-==============-==============
    ii  cimg-dev            1.5.7+dfsg-1   all

#### vlSLIC

Note that the source code in `lib_vlslic` is part of the [VLFeat](http://www.vlfeat.org/) library.
The library was stripped to the essentials components needed for vlSLIC. Alternatively,
VLFeat can be installed as described [here](http://www.vlfeat.org/compiling-unix.html). 
The corresponding `CMakeLists.txt` in `vlslic_cli` and `lib_vlslic` may need to 
be adapted (note that `lib_vlslic/vlslic_opencv.h` is header only such that `lib_vlslic`
might not be required to be compiled.

## MatLab

Some MatLab sources are based on C++ components. In order to compile and MEX
these components, `make.m` can be used. It provides the following variables:

    BUILD_TOOLS = 1;
    BUILD_EAMS = 0;
    BUILD_NC = 0;
    BUILD_QS = 0;
    BUILD_SEAW = 0;
    BUILD_TP = 0;
    BUILT_TPS = 0;

These can be used to compile individual algorithms. Alternatively, the corresponding
`make.m` files can be run directly, i.e. `lib_tools/make.m`, `lib_eams/make.m`,
`lib_nc/make.m`, `lib_qs/make.m`, `lib_seaw/make.m`, `lib_tp/make.m` and `lib_tps/make.m`.

Compiling was tested with MatLab R2015b and gcc >=4.8.4. Note however, that later
MatLab version have different requirements regarding the compiler. For example, in 
MatLab R2015b, gcc 4.8.4 is not officially supported (still it works in this case).

## Java

Only PF is written in Java. For compiling the Java source, follow `lib_pf/make.sh`:

    $JAVAC *.java
    $JAR cfm PathFinder.jar Manifest.txt *.java *.class
    $JAVA -jar PathFinder.jar

Make sure to set the variables `JAVAC`, `JAR` and `JAVA` correctly. On Ubuntu 14.04,
this may look as follows:

    JAVAC="/home/david/jdk1.8.0_45/bin/javac"
    JAR="/home/david/jdk1.8.0_45/bin/jar"
    JAVA="/home/david/jdk1.8.0_45/bin/java"