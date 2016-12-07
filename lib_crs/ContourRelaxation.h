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

#include "FeatureType.h"
#include "GrayvalueFeature.h"
#include "ColorFeature.h"
#include "CompactnessFeature.h"
#include "DepthFeature.h"
#include "TraversionGenerator.h"

#include <opencv2/opencv.hpp>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <assert.h>
#include <vector>
#include <algorithm>
#include <math.h>


/**
 * @class ContourRelaxation
 * @brief Main class for applying Contour Relaxation to a label image, using an arbitrary set of features.
 */
template <typename TLabelImage>
class ContourRelaxation
{
    private:

        boost::shared_ptr< GrayvalueFeature<TLabelImage> > grayvalueFeature; ///< Pointer to the grayvalue feature object, if enabled.
        bool grayvalueFeatureEnabled; ///< True if grayvalue feature is enabled.

        boost::shared_ptr< ColorFeature<TLabelImage> > colorFeature; ///< Pointer to the color feature object, if enabled.
        bool colorFeatureEnabled; ///< True if color feature is enabled.

        boost::shared_ptr< DepthFeature<TLabelImage> > depthFeature; ///< Pointer to the color feature object, if enabled.
        bool depthFeatureEnabled; ///< True if color feature is enabled.
        
        boost::shared_ptr< CompactnessFeature<TLabelImage> > compactnessFeature; ///< Pointer to the compactness feature object, if enabled.
        bool compactnessFeatureEnabled; ///< True if compactness feature is enabled.

        std::vector< boost::shared_ptr< IFeature<TLabelImage> > > allFeatures; ///< Vector of pointers to all enabled feature objects.
        typedef typename std::vector< boost::shared_ptr< IFeature<TLabelImage> > >::const_iterator FeatureIterator; ///< Shorthand for const_iterator over vector of feature pointers.

        std::vector<TLabelImage> getNeighbourLabels(cv::Mat const& labelImage, cv::Point2i const& curPixelCoords) const;

        double calculateCost(cv::Mat const& labelImage, cv::Point2i const& curPixelCoords,
            TLabelImage const& pretendLabel, std::vector<TLabelImage> const& neighbourLabels,
            double const& directCliqueCost, double const& diagonalCliqueCost) const;

        double calculateCliqueCost(cv::Mat const& labelImage, cv::Point2i const& curPixelCoords, TLabelImage const& pretendLabel,
            double const& directCliqueCost, double const& diagonalCliqueCost) const;

        void computeBoundaryMap(cv::Mat const& labelImage, cv::Mat& out_boundaryMap) const;

        void updateBoundaryMap(cv::Mat const& labelImage, cv::Point2i const& curPixelCoords, cv::Mat& boundaryMap) const;


    public:

        ContourRelaxation(std::vector<FeatureType> features);

        void relax(cv::Mat const& labelImage, double const& directCliqueCost, double const& diagonalCliqueCost,
            unsigned int const& numIterations, cv::Mat& out_labelImage, cv::Mat& out_regionMeanImage) const;

        void setGrayvalueData(cv::Mat const& grayvalueImage);

        void setColorData(cv::Mat const& channel1, cv::Mat const& channel2, cv::Mat const& channel3);

        void setDepthData(cv::Mat const& depth, double const& depthWeight);
        
        void setCompactnessData(double const& compactnessWeight);
};


/**
 * @brief Constructor. Create a ContourRelaxation object with the specified features enabled.
 * @param features contains the features to be enabled
 */
