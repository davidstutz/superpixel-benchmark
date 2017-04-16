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

#ifndef EVALUATION_H
#define	EVALUATION_H

#include "io_util.h"

/** \brief Provides measures to evaluate (over-) segmentations.
 * \author David Stutz
 */
class Evaluation {
    friend class Visualization;
public:
    /** \brief Compute the Undersegmentation error as follows:
     * 
     *  \f$UE(G, S) = \frac{1}{N} = \sum_{S_j \in S} \min_{G_i} \{|G_i - S_j|\}\f$
     * 
     * \param[in] labels superpixel labels as int image
     * \param[in] gt ground truth segmentation as int image
     * \return UE(gt, labels)
     */
    static float computeUndersegmentationError(const cv::Mat &labels, 
            const cv::Mat &gt);

    /**
     * Compute the oversegmentation error as follows:
     * 
     *  $OE(G, S)$ = \frac{1}{N} \sum_{G_i \in G} \min_{S_j} \{|G_i - S_j\}$
     * 
     * @param sv_video
     * @param gt_video
     * @return 
     */
    static float computeOversegmentationError(const cv::Mat &labels, 
            const cv::Mat &gt);

    /** \brief Compute boundary recall:
     * 
     *  \f$Rec(S, G) = \frac{TP}{TP + FN}\f$
     * 
     * where TP is the number of true positives, and FN is the number of 
     * false negatives.
     * 
     * For each boundary pixel in the G, the corresponding boundary
     * pixel in S is allowed to deviate by a euclidean distance of d times the
     * image diagonal.
     * 
     * \param[in] labels superpixel labels as int image
     * \param[in] gt ground truth segmentation as int image
     * \param[in] d fraction of the diagonal to use as tolerance
     * \return Rec(labels, gt)
     */
    static float computeBoundaryRecall(const cv::Mat &labels, 
            const cv::Mat &gt, float d = 0.0025);
    
    /** \brief Compute boundary precision:
     * 
     *  \f$Pre(S, G) = \frac{TP}{TP + FP}\f$
     * 
     * where TP is the number of true positives, and FP is the number of false
     * positives.
     * 
     * For each boundary pixel in the G, the corresponding boundary
     * pixel in S is allowed to deviate by a euclidean distance of d times the
     * image diagonal.
     * 
     * \param[in] labels superpixel labels as int image
     * \pram[in] gt ground truth segmentation as int image
     * \param[in] d fraction of the diagonal to use as tolerance
     * \return Pre(labels, gt)
     */
    static float computeBoundaryPrecision(const cv::Mat &labels, 
            const cv::Mat &gt, float d = 0.0025);
    
    /** \brief Compute the explained variation of the given segmentation.
     * \param[in] labels superpixel labels as int image
     * \param[in] image image of the corresponding superpixel labels
     * \return explained variation
     */
    static float computeExplainedVariation(const cv::Mat &labels, const cv::Mat &image);
    
    /** \brief Computes the Undersegmentation Error (Neubert, Protzel):
     * 
     *  \f$UE_{NP}(S, G) = \frac{1}{N} \sum_{G_i \in G} \sum_{S_j \cap G_i \neq \emptyset} \min\{S_j \cap G_i, S_j - g_i\}\f$
     * 
     * \param[in] labels superpixel labels as int image
     * \pram[in] gt ground truth segmentation as int image
     * \return UE_NP(labels, gt)
     */
    static float computeNPUndersegmentationError(const cv::Mat &labels, 
            const cv::Mat &gt);
    
    /** \brief Compute the Undersegmentation Error (Levinshtein et al.):
     * 
     *  \f$UE_{Levin}(S, G) = \frac{1}{|G|} \sum_{G_i \in G} \frac{\sum_{S_j \cap G_i \neq \emptyset} |S_j| - |G_i|}{|G_i|}\f$
     * 
     * \param[in] labels superpixel labels as int image
     * \pram[in] gt ground truth segmentation as int image
     * \return UE_Levin(labels, gt)
     */
    static float computeLevinUndersegmentationError(const cv::Mat &labels, 
            const cv::Mat &gt);
    
    /** \brief Compute achievable segmentation accuracy as follows:
     * 
     *  \f$ASA(G, S) = \frac{1}{N_t} \sum_{S_j \in S} |S_j \cap G_{g(j)}|\f$
     * 
     * where \f$g(j)\f$ is the ground truth segment with maximum overlap with $S_j$.
     * 
     * \param[in] labels superpixel labels as int image
     * \pram[in] gt ground truth segmentation as int image
     * \return ASA(gt, labels)
     */
    static float computeAchievableSegmentationAccuracy(const cv::Mat &labels, 
            const cv::Mat &gt);
    
    /** \brief Compute Sum-of-Squared Error on RGB.
     * \param[in] labels superpixel labels as int image
     * \param[in] image image corresponding to the superpixel labels
     * \return sum-of-squared error on RGB
     */
    static float computeSumOfSquaredErrorRGB(const cv::Mat &labels,
            const cv::Mat &image);
    
    /** \brief Compute Sum-of-Squared Error XY.
     * \param[in] labels superpixel labels as int image
     * \param[in] image image corresponding to the superpixel labels
     * \return sum-of-squared error on XY
     */
    static float computeSumOfSquaredErrorXY(const cv::Mat &labels,
            const cv::Mat &image);
    
