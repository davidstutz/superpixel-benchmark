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

#ifndef MSS_H
#define	MSS_H

#include "imagereconstruct.hpp"

int MSP(const cv::Mat& InpImg3C, cv::Mat& ImgMarker, int SizeStructElem = 7, 
        double Noise = 3.0f, double TolerRange = 7.0f, 
        int SPsizeX = 30, int SPsizeY = 30, int maxNoOfIter = 3) {
    
    cv::Mat InpImg3Ccopy =InpImg3C.clone();

    cv::Mat StructElem = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(SizeStructElem,SizeStructElem));
    cv::Mat StructElem3 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3,3));

    /////////////////////////////////////////////////////////////////////////////////
    //Image preprocessing using Morphological grayscale reconstruct
    /////////////////////////////////////////////////////////////////////////////////

    std::vector<cv::Mat> channels;
    cv::split(InpImg3Ccopy,channels);

    cv::Mat sourceReconstr = cv::Mat(InpImg3C.size(), CV_8U) ;
    cv::Mat FilterInpIm;
    //cvtColor(InpImg3Ccopy,FilterInpIm,cv::COLOR_BGR2Lab);
    cv::split(InpImg3Ccopy,channels);
    for(int ch = 0; ch < 3; ch++) {
        cv::bitwise_not(channels[ch],channels[ch]);
        cv::erode(channels[ch], sourceReconstr,StructElem);

        ImageReconstruct < unsigned char > ( sourceReconstr, channels[ch]);
        sourceReconstr.copyTo(channels[ch]);

        cv::bitwise_not(channels[ch],channels[ch]);
        cv::erode(channels[ch], sourceReconstr,StructElem);

        ImageReconstruct < unsigned char > ( sourceReconstr, channels[ch]);
        sourceReconstr.copyTo(channels[ch]);
    }
    cv::merge(channels, InpImg3Ccopy);
    //cvtColor(InpImg3Ccopy,FilterInpIm,cv::COLOR_Lab2BGR);


    /////////////////////////////////////////////////////////////////////////////////
    // Generate seeds - markers from edge image
    /////////////////////////////////////////////////////////////////////////////////
    cv::Mat Mask(cv::Size(InpImg3Ccopy.cols+2, InpImg3Ccopy.rows+2), CV_8U);
    Mask.setTo(0);
    cv::Mat MaskNot(cv::Size(InpImg3Ccopy.cols, InpImg3Ccopy.rows), CV_8U);
    MaskNot.setTo(1);
    int c;
    int r;
    for( c= 0; c<MaskNot.cols; c+= SPsizeX) {
        cv::line(Mask,cv::Point(c+1,0),cv::Point(c+1,Mask.rows), cv::Scalar(1),1);
        cv::line(MaskNot,cv::Point(c,0),cv::Point(c,MaskNot.rows), cv::Scalar(0),1);
    }
    for( r= 0; r<MaskNot.rows; r+= SPsizeY) {
        cv::line(Mask,cv::Point(0,r+1),cv::Point(Mask.cols,r+1), cv::Scalar(1),1);
        cv::line(MaskNot,cv::Point(0,r),cv::Point(MaskNot.cols,r), cv::Scalar(0),1);
    }
    //imshow("RasterMask",Mask*255);
    /////////////////////////////////////////////////////////////////////////////////
    cv::Mat edge;
    cv::Mat edgeAll;
    cv::Mat DifBlurIm;


    std::vector<cv::Mat> chnls;
    cv::Mat ImYCrCb;
    //cvtColor(FilterInpIm,ImYCrCb,cv::COLOR_BGR2YCrCb);
    cv::split(InpImg3Ccopy, chnls);

    cv::Mat OneChnls;
    for(int ch = 0; ch < 3; ch++) {
//        char name[1000];
        cv::Mat blurIm;
        cv::Mat BlurImWide;

        cv::erode(chnls[ch],BlurImWide,StructElem3);
        cv::dilate(chnls[ch],blurIm,StructElem3);
        DifBlurIm =  blurIm-BlurImWide;
        if(ch == 0) { 
            DifBlurIm.copyTo(edgeAll);
        }
        else {
            edgeAll= cv::max(DifBlurIm,edgeAll);
        }
    }

    edgeAll = edgeAll - Noise;

    cv::Mat edgeAllMask = edgeAll.clone();
    edgeAllMask.setTo(0,MaskNot);

    cv::Mat edgeAll16, edgeAllMask16;
    edgeAll.convertTo(edgeAll16, CV_16UC1);
    edgeAllMask.convertTo(edgeAllMask16, CV_16UC1);
    ImageReconstruct<unsigned short>(edgeAllMask16, edgeAll16);
    edgeAllMask16.convertTo(edge,CV_8U);


    cv::Mat Seeds = edge.clone();\
//    Seeds.convertTo(Seeds, CV_32S);
    
    int NoOfSeeds = 0; 
//    int MarkeerIdx = TolerRange+1;
    
//    int Idx = 0;
    // more iterations
    for (int NoOfIter = 1; NoOfIter <= maxNoOfIter; NoOfIter++) {
        //calculate for each rectangle
        for( c= 0; c< Seeds.cols-SPsizeX; c+= SPsizeX) {
            for(r= 0; r<Seeds.rows-SPsizeY; r+= SPsizeY) {
                double min,max;
                cv::Point minLoc, maxLoc;
                cv::minMaxLoc(Seeds(cv::Rect(c+1,r+1,SPsizeX-2,SPsizeY-2)),&min, &max, &minLoc);
                //first cycle for each min value, next cycle only if min < TolerRange
                if ((min < TolerRange) || (NoOfIter ==1)) {
                    int modulo = 255-TolerRange;
                    int Idx= rand()%modulo + TolerRange;
                    
                    cv::Rect testrect;
                    cv::floodFill(Seeds,Mask, cv::Point(minLoc.x +c+1,minLoc.y+r+1),cv::Scalar(Idx),&testrect,cv::Scalar(3), cv::Scalar(3),  8+cv::FLOODFILL_FIXED_RANGE );
                    Seeds.at < uchar > (cv::Point(minLoc.x +c+1,minLoc.y+r+1)) = Idx;
                    
                    NoOfSeeds ++;
//                    Idx++;
                }
            }
        }
    }
    
    /////////////////////////////////////////////////////////////////////////////////
    // Watershed
    /////////////////////////////////////////////////////////////////////////////////
    cv::bitwise_not(Mask*255,Mask);
    cv::bitwise_not(MaskNot*255,MaskNot);

    Seeds.setTo(0, Mask(cv::Rect(1,1,Seeds.cols, Seeds.rows)));
    Seeds.setTo(0, MaskNot);

    Seeds.convertTo(ImgMarker, CV_32S);
//    Seeds.copyTo(ImgMarker);
    cv::watershed(InpImg3Ccopy,ImgMarker);

    /////////////////////////////////////////////////////////////////////////////////

    return SPsizeX;
}

#endif	/* MSS_H */

