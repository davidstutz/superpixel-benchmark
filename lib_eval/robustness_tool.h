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

#ifndef ROBUSTNESS_TOOL_H
#define	ROBUSTNESS_TOOL_H

#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>

/** \brief Driver for different filters/enhancements/transformations.
 * \author David Stutz
 */
class RobustnessToolDriver {
public:
    /** \brief Apply the transformation with the current parameters.
     * \param[in] image input image to apply transformation on
     * \param[out] computed_image transformed image
     */
    virtual void computeImage(const cv::Mat &image, cv::Mat &computed_image) = 0;
    
    /** \brief Apply the transformation with the current parameters on the ground truth.
     * \param[in] segmentation segmentation to transform
     * \param[out] computed_segmentation transformed segmentation
     */
    virtual void computeSegmentation(const cv::Mat &segmentation, 
            cv::Mat &computed_segmentation) = 0;
    
    /** \brief Select next parameter set.
     */
    virtual bool next() = 0;
    
    /** \brief Identify current parameter settings.
     * \return identifier for current parameter setting
     */
    virtual std::string identify() = 0;
    
};

/** \brief Tool evaluating the robustness for different filters/enhancement/transformations.
 * \author David Stutz
 */
class RobustnessTool {
public:
    
    /** \brief Constructor.
     * \param[in] base_directory base directory to temporarily save transformed images in
     * \param[in] image_directory directory containing the images
     * \param[in] gt_directory directory containing hte ground truth segmentations
     * \param[in] command_line command of algorithm to run, including the default parameters
     * \param[in] driver driver to use, implicitly defining the transformation to apply
     */
    RobustnessTool(boost::filesystem::path &base_directory, boost::filesystem::path &image_directory, 
            boost::filesystem::path &gt_directory, std::string command_line, RobustnessToolDriver* driver);
    
    /** \brief Set files to keep.
     * \param[in] files names of files to keep the transformed images and segmentations for
     */
    void setFilesToKeep(const std::vector<std::string> &files);
    
    /** \brief Evaluate.
     */
    void evaluate();
    
private:
    
    /** \brief Clean the given directory.
     * \param[in] directory directory to clean up
     */
    void cleanDirectory(boost::filesystem::path directory);
    
    /** \brief Base directory to evaluate in. */
    boost::filesystem::path base_directory;
    /** \brief Directory containing images. */
    boost::filesystem::path image_directory;
    /** \brief Directory containing ground truths. */
    boost::filesystem::path gt_directory;
    
    /** \brief Command line to use for algorithm. */
    std::string command_line;
    
    /** \brief Driver to use to transform images and transformations. */
    RobustnessToolDriver* driver;    
    
    /** \brief Names of the files to keep. */
    std::vector<std::string> files;
    
};

/** \brief Additive Gaussian noise and Gaussian sampling error driver.
 */
class GaussianNoiseDriver : public RobustnessToolDriver {
public:
    
    /** \brief Constructor.
     * @param type noise or sampling error
     * @param variances variances of the Gaussian distribution to try
     */
    GaussianNoiseDriver(std::string type, const std::vector<float> &variances);
    
    /** \brief Apply the transformation with the current parameters.
     * \param[in] image input image to apply transformation on
     * \param[out] computed_image transformed image
     */
    void computeImage(const cv::Mat &image, cv::Mat &computed_image);
    
    /** \brief Apply the transformation with the current parameters on the ground truth.
     * \param[in] segmentation segmentation to transform
     * \param[out] computed_segmentation transformed segmentation
     */
    void computeSegmentation(const cv::Mat &segmentation, 
            cv::Mat &computed_segmentation);
    
    /** \brief Select next parameter set.
     */
    bool next();
    
    /** \brief Identify current parameter settings.
     * \return identifier for current parameter setting
     */
    std::string identify();
    
private:
    
    /** \brief Additive or sampling. */
    std::string type;
    
    /** \brief Variances of the Gaussian to evaluate. */
    std::vector<float> variances;
    
    /** \brief Index of current variance. */
    int current;
};

/** \brief Poisson noise driver.
 */
class PoissonNoiseDriver : public RobustnessToolDriver {
public:
    
