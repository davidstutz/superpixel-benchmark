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
#include "LabelStatisticsGauss.h"

#include <opencv2/opencv.hpp>
#include <vector>


/**
 * @class CompactnessFeature
 * @brief Feature class for computing a cost based on the spatial distribution of a label, to enforce compactness.
 */
template <typename TLabelImage>
class CompactnessFeature : public IFeature<TLabelImage>
{
    private:

        double featureWeight; ///< the weight with which to multiply computed costs before returning them
        std::vector<LabelStatisticsGauss> labelStatisticsPosX; ///< the statistics of x-positions of all labels
        std::vector<LabelStatisticsGauss> labelStatisticsPosY; ///< the statistics of y-positions of all labels

        void updateStatistics(cv::Point2i const& curPixelCoords,
            LabelStatisticsGauss& labelStatsOldLabelPosX, LabelStatisticsGauss& labelStatsNewLabelPosX,
            LabelStatisticsGauss& labelStatsOldLabelPosY, LabelStatisticsGauss& labelStatsNewLabelPosY) const;


    public:

        void setData(double const& compactnessWeight);

        void initializeStatistics(cv::Mat const& labelImage);

        double calculateCost(cv::Point2i const& curPixelCoords,
            TLabelImage const& oldLabel, TLabelImage const& pretendLabel, std::vector<TLabelImage> const& neighbourLabels) const;

        void updateStatistics(cv::Point2i const& curPixelCoords, TLabelImage const& oldLabel, TLabelImage const& newLabel);
};


/**
 * @brief Update label statistics to reflect a label change of the given pixel.
 * @param curPixelCoords coordinates of the pixel changing its label
 * @param labelStatsOldLabelPosX label statistics for x-positions of the old label (will be updated)
 * @param labelStatsNewLabelPosX label statistics for x-positions of the new label (will be updated)
 * @param labelStatsOldLabelPosY label statistics for y-positions of the old label (will be updated)
 * @param labelStatsNewLabelPosY label statistics for y-positions of the new label (will be updated)
 */
template <typename TLabelImage>
void CompactnessFeature<TLabelImage>::updateStatistics(cv::Point2i const& curPixelCoords,
    LabelStatisticsGauss& labelStatsOldLabelPosX, LabelStatisticsGauss& labelStatsNewLabelPosX,
    LabelStatisticsGauss& labelStatsOldLabelPosY, LabelStatisticsGauss& labelStatsNewLabelPosY) const
{
    // Update pixel count.
    labelStatsOldLabelPosX.pixelCount--;
    labelStatsNewLabelPosX.pixelCount++;

    // Update x-position sum.
    labelStatsOldLabelPosX.valueSum -= curPixelCoords.x;
    labelStatsNewLabelPosX.valueSum += curPixelCoords.x;

    // Update square x-position sum.
    labelStatsOldLabelPosX.squareValueSum -= pow(curPixelCoords.x, 2.0);
    labelStatsNewLabelPosX.squareValueSum += pow(curPixelCoords.x, 2.0);

    // Update pixel count.
    labelStatsOldLabelPosY.pixelCount--;
    labelStatsNewLabelPosY.pixelCount++;

    // Update x-position sum.
    labelStatsOldLabelPosY.valueSum -= curPixelCoords.y;
    labelStatsNewLabelPosY.valueSum += curPixelCoords.y;

    // Update square x-position sum.
    labelStatsOldLabelPosY.squareValueSum -= pow(curPixelCoords.y, 2.0);
    labelStatsNewLabelPosY.squareValueSum += pow(curPixelCoords.y, 2.0);
}


/**
 * @brief Set the weight to use when computing this feature's cost.
 * @param compactnessWeight weight (factor) with which to multiply the cost before returning it
 */
template <typename TLabelImage>
void CompactnessFeature<TLabelImage>::setData(double const& compactnessWeight)
{
    assert(compactnessWeight >= 0);

    featureWeight = compactnessWeight;
}


/**
 * @brief Estimate the statistics of the spatial distribution for each label.
 * @param labelImage contains the label identifier to which each pixel is assigned
 */
template <typename TLabelImage>
void CompactnessFeature<TLabelImage>::initializeStatistics(cv::Mat const& labelImage)
{
    assert(labelImage.type() == cv::DataType<TLabelImage>::type);

    // Find maximum label identifier in label image.
    TLabelImage maxLabelId = 0;

    for (int row = 0; row < labelImage.rows; ++row)
    {
        TLabelImage const* const labelImageRowPtr = labelImage.ptr<TLabelImage>(row);

        for (int col = 0; col < labelImage.cols; ++col)
        {
            maxLabelId = std::max(maxLabelId, labelImageRowPtr[col]);
        }
    }

    // Allocate the vectors of label statistics, with the maximum index being the maximum label identifier.
    // This might waste a small amount of memory, but we can use the label identifier as index for this vector.
    labelStatisticsPosX = std::vector<LabelStatisticsGauss>(maxLabelId + 1, LabelStatisticsGauss());
    labelStatisticsPosY = std::vector<LabelStatisticsGauss>(maxLabelId + 1, LabelStatisticsGauss());

    for (int row = 0; row < labelImage.rows; ++row)
    {
        TLabelImage const* const labelImageRowPtr = labelImage.ptr<TLabelImage>(row);

        for (int col = 0; col < labelImage.cols; ++col)
        {
            TLabelImage const curLabel = labelImageRowPtr[col];

            labelStatisticsPosX[curLabel].pixelCount++;
            labelStatisticsPosX[curLabel].valueSum += col;
            labelStatisticsPosX[curLabel].squareValueSum += pow(col, 2.0);

            labelStatisticsPosY[curLabel].pixelCount++;
            labelStatisticsPosY[curLabel].valueSum += row;
            labelStatisticsPosY[curLabel].squareValueSum += pow(row, 2.0);
        }
    }
}