template <typename TLabelImage>
ContourRelaxation<TLabelImage>::ContourRelaxation(std::vector<FeatureType> features)
{
    // First set all features to disabled.
    grayvalueFeatureEnabled = false;
    colorFeatureEnabled = false;
    compactnessFeatureEnabled = false;
    depthFeatureEnabled = false;
    allFeatures = std::vector< boost::shared_ptr< IFeature<TLabelImage> > >();

    // Remove duplicates from the vector of features.
    // First sort the vector, then remove consecutive duplicates, then resize.
    std::sort(features.begin(), features.end());
    typename std::vector<FeatureType>::iterator newVecEnd = std::unique(features.begin(), features.end());
    features.resize(newVecEnd - features.begin());

    // Now each feature type can only occur once or not at all in the features vector, so we can just create
    // one feature object for each found feature type without further checking.
    for (std::vector<FeatureType>::const_iterator it_curFeature = features.begin();
        it_curFeature != features.end(); ++it_curFeature)
    {
        switch (*it_curFeature)
        {
            case Grayvalue:
                grayvalueFeature = boost::shared_ptr< GrayvalueFeature<TLabelImage> >
                    (new GrayvalueFeature<TLabelImage>());
                allFeatures.push_back(grayvalueFeature);
                grayvalueFeatureEnabled = true;
                break;

            case Color:
                colorFeature = boost::shared_ptr< ColorFeature<TLabelImage> >
                    (new ColorFeature<TLabelImage>());
                allFeatures.push_back(colorFeature);
                colorFeatureEnabled = true;
                break;

            case Compactness:
                compactnessFeature = boost::shared_ptr< CompactnessFeature<TLabelImage> >
                    (new CompactnessFeature<TLabelImage>());
                allFeatures.push_back(compactnessFeature);
                compactnessFeatureEnabled = true;
                break;
                
            case Depth:
                depthFeature = boost::shared_ptr< DepthFeature<TLabelImage> >
                    (new DepthFeature<TLabelImage>());
                allFeatures.push_back(depthFeature);
                depthFeatureEnabled = true;
                break;
                
        }
    }
}


/**
 * @brief Apply Contour Relaxation to the given label image, with the features enabled in this ContourRelaxation object.
 * @param labelImage the input label image, containing one label identifier per pixel
 * @param directCliqueCost Markov clique cost for one clique in horizontal or vertical direction
 * @param diagonalCliqueCost Markov clique cost for one clique in diagonal direction
 * @param numIterations number of iterations of Contour Relaxation to be performed (one iteration can include multiple passes)
 * @param out_labelImage the resulting label image after Contour Relaxation, will be (re)allocated if necessary
 * @param out_regionMeanImage the region mean image of the resulting label image (if grayvalue or color feature enabled, else an empty matrix header)
 *
 * One iteration of Contour Relaxation may pass over the image multiple times, in changing directions, in order to
 * mitigate the dependency of the result on the chosen order in which pixels are processed. This dependency comes from
 * the greedy nature of the performed optimization.
 */