    /** \brief Constructor.
     */
    PoissonNoiseDriver();
    
    /** \brief Apply the transformation with the current parameters.
     * \param[in] image input image to apply transformation on
     * \param[out] computed_image transformed image
     */
    void computeImage(const cv::Mat &image, cv::Mat &computed_image);
    
    /** \brief Apply the transformation with the current parameters on the ground truth.
     * \param[in] segmentation segmentation to transform
     * \param[out] computed_segmentation transformed segmentation
     */
    void computeSegmentation(const cv::Mat &segmentation, 
            cv::Mat &computed_segmentation);
    
    /** \brief Select next parameter set.
     */
    bool next();
    
    /** \brief Identify current parameter settings.
     * \return identifier for current parameter setting
     */
    std::string identify();
    
};

/** \brief Salt and pepper noise driver. 
 */
class SaltAndPepperNoiseDriver : public RobustnessToolDriver {
public:
    
    /** \brief Constructor.
     * \param[in] probabilities probabilities of salt or pepper to evaluate
     */
    SaltAndPepperNoiseDriver(const std::vector<float> &probabilities);
    
    /** \brief Apply the transformation with the current parameters.
     * \param[in] image input image to apply transformation on
     * \param[out] computed_image transformed image
     */
    void computeImage(const cv::Mat &image, cv::Mat &computed_image);
    
    /** \brief Apply the transformation with the current parameters on the ground truth.
     * \param[in] segmentation segmentation to transform
     * \param[out] computed_segmentation transformed segmentation
     */
    void computeSegmentation(const cv::Mat &segmentation, 
            cv::Mat &computed_segmentation);
    
    /** \brief Select next parameter set.
     */
    bool next();
    
    /** \brief Identify current parameter settings.
     * \return identifier for current parameter setting
     */
    std::string identify();
    
private:
    
    /** \brief Probabilities for salt and pepper to evaluate. */
    std::vector<float> probabilities;
    
    /** \brief Current probability index. */
    int current;
    
};

/** \brief Blur driver.
 */
class BlurDriver : public RobustnessToolDriver {
public:
    
    /** \brief Constructor.
     * \param[in] sizes sizes of blur filter to evaluate
     */
    BlurDriver(const std::vector<int> &sizes);
    
    /** \brief Apply the transformation with the current parameters.
     * \param[in] image input image to apply transformation on
     * \param[out] computed_image transformed image
     */
    void computeImage(const cv::Mat &image, cv::Mat &computed_image);
    
    /** \brief Apply the transformation with the current parameters on the ground truth.
     * \param[in] segmentation segmentation to transform
     * \param[out] computed_segmentation transformed segmentation
     */
    void computeSegmentation(const cv::Mat &segmentation, 
            cv::Mat &computed_segmentation);
    
    /** \brief Select next parameter set.
     */
    bool next();
    
    /** \brief Identify current parameter settings.
     * \return identifier for current parameter setting
     */
    std::string identify();
    
private:
    
    /** \brief Sizes to evaluate. */
    std::vector<int> sizes;
    
    /** \brief Current size. */
    int current;
    
};

/** \brief Gaussian blur filter.
 */
class GaussianBlurDriver : public RobustnessToolDriver {
public:
    
    /** \brief Constructor.
     * \param[in] sizes sizes of filter to evaluate
     * \param[in] variances corresponding Gussian variances to evaluate
     */
    GaussianBlurDriver(const std::vector<int> &sizes, const std::vector<float> &variances);
    
    /** \brief Apply the transformation with the current parameters.
     * \param[in] image input image to apply transformation on
     * \param[out] computed_image transformed image
     */
    void computeImage(const cv::Mat &image, cv::Mat &computed_image);
    
    /** \brief Apply the transformation with the current parameters on the ground truth.
     * \param[in] segmentation segmentation to transform
     * \param[out] computed_segmentation transformed segmentation
     */
    void computeSegmentation(const cv::Mat &segmentation, 
            cv::Mat &computed_segmentation);
    
    /** \brief Select next parameter set.
     */
    bool next();
    
