// Copyright 2013 Visual Sensorics and Information Processing Lab, Goethe University, Frankfurt
//
// This file is part of Contour-relaxed Superpixels.
//
// Contour-relaxed Superpixels is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Contour-relaxed Superpixels is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Contour-relaxed Superpixels.  If not, see <http://www.gnu.org/licenses/>.


#pragma once

/**
 * @file VisualizationHelper.h
 * @brief Header file providing some functions for visualization purposes.
 */

#include <opencv2/opencv.hpp>
#include <vector>


/**
 * @brief Create a boundary overlay, that is: mark the label boundaries in the given color image by making them red.
 * @param bgrImage the color image to serve as basis for the boundary overlay image, in BGR format
 * @param labelImage the label image, contains one label identifier per pixel
 * @param out_boundaryOverlay the resulting boundary overlay image, will be (re)allocated if necessary
 *
 * This function uses a rather crude way to find boundary pixels: it only compares with the pixel to the right
 * and the one below. This works most of the time, however a few boundary pixels may not be found, especially
 * in the last row and column. Boundary overlays are only useful for visualization, so this is not much of an
 * issue. A perfectly accurate version of this function could be implemented by using the functionality from
 * ::computeBoundaryImage to identify boundary pixels, and just marking those in red.
 */
template <typename TLabelImage>
void computeBoundaryOverlay(cv::Mat const& bgrImage, cv::Mat const& labelImage, cv::Mat& out_boundaryOverlay)
{
    std::vector<cv::Mat> overlayChannels;
    cv::split(bgrImage, overlayChannels);

    for (int row = 0; row < labelImage.rows-1; ++row)
    {
        TLabelImage const* const labelImageUpperRowPtr = labelImage.ptr<TLabelImage>(row);
        TLabelImage const* const labelImageLowerRowPtr = labelImage.ptr<TLabelImage>(row+1);
        unsigned char* const outBoundaryImageUpperRowPtrB = overlayChannels[0].ptr<unsigned char>(row);
        unsigned char* const outBoundaryImageLowerRowPtrB = overlayChannels[0].ptr<unsigned char>(row+1);
        unsigned char* const outBoundaryImageUpperRowPtrG = overlayChannels[1].ptr<unsigned char>(row);
        unsigned char* const outBoundaryImageLowerRowPtrG = overlayChannels[1].ptr<unsigned char>(row+1);
        unsigned char* const outBoundaryImageUpperRowPtrR = overlayChannels[2].ptr<unsigned char>(row);
        unsigned char* const outBoundaryImageLowerRowPtrR = overlayChannels[2].ptr<unsigned char>(row+1);

        for (int col = 0; col < labelImage.cols-1; ++col)
        {

            if (labelImageUpperRowPtr[col] != labelImageUpperRowPtr[col+1]){
                outBoundaryImageUpperRowPtrB[col] = 0;
                outBoundaryImageUpperRowPtrB[col+1] = 0;
                outBoundaryImageUpperRowPtrG[col] = 0;
                outBoundaryImageUpperRowPtrG[col+1] = 0;
                outBoundaryImageUpperRowPtrR[col] = 255;
                outBoundaryImageUpperRowPtrR[col+1] = 255;
            }
            if (labelImageUpperRowPtr[col] != labelImageLowerRowPtr[col]){
                outBoundaryImageUpperRowPtrB[col] = 0;
                outBoundaryImageLowerRowPtrB[col] = 0;
                outBoundaryImageUpperRowPtrG[col] = 0;
                outBoundaryImageLowerRowPtrG[col] = 0;
                outBoundaryImageUpperRowPtrR[col] = 255;
                outBoundaryImageLowerRowPtrR[col] = 255;
            }
        }
    }

    cv::merge(overlayChannels, out_boundaryOverlay);
}


/**
 * @brief Compute a binary boundary image, containing 1 for pixels on a label boundary, 0 otherwise.
 * @param labelImage the label image, contains one label identifier per pixel
 * @param out_boundaryImage the resulting boundary image, binary by nature, but stored as unsigned chars, will be (re)allocated if necessary
 *
 * This function is basically taken from ContourRelaxation::computeBoundaryMap and provides exact boundary maps
 * (with 2 pixel-wide boundaries) which can be used e.g. for benchmarks.
 */
template <typename TLabelImage>
void computeBoundaryImage(cv::Mat const& labelImage, cv::Mat& out_boundaryImage)
{
    assert(labelImage.type() == cv::DataType<TLabelImage>::type);

    // Initialize (or reset) boundary map with zeros.
    cv::Mat tmpBoundaryMap = cv::Mat::zeros(labelImage.size(), cv::DataType<unsigned char>::type);

    // For each pixel, compare with neighbors. If different label, set both to 1 (= boundary pixel).
    // Compare only half of the neighbors, the other half will be compared when they themselves are the current pixel.
    for (int row = 0; row < labelImage.rows; ++row)
    {
        TLabelImage const* const labelImageUpperRowPtr = labelImage.ptr<TLabelImage>(row);
        unsigned char* const boundaryImageUpperRowPtr = tmpBoundaryMap.ptr<unsigned char>(row);

        TLabelImage const* labelImageLowerRowPtr = 0;
        unsigned char* boundaryImageLowerRowPtr = 0;

        // Check whether we have one more row downwards.
        // We can only get the row pointers to that row if it exists, obviously.
        bool canLookDown = false;
        if (row < labelImage.rows - 1)
        {
            labelImageLowerRowPtr = labelImage.ptr<TLabelImage>(row + 1);
            boundaryImageLowerRowPtr = tmpBoundaryMap.ptr<unsigned char>(row + 1);
            canLookDown = true;
        }

        for (int col = 0; col < labelImage.cols; ++col)
        {
            // Check whether we have one more column to the right.
            bool canLookRight = false;
            if (col < labelImage.cols - 1)
            {
                canLookRight = true;
            }

            // Neighbor to the right.
            if (canLookRight)
            {
                if (labelImageUpperRowPtr[col] != labelImageUpperRowPtr[col + 1])
                {
                    boundaryImageUpperRowPtr[col] = 1;
                    boundaryImageUpperRowPtr[col + 1] = 1;
                }
            }

            // Neighbor to the bottom.
            if (canLookDown)
            {
                if (labelImageUpperRowPtr[col] != labelImageLowerRowPtr[col])
                {
                    boundaryImageUpperRowPtr[col] = 1;
                    boundaryImageLowerRowPtr[col] = 1;
                }
            }

            // Neighbor to the bottom right.
            if (canLookDown && canLookRight)
            {
                if (labelImageUpperRowPtr[col] != labelImageLowerRowPtr[col + 1])
                {
                    boundaryImageUpperRowPtr[col] = 1;
                    boundaryImageLowerRowPtr[col + 1] = 1;
                }
            }

            // Neighbor to the bottom left.
            if (canLookDown && col > 0)
            {
                if (labelImageUpperRowPtr[col] != labelImageLowerRowPtr[col - 1])
                {
                    boundaryImageUpperRowPtr[col] = 1;
                    boundaryImageLowerRowPtr[col - 1] = 1;
                }
            }
        }
    }

//     out_boundaryImage = tmpBoundaryMap > 0;
    tmpBoundaryMap.copyTo(out_boundaryImage);
}
