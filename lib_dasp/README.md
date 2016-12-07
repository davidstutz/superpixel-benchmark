Depth-Adaptive Superpixels (DASP)
====

**Consider using my new project [asp](https://github.com/Danvil/asp)! It is much smaller and easier to use.**

DASP is a novel oversegmentation algorithm for RGB-D images. In contrast to previous approaches it uses 3D information in addition to color. DASP can be used as a preprocessing step for various computer vision applications, for example image segmentation or point cloud segmentation.

![Superpixels and segments](https://content.wuala.com/contents/Danvil/Public/dasp/dasp.jpg)

The DASP algorithm partitions the visible surface of the 3D geometry into uniformly distributed and equally sized planar patches. This results in a classic oversegmentation of pixels into depth-adaptive superpixels which correctly reflect deformation through perspective projection.

Using for example spectral graph theory, depth-adaptive superpixels are capable to produce high-quality image and point-cloud segmentations in near realtime (~2 fps). DASP outperform state-of-the-art oversegmentation and image segmentation algorithms both in quality and runtime.

Depth-adaptive superpixel can be used in 2D as well as in 3D. The following rendering visualizes depth-adaptive superpixels in 3D space.

![3D superpoints](https://content.wuala.com/contents/Danvil/Public/dasp/dasp_3d.jpg)


Publications
----
Further technical detail can be found in the following publication:

David Weikersdorfer, David Gossow, Michael Beetz. **Depth-Adaptive Superpixels** ([pdf](https://content.wuala.com/contents/Danvil/Public/dasp/weikersdorfer2012dasp.pdf)). *21-st International Conference on Patter Recognition (ICPR), 2012*.


Installation
----

Dasp was tested under Ubuntu 11.10, Ubuntu 12.04 and Ubuntu 14.04.

Dasp uses C++11 and requires at least GCC 4.6. Due to the poor support of the new C++ standard by Microsoft, it is probably not possible to use dasp with MSVC.

### Requirements

* Build essentials: `sudo apt-get install g++ build-essential cmake cmake-qt-gui`
* [Boost](http://www.boost.org/) 1.46.1 or higher: `sudo apt-get install libboost-all-dev`
* [Eigen](http://eigen.tuxfamily.org) 3.x: `sudo apt-get install libeigen3-dev`
* [Qt](http://qt.nokia.com/) 4.x: `sudo apt-get install libqt4-dev`
* snappy: `sudo apt-get install libsnappy-dev`
* slimage: Clone from https://github.com/Danvil/slimage (header only - no build required)

All apt-get dependencies in one line: *sudo apt-get install libboost-all-dev libeigen3-dev libqt4-dev libsnappy-dev g++ build-essential cmake cmake-qt-gui*

### Installation Instructions

1. `git clone git://github.com/Danvil/dasp.git`
2. `cd dasp; mkdir build; cd build`
3. `cmake -DCMAKE_BUILD_TYPE=Release ..`
4. `make`
5. `dasp_gui/dasp_gui` to run the Qt gui for dasp

### cmake flags

* CMAKE_BUILD_TYPE - Set to Release to compile with optimizations and get a huge speed increase
* EIGEN3_INCLUDE_DIR - Set this to the base include directory of eigen3 (e.g. '/usr/include/eigen3')


Misc
----

### Kinect Live Mode

Required if you want to process data from the Kinect in the live mode.

Download and install [OpenNI](https://github.com/OpenNI/OpenNI) and the Microsoft Kinect driver.

To enable OpenNI you have to enable the CMake flag `DASP_HAS_OPENNI` and set the CMake variable `OPENNI_INCLUDE_DIR` to the OpenNI include directory (normally `/path/to/OpenNI/Include`).

### Faster eigenvalues

There is some basic support for arpack++, Magma and ietl for computing the smallest eigenvalues of a sparse matrix. Use at your own peril. It can be enabled by setting the cmake flags USE_SOLVER_ARPACK, USE_SOLVER_MAGMA and USE_SOLVER_IETL.

For arpack the dependencies are `sudo apt-get install libarpack++2-dev libsuperlu3-dev`.

### 3D rendering

*under construction*

Enable with DASP_HAS_CANDY. This uses a small 3D engine to render superpixel in 3D.

Additional dependcies: `sudo apt-get install libglew1.6-dev libxmu-dev*`


Dataset
----
The RGBD dataset used for this paper consists of 11 RGBD images with annotated ground truth data. All images have the resolution of 640x480 pixels and were recorded with the Microsoft Kinect sensor. Ground truth annotations were created with the [Interactive Image Segmentation Tool](http://www.eecs.berkeley.edu/Research/Projects/CS/vision/grouping/resources.html) with additional manual processing.

![001 images](https://content.wuala.com/contents/Danvil/Public/dasp/001_montage.jpg)

For each scene (ID = 001 - 011) there are the following images in the dataset:
* ID_color.png - an RGB image of the measured color values in the common PNG image format.
* ID_depth.pgm - the measured depth value stored as an uncompressed 16-bit greyscale PGM image. See the [Netpbm wikipedia article](http://en.wikipedia.org/wiki/Netpbm_format#PGM_example) for specifications.
* ID_labels.pgm - segment integer labels stored as a 16-bit greyscale PGM image.
* ID_bnds.png - segment boundaries stored as an PGM image.
* ID.png - same as ID_labels.pgm but stored as a color PNG image.

The full dataset can be downloaded [here](https://content.wuala.com/contents/Danvil/Public/dasp/dasp_rgbd_dataset.7z).


Issues
----

### Performance

Do not forget to set CMAKE_BUILD_TYPE to Release! This will give a *huge* increase in performance.

### Compiler errors with arpack++

`/usr/include/arpack++/arrssym.h:278:7: error: ‘class ARrcSymStdEig<double>’ has no member named ‘EigVecp’`

There seems to be a bug in arpack++. Apply this patch to the file `/usr/include/arpack++/arrssym.h` to fix the issue.

	--- /usr/include/arpack++/arrssym.h
	+++ /home/david/arrssym.h
	@@ -275,8 +275,8 @@
	   }
	   else {                           // Eigenvalues and vectors are not available.
	     try {
	-      if (this->EigVecp == NULL) this->EigVecp = new ARFLOAT[this->ValSize()*this->n];
	-      if (this->EigValp == NULL) this->EigValp = new ARFLOAT[this->ValSize()];
	+      if (EigVecp == NULL) EigVecp = new ARFLOAT[this->ValSize()*this->n];
	+      if (EigValp == NULL) EigValp = new ARFLOAT[this->ValSize()];
	     }
	     catch (ArpackError) { return 0; }
	     if (this->newVec) {
	@@ -287,8 +287,8 @@
	       delete[] this->EigValR;
	       this->newVal = false;
	     }
	-    this->EigVec  = this->EigVecp;
	-    this->EigValR = this->EigValp;
	+    this->EigVec  = EigVecp;
	+    this->EigValR = EigValp;
	     this->nconv   = this->FindEigenvectors(ischur);
	     this->EigVec  = NULL;
	     this->EigValR = NULL;

### Compiler errors with OpenNI

There seems to be a bug that the operating system platform is not correctly identified. Apply this patch to the file `/usr/include/ni/XnPlatform.h` to fix the issue.

	--- /usr/include/ni/XnPlatform.h	2012-12-29 11:56:25.873983019 +0100
	+++ /home/david/XnPlatform.h	2012-12-29 12:35:40.666070961 +0100
	@@ -62,9 +62,9 @@
	 	#include "Win32/XnPlatformWin32.h"
	 #elif defined(ANDROID) && defined(__arm__)
	 	#include "Android-Arm/XnPlatformAndroid-Arm.h"
	-#elif (linux && (i386 || __x86_64__))
	+#elif (__linux__ && (__i386__ || __x86_64__))
	 	#include "Linux-x86/XnPlatformLinux-x86.h"
	-#elif (linux && __arm__)
	+#elif (__linux__ && __arm__)
	 	#include "Linux-Arm/XnPlatformLinux-Arm.h"
	 #elif _ARC
	 	#include "ARC/XnPlatformARC.h"


Links
----
* [David Weikersdorfer @ Technical University Munich, Germany](http://ias.cs.tum.edu/people/weikersdorfer)
* [Depth-adaptive superpixels, ICPR 2012](https://content.wuala.com/contents/Danvil/Public/dasp/weikersdorfer2012dasp.pdf)
* [DASP RGBD dataset with annotated ground truth](https://content.wuala.com/contents/Danvil/Public/dasp/dasp_rgbd_dataset.7z)
* [Interactive Image Segmentation Tool](http://www.eecs.berkeley.edu/Research/Projects/CS/vision/grouping/resources.html)