    /** \brief Identify current parameter settings.
     * \return identifier for current parameter setting
     */
    std::string identify();
    
private:
    
    /** \brief Sizes to evaluate. */
    std::vector<int> sizes;
    
    /** \brief Variances of Gaussian toe valuate. */
    std::vector<float> variances;
    
    /** \brief Current size and variance. */
    int current;
    
};

/** \brief Median blur driver.
 */
class MedianBlurDriver : public RobustnessToolDriver {
public:
    
    /** \brief Median blur driver.
     * \param[in] sizes sizes of median blur filter to evaluate
     */
    MedianBlurDriver(const std::vector<int> &sizes);
    
    /** \brief Apply the transformation with the current parameters.
     * \param[in] image input image to apply transformation on
     * \param[out] computed_image transformed image
     */
    void computeImage(const cv::Mat &image, cv::Mat &computed_image);
    
    /** \brief Apply the transformation with the current parameters on the ground truth.
     * \param[in] segmentation segmentation to transform
     * \param[out] computed_segmentation transformed segmentation
     */
    void computeSegmentation(const cv::Mat &segmentation, 
            cv::Mat &computed_segmentation);
    
    /** \brief Select next parameter set.
     */
    bool next();
    
    /** \brief Identify current parameter settings.
     * \return identifier for current parameter setting
     */
    std::string identify();
    
private:
    
    /** \brief Sizes to evaluate. */
    std::vector<int> sizes;
    
    /** \brief Current size. */
    int current;
};

/** \brief Bilateral filter driver.
 */
class BilateralFilterDriver : public RobustnessToolDriver {
public:
    
    /** \brief Constructor.
     * \param[in] sizes sizes of filter to evaluate
     * \param[in] color_variances variances for color Gaussian to evaluate
     * \param[in] space_variances variances for space Gaussian to evaluate
     */
    BilateralFilterDriver(const std::vector<int> &sizes, const std::vector<float> &color_variances, 
            const std::vector<float> &space_variances);
    
    /** \brief Apply the transformation with the current parameters.
     * \param[in] image input image to apply transformation on
     * \param[out] computed_image transformed image
     */
    void computeImage(const cv::Mat &image, cv::Mat &computed_image);
    
    /** \brief Apply the transformation with the current parameters on the ground truth.
     * \param[in] segmentation segmentation to transform
     * \param[out] computed_segmentation transformed segmentation
     */
    void computeSegmentation(const cv::Mat &segmentation, 
            cv::Mat &computed_segmentation);
    
    /** \brief Select next parameter set.
     */
    bool next();
    
    /** \brief Identify current parameter settings.
     * \return identifier for current parameter setting
     */
    std::string identify();
    
private:
    
    /** \brief Sizes to evaluate. */
    std::vector<int> sizes;
    
    /** \brief Variances for color Gaussian to evaluate. */
    std::vector<float> color_variances;
    
    /** \brief Variances for space Gaussian to evaluate. */
    std::vector<float> space_variances;
    
    /** \brief Current size and variances. */
    int current;
    
};

/** \brief Motion blur driver.
 */
class MotionBlurDriver : public RobustnessToolDriver {
public:
    
    /** \brief Constructor.
     * \param[in] sizes sizes of filter to evaluate
     * \param[in] angles angles of motion blur to evaluate
     */
    MotionBlurDriver(const std::vector<int> &sizes, const std::vector<float> &angles);
    
    /** \brief Apply the transformation with the current parameters.
     * \param[in] image input image to apply transformation on
     * \param[out] computed_image transformed image
     */
    void computeImage(const cv::Mat &image, cv::Mat &computed_image);
    
    /** \brief Apply the transformation with the current parameters on the ground truth.
     * \param[in] segmentation segmentation to transform
     * \param[out] computed_segmentation transformed segmentation
     */
    void computeSegmentation(const cv::Mat &segmentation, 
            cv::Mat &computed_segmentation);
    
    /** \brief Select next parameter set.
     */
    bool next();
    
    /** \brief Identify current parameter settings.
     * \return identifier for current parameter setting
     */
    std::string identify();
    
private:
    
    /** \brief Sizes to evaluate. */
    std::vector<int> sizes;
    
