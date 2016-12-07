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

#include "IFeature.h"
#include "AGaussianFeature.h"
#include "LabelStatisticsGauss.h"
#include "globalConstants.h"

#include <opencv2/opencv.hpp>
#include <vector>


/**
 * @class GrayvalueFeature
 * @brief Feature of one Gaussian-distributed channel, intended for use with grayvalue images.
 */
template <typename TLabelImage>
class GrayvalueFeature : public AGaussianFeature<TLabelImage>
{
    private:

        typedef uchar TGrayvalueData; ///< the type of the used grayvalue images

        std::vector<LabelStatisticsGauss> labelStatistics; ///< Gaussian label statistics of the grayvalue image
        cv::Mat grayvalImage; ///< the observed grayvalue data


    public:

        void setData(cv::Mat const& grayvalueImage);

        void initializeStatistics(cv::Mat const& labelImage);

        double calculateCost(cv::Point2i const& curPixelCoords,
            TLabelImage const& oldLabel, TLabelImage const& pretendLabel, std::vector<TLabelImage> const& neighbourLabels) const;

        void updateStatistics(cv::Point2i const& curPixelCoords, TLabelImage const& oldLabel, TLabelImage const& newLabel);

        void generateRegionMeanImage(cv::Mat const& labelImage, cv::Mat& out_regionMeanImage) const;
};


/**
 * @brief Assign new data to the feature object.
 * @param grayvalueImage observed grayvalue data
 */
template <typename TLabelImage>
void GrayvalueFeature<TLabelImage>::setData(cv::Mat const& grayvalueImage)
{
    assert(grayvalueImage.type() == cv::DataType<TGrayvalueData>::type);

    grayvalueImage.copyTo(grayvalImage);
}


/**
 * @brief Estimate the label statistics of all labels in the given label image, using the observed data saved in the feature object.
 * @param labelImage label identifiers of all pixels
 */
template <typename TLabelImage>
void GrayvalueFeature<TLabelImage>::initializeStatistics(cv::Mat const& labelImage)
{
    // Use the provided initialization method for gaussian statistics from AGaussianFeature.
    this->template initializeGaussianStatistics<TGrayvalueData>(labelImage, grayvalImage, labelStatistics);
}


/**
 * @brief Calculate the total cost of all labels in the 8-neighbourhood of a pixel, assuming the pixel would change its label.
 * @param curPixelCoords coordinates of the regarded pixel
 * @param oldLabel old label of the regarded pixel
 * @param pretendLabel assumed new label of the regarded pixel
 * @param neighbourLabels all labels found in the 8-neighbourhood of the regarded pixel, including the old label of the pixel itself
 * @return total negative log-likelihood (or cost) of all labels in the neighbourhood, assuming the label change
 */
template <typename TLabelImage>
double GrayvalueFeature<TLabelImage>::calculateCost(cv::Point2i const& curPixelCoords,
    TLabelImage const& oldLabel, TLabelImage const& pretendLabel, std::vector<TLabelImage> const& neighbourLabels) const
{
    // Use the provided cost calculation method for gaussian statistics from AGaussianFeature.
    return this->template calculateGaussianCost<TGrayvalueData>(curPixelCoords, oldLabel, pretendLabel,
        neighbourLabels, labelStatistics, grayvalImage);
}


/**
 * @brief Update the saved label statistics to reflect a label change of the given pixel.
 * @param curPixelCoords coordinates of the pixel whose label changes
 * @param oldLabel old label of the changing pixel
 * @param newLabel new label of the changing pixel
 */
template <typename TLabelImage>
void GrayvalueFeature<TLabelImage>::updateStatistics(cv::Point2i const& curPixelCoords, TLabelImage const& oldLabel,
    TLabelImage const& newLabel)
{
    // Use the provided update method for gaussian statistics from AGaussianFeature.
    this->template updateGaussianStatistics<TGrayvalueData>(curPixelCoords, labelStatistics[oldLabel],
        labelStatistics[newLabel], grayvalImage);
}


/**
 * @brief Create a representation of the current image visualizing each pixel with the mean grayvalue of its assigned label.
 * @param labelImage label identifier of each pixel
 * @param out_regionMeanImage will be (re)allocated if necessary and filled with the described visualization
 */
template <typename TLabelImage>
void GrayvalueFeature<TLabelImage>::generateRegionMeanImage(cv::Mat const& labelImage, cv::Mat& out_regionMeanImage) const
{
    assert(labelImage.size() == grayvalImage.size());
    assert(labelImage.type() == cv::DataType<TLabelImage>::type);

    // Generate an image which represents all pixels by the mean grayvalue of their label.
    // cv::Mat::create only reallocates memory if necessary, so this is no slowdown.
    // We don't care about initialization of out_regionMeanImage since we will set all pixels anyway.
    out_regionMeanImage.create(grayvalImage.size(), cv::DataType<TGrayvalueData>::type);

    for (int row = 0; row < out_regionMeanImage.rows; ++row)
    {
        TGrayvalueData* const out_rmiRowPtr = out_regionMeanImage.ptr<TGrayvalueData>(row);
        TLabelImage const* const labelImRowPtr = labelImage.ptr<TLabelImage>(row);

        for (int col = 0; col < out_regionMeanImage.cols; ++col)
        {
            out_rmiRowPtr[col] = labelStatistics[labelImRowPtr[col]].valueSum / labelStatistics[labelImRowPtr[col]].pixelCount;
        }
    }
}