template <typename TLabelImage>
void ContourRelaxation<TLabelImage>::relax(cv::Mat const& labelImage, double const& directCliqueCost, double const& diagonalCliqueCost,
    unsigned int const& numIterations, cv::Mat& out_labelImage, cv::Mat& out_regionMeanImage) const
{
    assert(labelImage.type() == cv::DataType<TLabelImage>::type);
    assert(directCliqueCost >= 0);
    assert(diagonalCliqueCost >= 0);

    // Copy the label image to the output variable. From then on, always work on the output label image!
    // Changes to the input label image are impossible anyway since it's const, but we also need to read
    // from the updated label image in each step because we have an iterative algorithm.
    labelImage.copyTo(out_labelImage);

    // Compute the initial statistics of all labels given in the label image, for all features.
    for (FeatureIterator it_curFeature = allFeatures.begin(); it_curFeature != allFeatures.end(); ++it_curFeature)
    {
        (*it_curFeature)->initializeStatistics(out_labelImage);
    }

    // Create the initial boundary map.
    cv::Mat boundaryMap;
    computeBoundaryMap(out_labelImage, boundaryMap);

    // Create a traversion generator object, which will give us all the pixel coordinates in the current image
    // in all traversion orders specified inside that class. We will just need to loop over the coordinates
    // we receive by this object.
    TraversionGenerator traversionGen;

    // Loop over specified number of iterations.
    for (unsigned int curIteration = 0; curIteration < numIterations; ++curIteration)
    {
        // Loop over all coordinates received by the traversion generator.
        // It is important to start with begin() here, which does not only set the correct image size,
        // but also resets all internal counters.
        for (cv::Point2i curPixelCoords = traversionGen.begin(labelImage.size()); curPixelCoords != traversionGen.end();
            curPixelCoords = traversionGen.nextPixel())
        {
            if (boundaryMap.at<unsigned char>(curPixelCoords) == 0)
            {
                // We are not at a boundary pixel, no further processing necessary.
                continue;
            }

            // Get all neighbouring labels. This vector also contains the label of the current pixel itself.
            std::vector<TLabelImage> const neighbourLabels = getNeighbourLabels(out_labelImage, curPixelCoords);

            // If we have more than one label in the neighbourhood, the current pixel is a boundary pixel
            // and optimization will be carried out. Else, the neighbourhood only contains the label of the
            // pixel itself (since this label will definitely be there, and there is only one), so we don't
            // have a boundary pixel.
            if (neighbourLabels.size() > 1)
            {
                std::vector<double> costs(neighbourLabels.size());
                std::vector<double>::iterator it_costs = costs.begin();

                for (typename std::vector<TLabelImage>::const_iterator it_neighbourLabel = neighbourLabels.begin();
                    it_neighbourLabel != neighbourLabels.end(); ++it_neighbourLabel, ++it_costs)
                {
                    *it_costs = calculateCost(out_labelImage, curPixelCoords, *it_neighbourLabel,
                                                neighbourLabels, directCliqueCost, diagonalCliqueCost);
                }

                // Find the minimum cost.
                std::vector<double>::iterator const it_minCost = std::min_element(costs.begin(), costs.end());

                // Get the index of the minimum cost in the costs vector, which is also the index of the associated label in the neighbourhood.
                std::vector<double>::size_type const minCostIndex = std::distance(costs.begin(), it_minCost);

                // Get the label associated with the minimum cost.
                TLabelImage bestLabel = neighbourLabels[minCostIndex];

                // If we have found a better label for the pixel, update the statistics for all features
                // and change the label of the pixel.
                if (bestLabel != out_labelImage.at<TLabelImage>(curPixelCoords))
                {
                    for (FeatureIterator it_curFeature = allFeatures.begin(); it_curFeature != allFeatures.end(); ++it_curFeature)
                    {
                        (*it_curFeature)->updateStatistics(curPixelCoords, out_labelImage.at<TLabelImage>(curPixelCoords),
                            bestLabel);
                    }

                    out_labelImage.at<TLabelImage>(curPixelCoords) = bestLabel;

                    // We also need to update the boundary map around the current pixel.
                    updateBoundaryMap(out_labelImage, curPixelCoords, boundaryMap);
                }
            }
        }
    }

    // Generate an image which represents all pixels by the mean grayvalue of their label.
    if (colorFeatureEnabled == true)
    {
        colorFeature->generateRegionMeanImage(out_labelImage, out_regionMeanImage);
    }
    else if (grayvalueFeatureEnabled == true)
    {
        grayvalueFeature->generateRegionMeanImage(out_labelImage, out_regionMeanImage);
    }
    else
    {
        out_regionMeanImage = cv::Mat();
    }
}


/**
 * @brief Get all labels in the 8-neighbourhood of a pixel, including the label of the center pixel itself.
 * @param labelImage the current label image, contains one label identifier per pixel
 * @param curPixelCoords the coordinates of the regarded pixel
 * @return a vector containing all labels in the neighbourhood, each only once, sorted in ascending order
 */
