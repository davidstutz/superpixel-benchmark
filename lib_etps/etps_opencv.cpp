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

#include "stdafx.h"
#include "segengine.h"
#include "functions.h"
#include "utils.h"
#include "SGMStereo.h"
#include "etps_opencv.h"

void ETPS_OpenCV::computeSuperpixels(const cv::Mat &image, int region_size, 
        double regularization_weight, double length_weight, double size_weight, int iterations, cv::Mat &labels) {
    
    SPSegmentationParameters params;
    params.superpixelNum = 0; // Do not set both superpixelNum and gridSize to greater zero!
    params.gridSize = region_size;
    params.appWeight = 1.0;
    params.regWeight = regularization_weight;
    params.lenWeight = length_weight;
    params.sizeWeight = size_weight;        
    params.dispWeight = 2000.0;     // \lambda_{disp} - stereo
    params.smoWeight = 0.2;         // \lambda_{smo} - stereo
    params.smoWeightCo = 0.2;       // \lambda_{smo} - stereo
    params.smoWeightHi = 0.2;       // \lambda_{smo} - stereo
    params.priorWeight = 0.2;       // \lambda_{prior} - stereo
    params.occPriorWeight = 15.0;   // \lambda_{occ} - stereo
    params.hiPriorWeight = 5.0;     // \lambda_{hinge} - stereo
    params.noDisp = 9.0;            // \lambda_{d} - stereo
    params.inlierThreshold = 3.0;
    params.peblThreshold = 0;
    params.updateThreshold = 0.01;
    params.iterations = iterations;
    params.maxUpdates = 400000;
    params.minPixelSize = 1;
    params.maxPixelSize = 16;
    params.reSteps = 10;
    params.instantBoundary = false;
    params.stereo = false;
    params.computeSGM = false;
    params.batchProcessing = true;
    params.inpaint = false;
    params.debugOutput = false;
    params.timingOutput = false;
    
    time_t  timev;
    params.randomSeed = time(&timev);
    
    SPSegmentationEngine engine(params, image);
    engine.ProcessImage();

    // Returns unsigned short!
    labels = engine.GetSegmentation();
    labels.convertTo(labels, CV_32S);
}