/**
 * @brief Calculate the total cost of all labels in the 8-neighbourhood of a pixel, assuming the pixel would change its label.
 * @param curPixelCoords coordinates of the regarded pixel
 * @param oldLabel old label of the regarded pixel
 * @param pretendLabel assumed new label of the regarded pixel
 * @param neighbourLabels all labels found in the 8-neighbourhood of the regarded pixel, including the old label of the pixel itself
 * @return weighted total cost of all labels in the 8-neighbourhood
 *
 * The cost is in this case not defined by a probabilistic distribution, but as the
 * sum over the squared distance of each pixel of all regarded labels from the spatial center of the pixel's label.
 * Because of this definition, we need a weight to adjust how much this cost should influence the total cost.
 * The usual assumption that we have a likelihood which is independent of all other features and can just be
 * added (since we use log-likelihoods) does not hold here.
 */
template <typename TLabelImage>
double CompactnessFeature<TLabelImage>::calculateCost(cv::Point2i const& curPixelCoords,
    TLabelImage const& oldLabel, TLabelImage const& pretendLabel, std::vector<TLabelImage> const& neighbourLabels) const
{
    // Modify the label statistics if the pixel at curPixelCoords has another
    // label than pretendLabel. We only modify a local copy of the statistics,
    // however, since we do not actually want to change a label yet. We are
    // just calculating the cost of doing so.
    // We only copy the affected statistics, namely the ones of the old and the
    // new (pretend) label. We have to handle these two cases separately when accessing
    // the statistics, because we specifically need to access the local, modified versions.
    // But we save a lot of time since we do not need to create a copy of all statistics.
    LabelStatisticsGauss labelStatsPosXOldLabel(labelStatisticsPosX[oldLabel]);
    LabelStatisticsGauss labelStatsPosXPretendLabel(labelStatisticsPosX[pretendLabel]);

    LabelStatisticsGauss labelStatsPosYOldLabel(labelStatisticsPosY[oldLabel]);
    LabelStatisticsGauss labelStatsPosYPretendLabel(labelStatisticsPosY[pretendLabel]);

    if (oldLabel != pretendLabel)
    {
        updateStatistics(curPixelCoords, labelStatsPosXOldLabel, labelStatsPosXPretendLabel,
                         labelStatsPosYOldLabel, labelStatsPosYPretendLabel);
    }

    double featureCost = 0;

    // For each neighbouring label, add its cost.
    for (typename std::vector<TLabelImage>::const_iterator it_neighbourLabel = neighbourLabels.begin();
        it_neighbourLabel != neighbourLabels.end(); ++it_neighbourLabel)
    {
        // Get a pointer to the label statistics of the current label.
        // This should be the associated entry in the vector of label statistics, except for the
        // special cases below.
        LabelStatisticsGauss const* curLabelStatsPosX = &labelStatisticsPosX[*it_neighbourLabel];
        LabelStatisticsGauss const* curLabelStatsPosY = &labelStatisticsPosY[*it_neighbourLabel];

        // If the current label is the old one at curPixelCoords, or the pretended new one,
        // then its statistics have changed. In this case, we need to read the statistics
        // from the modified local variables.
        if (*it_neighbourLabel == oldLabel)
        {
            curLabelStatsPosX = &labelStatsPosXOldLabel;
            curLabelStatsPosY = &labelStatsPosYOldLabel;
        }
        else if (*it_neighbourLabel == pretendLabel)
        {
            curLabelStatsPosX = &labelStatsPosXPretendLabel;
            curLabelStatsPosY = &labelStatsPosYPretendLabel;
        }

        // If a label completely vanished, disregard it (can happen to old label of pixel_index).
        if (curLabelStatsPosX->pixelCount == 0)
        {
            continue;
        }

        // Add the cost of the current region.
        featureCost += curLabelStatsPosX->squareValueSum
                - (pow(curLabelStatsPosX->valueSum, 2) / curLabelStatsPosX->pixelCount);

        featureCost += curLabelStatsPosY->squareValueSum
                - (pow(curLabelStatsPosY->valueSum, 2) / curLabelStatsPosY->pixelCount);
    }

    return featureWeight * featureCost;
}


/**
 * @brief Update label statistics to reflect a label change of the given pixel.
 * @param curPixelCoords coordinates of the pixel changing its label
 * @param oldLabel old label of the regarded pixel
 * @param newLabel new label of the regarded pixel
 */
template <typename TLabelImage>
void CompactnessFeature<TLabelImage>::updateStatistics(cv::Point2i const& curPixelCoords, TLabelImage const& oldLabel,
    TLabelImage const& newLabel)
{
    updateStatistics(curPixelCoords, labelStatisticsPosX[oldLabel], labelStatisticsPosX[newLabel],
                     labelStatisticsPosY[oldLabel], labelStatisticsPosY[newLabel]);
}