template <typename TLabelImage>
std::vector<TLabelImage> ContourRelaxation<TLabelImage>::getNeighbourLabels(cv::Mat const& labelImage, cv::Point2i const& curPixelCoords)
    const
{
    assert(labelImage.type() == cv::DataType<TLabelImage>::type);
    assert(curPixelCoords.inside(cv::Rect(0, 0, labelImage.cols, labelImage.rows)));

    // To get all pixels in the 8-neighbourhood (or 9, since the rectangle includes the central pixel itself)
    // we form the intersection between the theoretical full neighbourhood and the bounding area.
    cv::Rect const fullNeighbourhoodRect(curPixelCoords.x - 1, curPixelCoords.y - 1, 3, 3);
    cv::Rect const boundaryRect(0, 0, labelImage.cols, labelImage.rows);
    cv::Rect const croppedNeighbourhoodRect = fullNeighbourhoodRect & boundaryRect;

    // Get a new matrix header to the relevant neighbourhood in the label image.
    cv::Mat const neighbourhoodLabelImage = labelImage(croppedNeighbourhoodRect);

    // Push all labels in the neighbourhood into a vector.
    // Reserve enough space for the maximum of 9 labels in the neighbourhood.
    // Making this one big allocation is extremely faster than making multiple small allocations when pushing elements.
    std::vector<TLabelImage> neighbourLabels;
    neighbourLabels.reserve(9);

    for (int row = 0; row < neighbourhoodLabelImage.rows; ++row)
    {
        TLabelImage const* const neighbLabelsRowPtr = neighbourhoodLabelImage.ptr<TLabelImage>(row);

        for (int col = 0; col < neighbourhoodLabelImage.cols; ++col)
        {
            neighbourLabels.push_back(neighbLabelsRowPtr[col]);
        }
    }

    // Remove duplicates from the vector of neighbour labels.
    // First sort the vector, then remove consecutive duplicates, then resize.
    std::sort(neighbourLabels.begin(), neighbourLabels.end());
    typename std::vector<TLabelImage>::iterator newVecEnd = std::unique(neighbourLabels.begin(), neighbourLabels.end());
    neighbourLabels.resize(newVecEnd - neighbourLabels.begin());

    return neighbourLabels;
}


/**
 * @brief Calculate the total cost of all labels in the 8-neighbourhood of a pixel, assuming the pixel would change its label.
 * @param labelImage the current label image, contains one label identifier per pixel
 * @param curPixelCoords coordinates of the regarded pixel
 * @param pretendLabel assumed new label of the regarded pixel
 * @param neighbourLabels all labels in the neighbourhood of the regarded pixel, including the label of the pixel itself
 * @param directCliqueCost Markov clique cost for one clique in horizontal or vertical direction
 * @param diagonalCliqueCost Markov clique cost for one clique in diagonal direction
 * @return the total cost, summed over all labels in the neighbourhood and all enabled features, plus the Markov clique costs
 */
template <typename TLabelImage>
double ContourRelaxation<TLabelImage>::calculateCost(cv::Mat const& labelImage, cv::Point2i const& curPixelCoords,
    TLabelImage const& pretendLabel, std::vector<TLabelImage> const& neighbourLabels,
    double const& directCliqueCost, double const& diagonalCliqueCost) const
{
    assert(labelImage.type() == cv::DataType<TLabelImage>::type);
    assert(curPixelCoords.inside(cv::Rect(0, 0, labelImage.cols, labelImage.rows)));

    // Calculate clique cost.
    double cost = calculateCliqueCost(labelImage, curPixelCoords, pretendLabel, directCliqueCost, diagonalCliqueCost);

    // Calculate and add up the costs of all features.
    TLabelImage const oldLabel = labelImage.at<TLabelImage>(curPixelCoords);

    for (FeatureIterator it_curFeature = allFeatures.begin(); it_curFeature != allFeatures.end(); ++it_curFeature)
    {
        cost += (*it_curFeature)->calculateCost(curPixelCoords, oldLabel, pretendLabel, neighbourLabels);
    }

    return cost;
}


