/* 
 * File:   DepthFeature.h
 * Author: david
 *
 * Created on August 10, 2014, 9:02 PM
 */

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
class DepthFeature : public AGaussianFeature<TLabelImage>
{
    private:

        typedef unsigned short TDepthData; ///< the type of the used image channels

        double featureWeight;
        std::vector<LabelStatisticsGauss> labelStatistics; ///< Gaussian label statistics of the first channel
        cv::Mat depth; ///< observed data of the first channel

    public:

        void setData(cv::Mat const& depth, double const& depthWeight);

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
void DepthFeature<TLabelImage>::setData(cv::Mat const& depth, double const& depthWeight)
{
    assert(depth.type() == cv::DataType<TDepthData>::type);

    depth.copyTo(this->depth);
    featureWeight = depthWeight;
}


/**
 * @brief Estimate the label statistics of all labels in the given label image, using the observed data saved in the feature object.
 * @param labelImage label identifiers of all pixels
 */
template <typename TLabelImage>
void DepthFeature<TLabelImage>::initializeStatistics(cv::Mat const& labelImage)
{
    // Use the provided initialization method for gaussian statistics from AGaussianFeature.
    this->template initializeGaussianStatistics<TDepthData>(labelImage, depth, labelStatistics);
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
double DepthFeature<TLabelImage>::calculateCost(cv::Point2i const& curPixelCoords,
    TLabelImage const& oldLabel, TLabelImage const& pretendLabel, std::vector<TLabelImage> const& neighbourLabels) const
{
    // Use the provided cost calculation method for gaussian statistics from AGaussianFeature.
    double cost = this->template calculateGaussianCost<TDepthData>(curPixelCoords, oldLabel, pretendLabel,
            neighbourLabels, this->labelStatistics, this->depth);

    return featureWeight * cost;
}


/**
 * @brief Update the saved label statistics to reflect a label change of the given pixel.
 * @param curPixelCoords coordinates of the pixel whose label changes
 * @param oldLabel old label of the changing pixel
 * @param newLabel new label of the changing pixel
 */
template <typename TLabelImage>
void DepthFeature<TLabelImage>::updateStatistics(cv::Point2i const& curPixelCoords, TLabelImage const& oldLabel,
    TLabelImage const& newLabel)
{
    this->template updateGaussianStatistics<TDepthData>(curPixelCoords, this->labelStatistics[oldLabel],
        this->labelStatistics[newLabel], this->depth);
}


/**
 * @brief Create a representation of the current image visualizing each pixel with the mean color value of its assigned label.
 * @param labelImage label identifier of each pixel
 * @param out_regionMeanImage will be (re)allocated if necessary and filled with the described visualization
 */
template <typename TLabelImage>
void DepthFeature<TLabelImage>::generateRegionMeanImage(cv::Mat const& labelImage, cv::Mat& out_regionMeanImage) const
{
    assert(labelImage.size() == this->depth.size());
    assert(labelImage.type() == cv::DataType<TLabelImage>::type);

    // Generate an image which represents all pixels by the mean color value of their label.
    // Channels will be filled separately and later on merged, since accessing multi-channel
    // matrices is not quite straightforward.
    cv::Mat out_depth(this->depth.size(), cv::DataType<TDepthData>::type);

    for (int row = 0; row < out_depth.rows; ++row)
    {
        TDepthData* const out_depthRowPtr = out_depth.ptr<TDepthData>(row);
        TLabelImage const* const labelImRowPtr = labelImage.ptr<TLabelImage>(row);

        for (int col = 0; col < out_depth.cols; ++col)
        {
            out_depthRowPtr[col] = this->labelStatistics[labelImRowPtr[col]].valueSum
                / this->labelStatistics[labelImRowPtr[col]].pixelCount;
        }
    }
}


