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
 * @class ColorFeature
 * @brief Feature of three independently Gaussian-distributed channels, intended for use with color images in YUV space.
 */
template <typename TLabelImage>
class ColorFeature : public AGaussianFeature<TLabelImage>
{
    private:

        typedef uchar TColorData; ///< the type of the used image channels

        std::vector<LabelStatisticsGauss> labelStatisticsChan1; ///< Gaussian label statistics of the first channel
        std::vector<LabelStatisticsGauss> labelStatisticsChan2; ///< Gaussian label statistics of the second channel
        std::vector<LabelStatisticsGauss> labelStatisticsChan3; ///< Gaussian label statistics of the third channel
        cv::Mat channel1; ///< observed data of the first channel
        cv::Mat channel2; ///< observed data of the second channel
        cv::Mat channel3; ///< observed data of the third channel


    public:

        void setData(cv::Mat const& channel1, cv::Mat const& channel2, cv::Mat const& channel3);

        void initializeStatistics(cv::Mat const& labelImage);

        double calculateCost(cv::Point2i const& curPixelCoords,
            TLabelImage const& oldLabel, TLabelImage const& pretendLabel, std::vector<TLabelImage> const& neighbourLabels) const;

        void updateStatistics(cv::Point2i const& curPixelCoords, TLabelImage const& oldLabel, TLabelImage const& newLabel);

        void generateRegionMeanImage(cv::Mat const& labelImage, cv::Mat& out_regionMeanImage) const;
};


/**
 * @brief Assign new data to the feature object.
 * @param colorChannel1 observed data of channel 1
 * @param colorChannel2 observed data of channel 2
 * @param colorChannel3 observed data of channel 3
 */
template <typename TLabelImage>
void ColorFeature<TLabelImage>::setData(cv::Mat const& colorChannel1, cv::Mat const& colorChannel2, cv::Mat const& colorChannel3)
{
    assert(colorChannel2.size() == colorChannel1.size());
    assert(colorChannel3.size() == colorChannel1.size());
    assert(colorChannel1.type() == cv::DataType<TColorData>::type);
    assert(colorChannel2.type() == cv::DataType<TColorData>::type);
    assert(colorChannel3.type() == cv::DataType<TColorData>::type);

    colorChannel1.copyTo(channel1);
    colorChannel2.copyTo(channel2);
    colorChannel3.copyTo(channel3);
}


/**
 * @brief Estimate the label statistics of all labels in the given label image, using the observed data saved in the feature object.
 * @param labelImage label identifiers of all pixels
 */
template <typename TLabelImage>
void ColorFeature<TLabelImage>::initializeStatistics(cv::Mat const& labelImage)
{
    // Use the provided initialization method for gaussian statistics from AGaussianFeature.
    this->template initializeGaussianStatistics<TColorData>(labelImage, channel1, labelStatisticsChan1);
    this->template initializeGaussianStatistics<TColorData>(labelImage, channel2, labelStatisticsChan2);
    this->template initializeGaussianStatistics<TColorData>(labelImage, channel3, labelStatisticsChan3);
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
double ColorFeature<TLabelImage>::calculateCost(cv::Point2i const& curPixelCoords,
    TLabelImage const& oldLabel, TLabelImage const& pretendLabel, std::vector<TLabelImage> const& neighbourLabels) const
{
    // Use the provided cost calculation method for gaussian statistics from AGaussianFeature.
    double cost = this->template calculateGaussianCost<TColorData>(curPixelCoords, oldLabel, pretendLabel,
            neighbourLabels, labelStatisticsChan1, channel1)
        + this->template calculateGaussianCost<TColorData>(curPixelCoords, oldLabel, pretendLabel,
            neighbourLabels, labelStatisticsChan2, channel2)
        + this->template calculateGaussianCost<TColorData>(curPixelCoords, oldLabel, pretendLabel,
            neighbourLabels, labelStatisticsChan3, channel3);

    return cost;
}


/**
 * @brief Update the saved label statistics to reflect a label change of the given pixel.
 * @param curPixelCoords coordinates of the pixel whose label changes
 * @param oldLabel old label of the changing pixel
 * @param newLabel new label of the changing pixel
 */
template <typename TLabelImage>
void ColorFeature<TLabelImage>::updateStatistics(cv::Point2i const& curPixelCoords, TLabelImage const& oldLabel,
    TLabelImage const& newLabel)
{
    // Use the provided update method for gaussian statistics from AGaussianFeature.
    this->template updateGaussianStatistics<TColorData>(curPixelCoords, labelStatisticsChan1[oldLabel],
        labelStatisticsChan1[newLabel], channel1);
    this->template updateGaussianStatistics<TColorData>(curPixelCoords, labelStatisticsChan2[oldLabel],
        labelStatisticsChan2[newLabel], channel2);
    this->template updateGaussianStatistics<TColorData>(curPixelCoords, labelStatisticsChan3[oldLabel],
        labelStatisticsChan3[newLabel], channel3);
}


/**
 * @brief Create a representation of the current image visualizing each pixel with the mean color value of its assigned label.
 * @param labelImage label identifier of each pixel
 * @param out_regionMeanImage will be (re)allocated if necessary and filled with the described visualization
 */
template <typename TLabelImage>
void ColorFeature<TLabelImage>::generateRegionMeanImage(cv::Mat const& labelImage, cv::Mat& out_regionMeanImage) const
{
    assert(labelImage.size() == channel1.size());
    assert(labelImage.type() == cv::DataType<TLabelImage>::type);

    // Generate an image which represents all pixels by the mean color value of their label.
    // Channels will be filled separately and later on merged, since accessing multi-channel
    // matrices is not quite straightforward.
    std::vector<cv::Mat> out_channels(3);
    out_channels[0].create(channel1.size(), cv::DataType<TColorData>::type);
    out_channels[1].create(channel1.size(), cv::DataType<TColorData>::type);
    out_channels[2].create(channel1.size(), cv::DataType<TColorData>::type);

    for (int row = 0; row < out_channels[0].rows; ++row)
    {
        TColorData* const out_chan1RowPtr = out_channels[0].ptr<TColorData>(row);
        TColorData* const out_chan2RowPtr = out_channels[1].ptr<TColorData>(row);
        TColorData* const out_chan3RowPtr = out_channels[2].ptr<TColorData>(row);
        TLabelImage const* const labelImRowPtr = labelImage.ptr<TLabelImage>(row);

        for (int col = 0; col < out_channels[0].cols; ++col)
        {
            out_chan1RowPtr[col] = labelStatisticsChan1[labelImRowPtr[col]].valueSum
                / labelStatisticsChan1[labelImRowPtr[col]].pixelCount;
            out_chan2RowPtr[col] = labelStatisticsChan2[labelImRowPtr[col]].valueSum
                / labelStatisticsChan2[labelImRowPtr[col]].pixelCount;
            out_chan3RowPtr[col] = labelStatisticsChan3[labelImRowPtr[col]].valueSum
                / labelStatisticsChan3[labelImRowPtr[col]].pixelCount;
        }
    }

    // Merge the channels into one 3-dim output image.
    // out_regionMeanImage will be (re)allocated automatically by cv::merge if necessary.
    cv::merge(out_channels, out_regionMeanImage);
}
