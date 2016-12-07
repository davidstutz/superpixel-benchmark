/**
 * Copyright (c) 2016, David Stutz
 * Contact: david.stutz@rwth-aachen.de, davidstutz.de
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "SegmentExtraction.h"
#include "Hexagon.h"
#include "FriendFunctions.h"
#include "ccs_opencv.h"

void CCS_OpenCV::computeSuperpixels(const cv::Mat& mat, int region_size, 
        int iterations, int compactness, bool lab, cv::Mat& labels) {
    
    cv::Mat image;
    if (lab) {
        cv::cvtColor(mat, image, CV_BGR2Lab);
    }
    else {
        image = mat.clone();
    }
    
    int* s_index = new int[5];
    s_index[0] = region_size;
    s_index[1] = iterations;
    s_index[2] = compactness;
    s_index[3] = 1; // Seems not to be used ...
    
    int rows = image.rows;
    int cols = image.cols;
    
    // s_index[0] Segment Number; now region size
    // s_index[1] itertion
    // s_index[2] compactness factor
    // s_index[3] method choice 1:ConvexRGB

    SegmentExtraction SE;
    DynamicSegList *SegData;
    SE.total_time = 0;
    
    //cvSmooth(imgIn, imgIn,CV_MEDIAN,3,0 );
    
    IplImage* img;
    IplImage* img2;
    
    CvSize newSize;
    newSize.height = rows;
    newSize.width = cols;
    
    img = cvCreateImage(newSize, IPL_DEPTH_8U, image.channels());
    img2 = cvCreateImage(newSize, IPL_DEPTH_8U, image.channels());

    uchar* Image = new uchar[3*rows*cols];
    uchar* ImageSeg = new uchar[3*rows*cols];
    
    cv::Mat image_clone = image.clone();
    Mat2uchar_Nomem(&image_clone, Image);
    
    vector<uchar*> Seg_Image_Array;
    SE.get_data(img->width, img->height, Seg_Image_Array, 0, s_index);

    SE.KmeansOverSeg(Image, ImageSeg, 0);	
    
    labels.create(rows, cols, CV_32SC1);
    int2Mat(SE.mSegmentIndexK, &labels);
    
    free(Image);
    cvReleaseImage(&img);
    cvReleaseImage(&img2);
}