    /** \brief Angles to evaluate. */
    std::vector<float> angles;
    
    /** \brief Current size and angle. */
    int current;
    
};

/** \brief Vertical and horizontal shear driver.
 */
class ShearDriver : public RobustnessToolDriver {
public:
    
    /** \brief Constructor.
     * \param[in] type horizontal or vertical
     * \apram[in] crop crop the image
     * \param[in] m strenght of shear
     */
    ShearDriver(std::string type, int crop, const std::vector<float> &m);
    
    /** \brief Apply the transformation with the current parameters.
     * \param[in] image input image to apply transformation on
     * \param[out] computed_image transformed image
     */
    void computeImage(const cv::Mat &image, cv::Mat &computed_image);
    
    /** \brief Apply the transformation with the current parameters on the ground truth.
     * \param[in] segmentation segmentation to transform
     * \param[out] computed_segmentation transformed segmentation
     */
    void computeSegmentation(const cv::Mat &segmentation, 
            cv::Mat &computed_segmentation);
    
    /** \brief Select next parameter set.
     */
    bool next();
    
    /** \brief Identify current parameter settings.
     * \return identifier for current parameter setting
     */
    std::string identify();
    
private:
    
    /** \brief Type to use, horizontal or vertical. */
    std::string type;
    
    /** \brief Crop to use. */
    int crop;
    
    /** \brief Shear coefficients to evaluate. */
    std::vector<float> m;
    
    /** \brief Current shear coefficient. */
    int current;
    
};

/** \brief Rotation driver.
 */
class RotationDriver : public RobustnessToolDriver {
public:
    
    /** \brief Constructor.
     * \param[in] crop crop to use on image
     * \param[in] angles angles to evaluate rotation for
     */
    RotationDriver(int crop, const std::vector<float> &angles);
    
    /** \brief Apply the transformation with the current parameters.
     * \param[in] image input image to apply transformation on
     * \param[out] computed_image transformed image
     */
    void computeImage(const cv::Mat &image, cv::Mat &computed_image);
    
    /** \brief Apply the transformation with the current parameters on the ground truth.
     * \param[in] segmentation segmentation to transform
     * \param[out] computed_segmentation transformed segmentation
     */
    void computeSegmentation(const cv::Mat &segmentation, 
            cv::Mat &computed_segmentation);
    
    /** \brief Select next parameter set.
     */
    bool next();
    
    /** \brief Identify current parameter settings.
     * \return identifier for current parameter setting
     */
    std::string identify();
    
private:
    
    /** \brief Crop to use. */
    int crop;
    
    /** \brief Angles to evaluate. */
    std::vector<float> angles;
    
    /** \brief Current angle. */
    int current;
    
};

/** \brief Translation driver.
 */
class TranslationDriver : public RobustnessToolDriver {
public:
    
    /** \brief Constructor.
     * \param[in] crop crop to use
     * \param[in] x translation along x to evaluate
     * \param[in] y translation along y to evaluate
     */
    TranslationDriver(int crop, const std::vector<int> &x, const std::vector<int> &y);
    
    /** \brief Apply the transformation with the current parameters.
     * \param[in] image input image to apply transformation on
     * \param[out] computed_image transformed image
     */
    void computeImage(const cv::Mat &image, cv::Mat &computed_image);
    
    /** \brief Apply the transformation with the current parameters on the ground truth.
     * \param[in] segmentation segmentation to transform
     * \param[out] computed_segmentation transformed segmentation
     */
    void computeSegmentation(const cv::Mat &segmentation, 
            cv::Mat &computed_segmentation);
    
    /** \brief Select next parameter set.
     */
    bool next();
    
    /** \brief Identify current parameter settings.
     * \return identifier for current parameter setting
     */
    std::string identify();
    
private:
    
    /** \brief Crop to use. */
    int crop;
    
    /** \brief Translations along x to evaluate. */
    std::vector<int> x;
    
    /** \brief Translations along y to evaluate. */
    std::vector<int> y;
    
    /** \brief Current translation. */
    int current;
    
};

#endif	/* ROBUSTNESS_TOOL_H */

