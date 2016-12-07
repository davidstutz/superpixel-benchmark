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

#ifndef TRANSFORMATIONS_H
#define	TRANSFORMATIONS_H

#include <opencv2/opencv.hpp>

/** \brief Utility class for image and ground truth transofmrations.
 * \author David Stutz
 */
class Transformation {
public:
    /** \brief Add Gaussian additive noise to image with the given variance on all channels.
     * \param[in] image image to add Gaussian additive noise to
     * \param[in] variance variance of the Gaussian
     * \param[out] noisy_image noisy image
     */
    static void applyGaussianAdditiveNoise(const cv::Mat &image, float variance, 
            cv::Mat &noisy_image);
    
    /** \brief Add Gaussian sampling errors with given variance.
     * \param[in] image image to add Gaussian sampling errors to
     * \param[in] variance variance of the Gaussian
     * \param[out] noisy_image
     */
    static void applyGaussianSamplingErrors(const cv::Mat &image, float variance, 
            cv::Mat &noisy_image);
    
    /** \brief Add salt and pepper noise.
     * \param[in] image image to add salt and pepper noise to
     * \param[in] p probability of salt or pepper
     * \param[out] noisy_image noisy image
     */
    static void applySaltAndPepperNoise(const cv::Mat &image, float p, cv::Mat &noisy_image);
    
    /** \brief Add poisson noise.
     * \param[in] image image to add Poisson noise to
     * \param[out] noisy_image noisy image
     */
    static void applyPoissonNoise(const cv::Mat &image, cv::Mat &noisy_image);
    
    /** \brief Apply blur (box filter).
     * \param[in] image image to apply blur filter to
     * \param[in] size size of filter
     * \param[out] blurred_image blrured image
     */
    static void applyBlur(const cv::Mat &image, int size, cv::Mat &blurred_image);

    /** \brief Apply Gaussian blurring.
     * \param[in] image image to apply Gaussian blur to
     * \param[in] size size of filter
     * \param[in] sigma_x variance in x direction
     * \param[in] sigma_y variance in y direction
     * \param[out] blurred_image blurred image
     */
    static void applyGaussianBlur(const cv::Mat &image, int size, float sigma_x, 
            float sigma_y, cv::Mat &blurred_image);

    /** \brief Apply median filter.
     * \param[in] image image to apply median filter to
     * \param[in] size size of filter
     * \param[out] blurred_image blurred image
     */
    static void applyMedianBlur(const cv::Mat &image, int size, 
            cv::Mat &blurred_image);

    /** \brief Apply bilateral filter.
     * \param[in] image image to apply bilateral filter to
     * \param[in] size filter size
     * \param[in] sigma_color Gaussian variance for color 
     * \param[in] sigma_space Gaussian variance for space
     * \param[out] filtered_image filtered image
     */
    static void applyBilateralFilter(const cv::Mat &image, int size, float sigma_color, 
            float sigma_space, cv::Mat &filtered_image);
    
    /** \brief Apply motion blur.
     * \param[in] image image to apply motion blur to
     * \param[in] size size of filter
     * \param[in] angle angle of motion blur
     * \param[out] blrured_image blurred image
     */
    static void applyMotionBlur(const cv::Mat &image, int size, 
            float angle, cv::Mat &blurred_image);

    /** \brief Apply vertical shear.
     * \param[in] image image to apply shear to
     * \param[in] crop crop image before shearing
     * \param[in] shear shear parameter
     * \param[out] sheared_image sheared image
     */
    template<typename T>
    static void applyVerticalShear(const cv::Mat &image, int crop, 
            float shear, cv::Mat &sheared_image);

    /** \brief Apply horizontal shear.
     * \param[in] image image to apply shear to
     * \param[in] crop crop image before shearing
     * \param[in] shear shear parameter
     * \param[out] sheared_image sheared image
     */
    template<typename T>
    static void applyHorizontalShear(const cv::Mat &image, int crop, 
            float shear, cv::Mat &sheared_image);

    /** \brief Apply rotation.
     * \param[in] image image ot apply rotation to
     * \param[in] crop crop the image before rotating
     * \param[in] angle angle of rotation
     * \param[out] rotated_image rotated image
     */
    template<typename T>
    static void applyRotation(const cv::Mat &image, int crop, float angle, 
            cv::Mat &rotated_image, int interpolation = cv::INTER_LINEAR);

    /** \brief Apply translation.
     * \param[in] image image to translate
     * \param[in] crop crop image before translating
     * \param[in] translation_x translation in x direction
     * \param[in] translation_y translation in y direction
     * \param[out] translated_image translated image
     */
    template<typename T>
    static void applyTranslation(const cv::Mat &image, int crop, 
            int translation_x, int translation_y, cv::Mat &translated_image);
};

#endif	/* TRANSFORMATIONS_H */