/**
 * @brief Calculate the Markov clique cost of a pixel, assuming it would change its label.
 * @param labelImage the current label image, contains one label identifier per pixel
 * @param curPixelCoords coordinates of the regarded pixel
 * @param pretendLabel assumed new label of the regarded pixel
 * @param directCliqueCost Markov clique cost for one clique in horizontal or vertical direction
 * @param diagonalCliqueCost Markov clique cost for one clique in diagonal direction
 * @return the total Markov clique cost for the given label at the given pixel coordinates
 */
template <typename TLabelImage>
double ContourRelaxation<TLabelImage>::calculateCliqueCost(cv::Mat const& labelImage, cv::Point2i const& curPixelCoords,
    TLabelImage const& pretendLabel, double const& directCliqueCost, double const& diagonalCliqueCost) const
{
    assert(labelImage.type() == cv::DataType<TLabelImage>::type);
    assert(curPixelCoords.inside(cv::Rect(0, 0, labelImage.cols, labelImage.rows)));

    // Find number of (direct / diagonal) cliques around pixelIndex, pretending the pixel at
    // curPixelCoords belongs to pretendLabel. Then calculate and return the associated combined cost.

    // Create a rectangle spanning the image area. This will be used to check if points are inside the image area.
    cv::Rect boundaryRect(0, 0, labelImage.cols, labelImage.rows);

    // Direct cliques.

    // Store the differences in coordinates of all direct cliques in reference to the central pixel.
    // Fill this static vector on the first function call, the elements will never change.
    static std::vector<cv::Point2i> directCoordDiffs;

    if (directCoordDiffs.size() == 0)
    {
        directCoordDiffs.push_back(cv::Point2i(-1, 0));
        directCoordDiffs.push_back(cv::Point2i(1, 0));
        directCoordDiffs.push_back(cv::Point2i(0, -1));
        directCoordDiffs.push_back(cv::Point2i(0, 1));
    }

    int numDirectCliques = 0;

    // Loop over all direct clique coordinate differences.
    // Translate the central pixel by the current difference.
    // If the resulting coordinates are inside the image area, and the label there differs from the pretended label of the
    // central pixel, increase the number of direct cliques.
    for (std::vector<cv::Point2i>::const_iterator it_coordDiff = directCoordDiffs.begin(); it_coordDiff != directCoordDiffs.end(); ++it_coordDiff)
    {
        cv::Point2i comparisonCoords = curPixelCoords + *it_coordDiff;

        if (comparisonCoords.inside(boundaryRect) &&
            labelImage.at<TLabelImage>(comparisonCoords) != pretendLabel)
        {
            ++numDirectCliques;
        }
    }


    // Diagonal cliques.

    // Store the differences in coordinates of all diagonal cliques in reference to the central pixel.
    // Fill this static vector on the first function call, the elements will never change.
    static std::vector<cv::Point2i> diagonalCoordDiffs;

    if (diagonalCoordDiffs.size() == 0)
    {
        diagonalCoordDiffs.push_back(cv::Point2i(-1, -1));
        diagonalCoordDiffs.push_back(cv::Point2i(-1, 1));
        diagonalCoordDiffs.push_back(cv::Point2i(1, -1));
        diagonalCoordDiffs.push_back(cv::Point2i(1, 1));
    }

    int numDiagonalCliques = 0;

    // Loop over all diagonal clique coordinate differences.
    // Translate the central pixel by the current difference.
    // If the resulting coordinates are inside the image area, and the label there differs from the pretended label of the
    // central pixel, increase the number of diagonal cliques.
    for (std::vector<cv::Point2i>::const_iterator it_coordDiff = diagonalCoordDiffs.begin(); it_coordDiff != diagonalCoordDiffs.end(); ++it_coordDiff)
    {
        cv::Point2i comparisonCoords = curPixelCoords + *it_coordDiff;

        if (comparisonCoords.inside(boundaryRect) &&
            labelImage.at<TLabelImage>(comparisonCoords) != pretendLabel)
        {
            ++numDiagonalCliques;
        }
    }

    // Calculate and return the combined clique cost.
    double cost = numDirectCliques * directCliqueCost + numDiagonalCliques * diagonalCliqueCost;
    return cost;
}


