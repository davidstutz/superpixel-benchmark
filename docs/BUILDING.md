# Building

There are three main components that need to be built:

* [C++ Components](c++)
* [MatLab Components](matlab)
* [Java Components (PF)](java-components)

Building has been tested under Ubuntu 12.04 and Ubuntu 14.04. **Note that
building can be customized allowing to built the desired components and
algorithms only.**

**Quick Start:**

Make sure to clone the repository recursively:

    $ git clone https://github.com/davidstutz/superpixel-benchmark --recursive

Install [Cmake](https://cmake.org/), [Boost](http://www.boost.org/), [OpenCV](http://opencv.org/), [GLog](https://github.com/google/glog) and run:

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

To compile the evaluation code including the superpixel algorithms recommended
in [1]

## C++

Building the C++ components is done using [CMake](https://cmake.org/) and all of them are based
on the `lib_eval` component which holds the evaluation metrics as well as
IO and superpixel tools.

### Prerequisites

A C++ compiler supporting C++11 is assumed to be available.
It was tested with gcc >= 4.8.4.

Being able to build `lib_eval` is prerequisite independent of the algorithms
to be built. Requirements to build `lib_eval` are [CMake](https://cmake.org/), 
[OpenCV](http://www.boost.org/), [Boost](https://github.com/google/glog)
and [GLog](https://github.com/google/glog). Note that the required CMake modules, e.g. for finding GLog, can
be found in `cmake` in case these need to be adapted. Build essentials, CMake, OpenCV
and Boost can be installed as follows:

    $sudo apt-get install build-essential cmake libboost-dev-all libopencv-dev

OpenCV can alternatively be installed following [these instructions](http://docs.opencv.org/2.4/doc/tutorials/introduction/linux_install/linux_install.html#linux-installation).
GLog should be installed manually (at least this was the best option at the
point of writing). GLog should be downloaded or cloned from [google/glog](https://github.com/google/glog). The 
release 0.3.3 was tested and works with the provided `cmake/FindGlog.cmake`. Extract
GLog into the home directory, and:

    $ cd glog-0.3.3/
    $ ./configure
    $ make
    $ sudo make install

For further instructions see the issue tracker at [google/glog](https://github.com/google/glog). Note that GLog 
can alternatively be installed using

    $ sudo apt-get install libgoogle-glog-dev

However, `cmake/FindGlog.cmake` needs to be adapted.

### Building Options

After verifying that the requirements for `lib_eval` are installed:

    $ mkdir build
    $ cd build
    $ cmake ..
    $ cmake -LAH

This will list all available CMake options. You will find the options provided
by this repository at the beginning; these options correspond to the individual
algorithms. These options include:

* `-DBUILD_CCS`: build CCS (Off)
* `-DBUILD_CIS`: build CIS (Off), follow [building CIS](docs/BUILDING_CIS.md) for details
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

Note that the algorithms recommended in [1] are built by default. To change this,
use the option indicated above or edit `CMakeLists.txt` accordingly.

### Individual Requirements

In the following, we outline additional prerequisites needed to build selected
superpixel algorithms. Note that when using the default options, no additional
requirements need to be fulfilled.

#### CIS

The CIS source code is not directly included in this repository due to license constraints.
For installing and building CIS, follow [these instructions](docs/BUILDING_CIS.md).

#### DASP

[Eigen3](http://eigen.tuxfamily.org/index.php?title=Main_Page) is required:

    sudo apt-get install libeigen3-dev

#### ERGC

[CImg](http://cimg.eu/) and [LAPACK](http://www.netlib.org/lapack/) is required:

    sudo apt-get install cimg-dev cimg-doc cimg-examples
    # TODO: installation instructions on clean Ubuntu

#### ETPS

`libpng` and `png++` need to be installed. `libpng` should be installed as prerequisite of OpenCV,
otherwise use

    sudo apt-get install libpng-dev

For `png++` use

    sudo apt-get install libpng++-dev

#### vlSLIC

Note that the source code in `lib_vlslic` is part of the [VLFeat](http://www.vlfeat.org/) library.
The library was stripped to the essentials components needed for vlSLIC. Alternatively,
VLFeat can be installed as described [here](http://www.vlfeat.org/compiling-unix.html). The corresponding `CMakeLists.txt`
in `vlslic_cli` and `lib_vlslic` may need to be adapted (note that `lib_vlslic/vlslic_opencv.h`
is header only such that `lib_vlslic` might not be required to be compiled.

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

Make sure to set the variables `JAVAC`, `JAR` and `JAVA` correctly. On Ubuntu, this
may look as follows:

    JAVAC="/home/david/jdk1.8.0_45/bin/javac"
    JAR="/home/david/jdk1.8.0_45/bin/jar"
    JAVA="/home/david/jdk1.8.0_45/bin/java"

Here, Java SE was installed locally for the use "david".