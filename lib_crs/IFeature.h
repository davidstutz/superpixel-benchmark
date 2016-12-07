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

#include <opencv2/opencv.hpp>


/**
 * @class IFeature
 * @brief Interface for feature classes. Defines the functions a feature class must implement so that it can be used in the Contour Relaxation framework.
 */
template <typename TLabelImage>
class IFeature
{
    public:

        /**
         * @brief Provide a virtual destructor so instances of derived classes can be safely destroyed through a pointer or reference of type IFeature.
         */
        virtual ~IFeature() {}

        /**
         * @brief Compute the internal label statistics for all labels in the given label image.
         * @param labelImage the current label image, contains one label identifier per pixel
         */
        virtual void initializeStatistics(cv::Mat const& labelImage) = 0;

        /**
         * @brief Calculate the total cost of all labels in the 8-neighbourhood of a pixel, assuming the pixel would change its label.
         * @param curPixelCoords coordinates of the regarded pixel
         * @param oldLabel old label of the regarded pixel
         * @param pretendLabel assumed new label of the regarded pixel
         * @param neighbourLabels all labels found in the 8-neighbourhood of the regarded pixel, including the old label of the pixel itself
         * @return total negative log-likelihood (or cost) of all labels in the neighbourhood, assuming the label change
         */
        virtual double calculateCost(cv::Point2i const& curPixelCoords,
                                     TLabelImage const& oldLabel, TLabelImage const& pretendLabel,
                                     std::vector<TLabelImage> const& neighbourLabels) const = 0;

        /**
         * @brief Update the saved label statistics to reflect a label change of the given pixel.
         * @param curPixelCoords coordinates of the pixel whose label changes
         * @param oldLabel old label of the changing pixel
         * @param newLabel new label of the changing pixel
         */
        virtual void updateStatistics(cv::Point2i const& curPixelCoords, TLabelImage const& oldLabel, TLabelImage const& newLabel) = 0;
};