/**
 * @brief Create a binary map highlighting pixels on the boundary of their respective labels (1 for boundary pixels, 0 otherwise).
 * @param labelImage the current label image, contains one label identifier per pixel
 * @param out_boundaryMap the resulting boundary map, will be (re)allocated if necessary, binary by nature but stored as unsigned char
 */
template <typename TLabelImage>
void ContourRelaxation<TLabelImage>::computeBoundaryMap(cv::Mat const& labelImage, cv::Mat& out_boundaryMap) const
{
    assert(labelImage.type() == cv::DataType<TLabelImage>::type);

    // Initialize (or reset) boundary map with zeros.
    out_boundaryMap = cv::Mat::zeros(labelImage.size(), cv::DataType<unsigned char>::type);

    // For each pixel, compare with neighbors. If different label, set both to 1 (= boundary pixel).
    // Compare only half of the neighbors, the other half will be compared when they themselves are the current pixel.
    for (int row = 0; row < labelImage.rows; ++row)
    {
        TLabelImage const* const labelImageUpperRowPtr = labelImage.ptr<TLabelImage>(row);
        unsigned char* const boundaryImageUpperRowPtr = out_boundaryMap.ptr<unsigned char>(row);

        TLabelImage const* labelImageLowerRowPtr = 0;
        unsigned char* boundaryImageLowerRowPtr = 0;

        // Check whether we have one more row downwards.
        // We can only get the row pointers to that row if it exists, obviously.
        bool canLookDown = false;
        if (row < labelImage.rows - 1)
        {
            labelImageLowerRowPtr = labelImage.ptr<TLabelImage>(row + 1);
            boundaryImageLowerRowPtr = out_boundaryMap.ptr<unsigned char>(row + 1);
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
}


/**
 * @brief Update a boundary map to reflect a label change of a single pixel.
 * @param labelImage the current label image (after the label change), contains one label identifier per pixel
 * @param curPixelCoords the coordinates of the changed pixel
 * @param boundaryMap the boundary map before the label change, will be updated if necessary to be consistent with the change
 */
template <typename TLabelImage>
void ContourRelaxation<TLabelImage>::updateBoundaryMap(cv::Mat const& labelImage, cv::Point2i const& curPixelCoords,
    cv::Mat& boundaryMap) const
{
    // Update the boundary map in the 8-neighbourhood around curPixelCoords.
    // This needs to be done each time a pixel's label was changed.

    assert(labelImage.type() == cv::DataType<TLabelImage>::type);
    assert(boundaryMap.type() == cv::DataType<unsigned char>::type);
    assert(boundaryMap.size() == labelImage.size());
    assert(curPixelCoords.inside(cv::Rect(0, 0, labelImage.cols, labelImage.rows)));

    // The current pixel can influence all pixels in the 8-neighborhood (and itself).
    // But for the neighbors we also need to look at all their neighbors, so we need to
    // scan a 5x5 window centered around the current pixel, but only update the central 3x3 window
    // in the boundary map.

    // Find out how far we can look in all four directions around the current pixel, i.e. handle border pixels.
    // We are only interested in a maximum distance of 2 in all directions, resulting in a 5x5 window.
    unsigned char const win5SizeLeft = std::min(2, curPixelCoords.x);
    unsigned char const win5SizeRight = std::min(2, labelImage.cols - 1 - curPixelCoords.x);
    unsigned char const win5SizeTop = std::min(2, curPixelCoords.y);
    unsigned char const win5SizeBottom = std::min(2, labelImage.rows - 1 - curPixelCoords.y);

    cv::Rect const window5by5(curPixelCoords.x - win5SizeLeft, curPixelCoords.y - win5SizeTop,
        win5SizeLeft + 1 + win5SizeRight, win5SizeTop + 1 + win5SizeBottom);

    // Compute a boundary map for the (maximum) 5x5 window around the current pixel.
    cv::Mat const labelArray5Window = labelImage(window5by5);
    cv::Mat boundaryMap5by5;
    computeBoundaryMap(labelArray5Window, boundaryMap5by5);

    // Find out which parts of the 8-neighborhood are available around the current pixel
    // and get a window for this potentially cropped 8-neighborhood.
    unsigned char const win3SizeLeft = std::min(1, curPixelCoords.x);
    unsigned char const win3SizeRight = std::min(1, labelImage.cols - 1 - curPixelCoords.x);
    unsigned char const win3SizeTop = std::min(1, curPixelCoords.y);
    unsigned char const win3SizeBottom = std::min(1, labelImage.rows - 1 - curPixelCoords.y);

    cv::Rect const window3by3(curPixelCoords.x - win3SizeLeft, curPixelCoords.y - win3SizeTop,
        win3SizeLeft + 1 + win3SizeRight, win3SizeTop + 1 + win3SizeBottom);

    // Get the coordinates of the top-left corner of the (cropped) 8-neighborhood in the
    // temporary 5x5 boundary map. The width and height of the 8-neighborhood is the same
    // as above.
    unsigned char const tempBoundaryMapWin3x = std::max(win5SizeLeft - 1, 0);
    unsigned char const tempBoundaryMapWin3y = std::max(win5SizeTop - 1, 0);

    cv::Rect const tempBoundaryMapWin3by3(tempBoundaryMapWin3x, tempBoundaryMapWin3y,
        window3by3.width, window3by3.height);

    // Copy the central (cropped) 3x3 window of the up-to-date (cropped) 5x5 boundary map
    // to the (cropped) 3x3 window in the full boundary map.
    cv::Mat boundaryMap3Window = boundaryMap(window3by3);
    boundaryMap5by5(tempBoundaryMapWin3by3).copyTo(boundaryMap3Window);
}


/**
 * @brief Set the observed data for the grayvalue feature.
 * @param grayvalueImage the observed grayvalue image
 */
template <typename TLabelImage>
void ContourRelaxation<TLabelImage>::setGrayvalueData(cv::Mat const& grayvalueImage)
{
    assert(grayvalueFeatureEnabled == true);

    grayvalueFeature->setData(grayvalueImage);
}


/**
 * @brief Set the observed data for the color feature.
 * @param channel1 the observed first image channel
 * @param channel2 the observed second image channel
 * @param channel3 the observed third image channel
 */
template <typename TLabelImage>
void ContourRelaxation<TLabelImage>::setColorData(cv::Mat const& channel1, cv::Mat const& channel2, cv::Mat const& channel3)
{
    assert(colorFeatureEnabled == true);

    colorFeature->setData(channel1, channel2, channel3);
}

/**
 * @brief Set the observed data for the color feature.
 * @param channel1 the observed first image channel
 * @param channel2 the observed second image channel
 * @param channel3 the observed third image channel
 */
template <typename TLabelImage>
void ContourRelaxation<TLabelImage>::setDepthData(cv::Mat const& depth, double const& depthWeight)
{
    assert(depthFeatureEnabled == true);

    depthFeature->setData(depth, depthWeight);
}


/**
 * @brief Set the compactness weight for the compactness feature.
 * @param compactnessWeight the compactness weight to be multiplied with the compactness cost before returning that
 */
template <typename TLabelImage>
void ContourRelaxation<TLabelImage>::setCompactnessData(double const& compactnessWeight)
{
    assert(compactnessFeatureEnabled == true);

    compactnessFeature->setData(compactnessWeight);
}
