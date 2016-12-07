---

Contour-relaxed Superpixels

Version 0.1-r2, 05.08.2013

Copyright 2013 Visual Sensorics and Information Processing Lab, Goethe University, Frankfurt

http://www.vsi.cs.uni-frankfurt.de/

---

This file is part of Contour-relaxed Superpixels.

Contour-relaxed Superpixels is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Contour-relaxed Superpixels is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Contour-relaxed Superpixels.  If not, see <http://www.gnu.org/licenses/>.

---

If you use this software for research purposes, please cite the following
in any resulting publication:

Contour-relaxed Superpixels
Christian Conrad, Matthias Mertz, and Rudolf Mester
LNCS: 9th Intl. Conf. on Energy Minimization Methods in Computer Vision and Pattern Recognition (EMMCVPR), 2013

---

Build information:

Contour-relaxed Superpixels can be most comfortably built using CMake.
Additionally, you need to have a C++ compiler (e.g. GCC) as well as the OpenCV and Boost libraries
and their header files installed.

The program has been successfully built and tested with the following versions:

CMake 2.8.7
GCC 4.6.3
OpenCV 2.3.1
Boost 1.46.1

In order to build the program in the packed build directory, after unpacking the whole archive,
execute the following two commands from within the build directory:

cmake ../src
make

Additionally, you can then execute "make doc" to build the Doxygen documentation of the source code,
provided you have Doxygen installed.

---

Using Contour-relaxed Superpixels:

After the program has been successfully built, you can run the executable contourRelaxedSuperpixels
with the following command:

./contourRelaxedSuperpixels inputImage [blockWidth = 20] [blockHeight = 20] [directCliqueCost = 0.3] [compactnessWeight = 0.045 (color) / 0.015 (grayscale)]

Required parameters:
 - inputImage: absolute or relative path to the input image file (1 (gray) or 3 (color) channels, must be readable by OpenCV)

Optional parameters:
 - blockWidth: width of the rectangular blocks that are used to initialize the label image, default value = 20
 - blockHeight: height of the rectangular blocks that are used to initialize the label image, default value = 20
 - directCliqueCost: Markov clique cost for vertical and horizontal cliques, default value = 0.3
 - compactnessWeight: weight of the compactness feature, default value = 0.045 for color images, 0.015 for grayscale images

For a detailed explanation of the optional parameters, and some other parameters you can modify
in the source code, see the EMMCVPR 2013 publication mentioned above.

The program will not generate any output, neither text nor graphics, in case everything goes well.
The results will be written to image files in the current working directory.

With inFile.ext being the input image file specified in the program call, the following output files
will be generated:

 - inFile_labelImage.png: 16bit PNG image containing the label identifiers for each pixel.
   Can be opened with other software to retrieve the labels for further processing.

 - inFile_regionMean.png: Region-mean image, replacing each pixel with the mean color / gray value of its label.

 - inFile_boundaryImage.png: Black image overlayed with the label boundaries in white.

 - inFile_boundaryOverlay.png: Input image overlayed with the label boundaries in red.

---

If you want to make changes to the source code, consider switching to a debug build to activate
assertions. This can be very useful should you run into problems. You can do so by replacing
the line

set (CMAKE_BUILD_TYPE Release)

with

set (CMAKE_BUILD_TYPE Debug)

in src/CMakeLists.txt.

---

