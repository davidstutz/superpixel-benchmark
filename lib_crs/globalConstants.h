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
 * @file globalConstants.h
 * @brief Header file containing some global constant definitions.
 */


/**
 * @brief The threshold for minimum variance.
 *
 * Minimum variance > 0 is necessary to prevent labels with only one pixel from staying alive forever.
 * Value is arbitrary but works. As long as it is not too large, the exact value does not influence the
 * results much, so this is not needed as a parameter.
 */
double const featuresMinVariance = 1.0 / 12.0;