    /** \brief Compute Mean Distance to Edge:
     * 
     *  \f$MDE(S, G) = \frac{1}{|B|} \sum_{b \in B} D(b)\f$
     * 
     * where B is the set of boundary pixels in the ground truth segmentation
     * G and D is a distance transform image of the superpixel 
     * segmentation S. 
     * 
     * \param[in] labels superpixel labels as int image
     * \param[in] gt ground truth segmentation as int image
     * \return MDE(labels, gt)
     */
    static float computeMeanDistanceToEdge(const cv::Mat &labels,
            const cv::Mat &gt);
    
    /** \brief Compute Intra-Cluster Variation, that is the average standard deviation
     * over all superpixels.
     * \param[in] labels superpixel labels as int image
     * \param[in] image image corresponding to the superpixel labels
     * \return intra-cluster variation
     */
    static float computeIntraClusterVariation(const cv::Mat &labels,
            const cv::Mat &image);
    
    /** \brief Compute Compactness as follows:
     * 
     *  \f$CO(S) = \sum_{S_j in S} \frac{|S_j|}{N} \frac{4*pi*A(S_j)}{L(S_j)*L(S_j)}\f$
     * 
     * where \f$A(S_j)\f$ is the area and \f$L(S_j)\f$ the perimeter of the superpixel \f$S_j\f$.
     * 
     * \param[in] labels superpixel labels as int image
     * \return CO(labels)
     */
    static float computeCompactness(const cv::Mat &labels);
    
    /** \brief Contour Density is given as
     * 
     *  \f$CD(S) = \frac{|C|}{N}\f$
     * 
     * where C is the set of all contours int he superpixel segmentation and
     * N the number of pixels.
     * 
     * \param[in] labels superpixel labels as int image
     * \return CD(labels)
     */
    static float computeContourDensity(const cv::Mat &labels);

    /** \brief Regularity can be computed as the fraction of boundary pixels of
     * a superpixels and the boundary pixels of the corresponding bounding
     * boxes.
     * 
     * \param[in] labels superpixel labels as int image
     * \return regularity
     */
    static float computeRegularity(const cv::Mat &labels);
    
    /** \brief Count the number of superpixels.
     * 
     * \param[in] labels superpixel labels as int image
     * \return number of superpixels
     */
    static int computeSuperpixels(const cv::Mat &labels);
    
    /** \brief Compute superpixel size statistics.
     * 
     * \param[in] labels superpixel labels as int image
     * \param[out] average_size average size of superpixels
     * \param[out] min_size minimum size of superpixels
     * \param[out] max_size maximum size of superpixels
     */
    static void computeSuperpixelSizes(const cv::Mat &labels, float &average_size, 
            int &min_size, int &max_size, float &size_variation);
    
    /** \brief Compute edge recall based on a computed edge map.
     * \param[in] labels superpixel labels as int image
     * \param[in] edges and edge map as unsigned char image
     * \param[in] d fraction of the diagonal used as tolerance
     * \return edge recall
     */
    static float computeEdgeRecall(const cv::Mat &labels, const cv::Mat &edges,
            float d = 0.0025);    
    
    /** \brief Computes the average of a metric, i.e. computes the
     * integral of the metric in the given superpixel range using the trapezoidal
     * rule.
     * \param[in] values metric values
     * \param[in] superpixels number of superpixels corresponding to the metric values
     * \param[in] min_superpixels lower interval bound for intergration
     * \param[in] max_superpixels upper interval bound for integration
     */
    static float computeAverageMetric(const std::vector<float>& values, const std::vector<float>& superpixels,
        int min_superpixels = 200, int max_superpixels = 5200);
    
private:
    /** \brief Compute bounding boxes for all superpixels as cv::Rect.
     * \param[in] labels superpixel labels as int image
     * \param[out] rectangles vector of cv::Rect for each superpixel
     */
    static void computeBoundingBoxes(const cv::Mat &labels, std::vector<cv::Rect> &rectangles);
    
    /** \brief Compute the intersection matrix for a superpixel and a groudn truth
     * segmentation. Element (i, j) contains the number of pixels in the
     * intersection of \f$G_i\f$ and \f$S_j\f$.
     * \param[in] labels superpixel labels as int
     * \param[in] gt ground truth segmentation as int
     * \param[out] intersection_matrix matrix with intersection values for each ground truth/superpixel pair
     * \param[out] superpixel_sizes size of each superpixel
     * \param[out] gt_sizes sizes of each ground truth segment
     */
    static void computeIntersectionMatrix(const cv::Mat &labels, const cv::Mat &gt,
            cv::Mat &intersection_matrix, std::vector<int> &superpixel_sizes, std::vector<int> &gt_sizes);
    
    /** \brief Is a boundary pixel in the 4-connected sense.
     * \param[in] labels superpixel labels as int image
     * \param[in] i i coordinate
     * \param[in] j j cordinate
     * \return whether it is a boundary pixel
     */
    static bool is4ConnectedBoundaryPixel(const cv::Mat &labels, int i, int j);
    
    /** \brief Is a boundary pixel in the 8-connected sense but not in the 4-connected sense.
     * \param[in] labels superpixel labels as int image
     * \param[in] i i coordinate
     * \param[in] j j cordinate
     * \return whether it is a boundary pixel in the 8-connected sense without considering the 4-connected boundary pixels
     */
    static bool is8Minus4ConnectedBoundaryPixel(const cv::Mat &labels, int i, int j);

};

#endif	/* EVALUATION_H */