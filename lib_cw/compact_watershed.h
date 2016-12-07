/*
 * Compact Watershed
 * Copyright (C) 2014  Peer Neubert, peer.neubert@etit.tu-chemnitz.de
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */ 

#ifndef COMPACT_WATERSHED_H
#define COMPACT_WATERSHED_H

#include <opencv2/opencv.hpp>
#include <stdio.h>


/**
Compact watershed is a superpixel or oversegmentation algorithm.
For details have a look at:

"Compact Watershed and Preemptive SLIC:\\On improving trade-offs of superpixel segmentation algorithms"
Peer Neubert and Peter Protzel, ICPR 2014

@param img input image CV_8UC3 or CV_32FC1
@param B output boundary map
@param n input parameter, approximatly the number of the output superpixels
@param compValStep input parameter for the desired compactness
@param seeds matrix of initial seeds, CV_32FC1, each col: [i; j], if empty, use grid like initialization

*/
void compact_watershed(cv::Mat& img, cv::Mat& B, float n, float compValStep, cv::Mat& seeds);

/**
Compact watershed is a superpixel or oversegmentation algorithm.
For details have a look at:

"Compact Watershed and Preemptive SLIC:\\On improving trade-offs of superpixel segmentation algorithms"
Peer Neubert and Peter Protzel, ICPR 2014

@param img input image CV_8UC3 or CV_32FC1
@param B output boundary map
@param ny appr number of superpixels in y direction
@param nx appr number of superpixels in y direction
@param compValStep input parameter for the desired compactness
@param seeds matrix of initial seeds, CV_32FC1, each col: [i; j], if empty, use grid like initialization

*/
void compact_watershed(cv::Mat& img, cv::Mat& B, float ny, float nx, float compValStep, cv::Mat& seeds);

#endif