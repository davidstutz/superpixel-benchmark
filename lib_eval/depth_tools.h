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

#ifndef DEPTH_TOOLS_H
#define	DEPTH_TOOLS_H

#include <opencv2/opencv.hpp>

/** \brief Tools for using depth information.
 * \author David Stutz
 */
class DepthTools {
public:
    /**\brief Represents basic intrinsic parameters used to project pixels into 
     * a point cloud.
     */
    struct Camera {

        /** \brief Get the z coordinate of the projected point given the depth as
         * depth/factor.
         * \param[in] depth depth as unsigned short
         * \param[in] factor factor to divide depth by
         * \return z coordinate
         */
        float projectZ(unsigned short depth, float factor = 1000.f) const;

        /** \brief Given the depth by depth/factor, project the x coordinate into
         * 3D room.
         * \param[in] x x coordinate
         * \param[in] depth depth as unsigned short
         * \param[in] factor factor to divide depth by
         * \return x coordinate
         */
        template <typename T>
        float projectX(T x, unsigned short depth, float factor = 1000.f) const;

        /** \brief Given the depth by depth/factor, project the y coordinate into
         * 3D room.
         * \param[in] y y coordinate
         * \param[in] depth depth as unsigned short
         * \param[in] factor factor to divide depth by
         * \return y coordinate
         */
        template <typename T>
        float projectY(T y, unsigned short depth, float factor = 1000.f) const;

        /** \brief Given the depth by depth/factor, back project the x coordinate to
         * image plain.
         * \param[in] x x coordinate
         * \param[in] depth depth coordinate
         * \param[in] factor factor to multiply depth by
         * \return x coordinate
         */
        int backprojectX(float x, float depth, float factor = 1000.f) const;

        /** \brief Given the depth by depth/factor, back project the y coordinate to
         * image plain.
         * \param[in] y y coordinate
         * \param[in] depth depth coordinate
         * \param[in] factor factor to multiply depth with
         * \return y coordinate
         */
        int backprojectY(float y, float depth, float factor = 1000.f) const;

        /** \brief If the image was cropped in x dimension by a specific number of pixels. */
        float cropping_x;
        /** \brief If the images was cropped in y dimension by a specific number of pixels. */
        float cropping_y;
        /** \brief Principal point x coordinate. */
        float principal_x;
        /** \brief Principal point y coordinate. */
        float principal_y;
        /** \brief Focal length in x. */
        float focal_x;
        /** \brief Focal length in y. */
        float focal_y;
    };
    
    /** \brief Compute a point cloud stored as three channel float image from the given
     * depth map and the intrinsic parameters.
     * \param[in] depth depth as unsigned short image
     * \param[in] camera camera object allowing to project pixels, see above
     * \param[out] cloud point cloud as three-channel image
     */
    static void computeCloudFromDepth(const cv::Mat &depth, const Camera &camera,
            cv::Mat &cloud);
};

#endif	/* DEPTH_TOOLS_H */

