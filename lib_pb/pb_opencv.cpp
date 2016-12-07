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

#include "graph.h"
#include "QPBO_MaxFlow.h"
#include "Elimination.h"
#include "pb_opencv.h"

struct scost {
    float h00;
    float h01;
    float h10;
    float h11;
    float v00;
    float v01;
    float v10;
    float v11;
};

int myfloor(float t) {
    return (int) t > t ? (int) t - 1 : (int) t;
}

void smoothcost(const cv::Mat &im, int i1, int i2, int j1, int j2, int hsize, int vsize, float sigma, scost *sc) {
    float temp;
    float colordiff = 0;

    temp = im.at<cv::Vec3b>(j1,i1)[0];
    temp -= im.at<cv::Vec3b>(j2,i2)[0];
    colordiff += fabs(temp);

    temp = im.at<cv::Vec3b>(j1,i1)[1];
    temp -= im.at<cv::Vec3b>(j2,i2)[1];
    colordiff += fabs(temp);

    temp = im.at<cv::Vec3b>(j1,i1)[2];
    temp -= im.at<cv::Vec3b>(j2,i2)[2];
    colordiff += fabs(temp);

    colordiff = exp(-colordiff/sigma);

    sc->h01 = colordiff;
    sc->h10 = colordiff;
    sc->v01 = colordiff;
    sc->v10 = colordiff;

//    float tau = 0.001;
    
    if(myfloor((float) i1/hsize)!=myfloor((float) i2/hsize)) {
        sc->h00 = colordiff;
    }
    else {
        sc->h00 = 0;
        
//        if (colordiff < tau) {
//            sc->h00 = 1 - colordiff;
//        }
    }

    if(myfloor((float) (i1 - hsize/2)/hsize) != myfloor((float) (i2 - hsize/2)/hsize)) {
        sc->h11 = colordiff;
    }
    else {
        sc->h11 = 0;
        
//        if (colordiff < tau) {
//            sc->h11 = 1 - colordiff; 
//        }
    }


    if(myfloor((float) j1/vsize)!=myfloor((float) j2/vsize)) {
        sc->v00 = colordiff;
    }
    else {
        sc->v00 = 0;
        
//        if (colordiff < tau) {
//            sc->v00 = 1 - colordiff;
//        }
    }

    if(myfloor((float) (j1 - vsize/2)/vsize) != myfloor((float) (j2 - vsize/2)/vsize)) {
        sc->v11 = colordiff;
    }
    else {
        sc->v11 = 0;
        
//        if (colordiff < tau) {
//            sc->v11 = 1 - colordiff;
//        }
    }
}

void PB_OpenCV::computeSuperpixels(const cv::Mat& image, int region_size, 
        float sigma, bool max_flow, cv::Mat& labels) {
    
    int width = image.cols;
    int height = image.rows;
    int strip_size = region_size;
    
    if (!max_flow) {
        strip_size *= 2;
    }
    
    Matrix<float> U1(height, width), U2(height, width); //to hold data cost
    Matrix<float> Bh1(height, width), Bh2(height, width); //horizontal smooth cost
    Matrix<float> Bv1(height, width), Bv2(height, width); //vertical smooth cost
    Matrix<unsigned char> solution1(height, width), solution2(height, width);

    struct scost *sc = (struct scost*)malloc(sizeof(struct scost));

    U1.fill(0);
    Bh1.fill(0);
    Bv1.fill(0);
    U2.fill(0);
    Bh2.fill(0);
    Bv2.fill(0);

    for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
            if(i<width-1) {
                smoothcost(image, i, i + 1, j, j, strip_size, strip_size, sigma, sc);
                Bh1[j][i]   = sc->h00-sc->h01-sc->h10+sc->h11;
                U1[j][i]   += sc->h10-sc->h00;
                U1[j][i+1] += sc->h01-sc->h00;
                Bh2[j][i]   = sc->v00-sc->v01-sc->v10+sc->v11;
                U2[j][i]   += sc->v10-sc->v00;
                U2[j][i+1] += sc->v01-sc->v00;
            }

            if(j<height-1) {
                smoothcost(image, i, i, j, j + 1, strip_size, strip_size, sigma, sc);

                Bv1[j][i]   = sc->h00-sc->h01-sc->h10+sc->h11;
                U1[j][i]   += sc->h10-sc->h00;
                U1[j+1][i] += sc->h01-sc->h00;
                Bv2[j][i]   = sc->v00-sc->v01-sc->v10+sc->v11;
                U2[j][i]   += sc->v10-sc->v00;
                U2[j+1][i] += sc->v01-sc->v00;
            }
        }
    }

    if(max_flow) {
        MaxFlowQPBO solve1(U1,Bh1,Bv1,solution1);
        MaxFlowQPBO solve2(U2,Bh2,Bv2,solution2);
    }
    else {
        Elimination< float >::solve(U1, Bh1, Bv1, solution1);
        Elimination< float >::solve(U2, Bh2, Bv2, solution2);
    }

    cv::Mat imh(height, width, CV_16UC1);
    cv::Mat imv(height, width, CV_16UC1);

    for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
            if(solution1[j][i] == 0) {
                imh.at<unsigned short>(j,i) = myfloor((float) i/strip_size)*2;
            }
            else {
                imh.at<unsigned short>(j,i) = myfloor((float) (i + strip_size/2)/strip_size)*2 + 1;
            }

            if(solution2[j][i] == 0) {
                imv.at<unsigned short>(j,i) = myfloor((float) j/strip_size)*2;
            }
            else {
                imv.at<unsigned short>(j,i) = myfloor((float) (j + strip_size/2)/strip_size)*2 + 1;
            }
        }
    }

    labels.create(image.rows, image.cols, CV_32SC1);
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            labels.at<int>(i, j) = imh.at<unsigned short>(i, j)*width + imv.at<unsigned short>(i, j);
        }
    }
}