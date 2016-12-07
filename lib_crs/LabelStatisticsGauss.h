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

#include <boost/cstdint.hpp>


/**
 * @struct LabelStatisticsGauss
 * @brief Struct containing the sufficient statistics needed to compute label likelihoods of a Gaussian-distributed feature channel.
 */
struct LabelStatisticsGauss
{
    boost::uint_fast32_t pixelCount; ///< the number of pixels assigned to the label
    double valueSum; ///< the sum of values of all pixels assigned to the label
    double squareValueSum; ///< the sum of squared values of all pixels assigned to the label

    /**
     * @brief Default constructor to ensure that all members are initialized to sensible values.
     */
    LabelStatisticsGauss() : pixelCount(0), valueSum(0), squareValueSum(0) {}
};
