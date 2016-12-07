/*
 * Parameters.hpp
 *
 *  Created on: Apr 4, 2012
 *      Author: david
 */

#ifndef DASP_PARAMETERS_HPP_
#define DASP_PARAMETERS_HPP_

#include "Tools.hpp"
#include <ctype.h>

namespace dasp
{

	namespace SeedModes
	{
		enum Type {
			Random,
			Grid,
			SimplifiedPDS_Old,
			SimplifiedPDS,
			FloydSteinberg,
			FloydSteinbergExpo,
			FloydSteinbergMultiLayer,
			Fattal,
			Delta
		};
	}
	typedef SeedModes::Type SeedMode;

	namespace DensityModes
	{
		enum Type {
			ASP_RGB, // constant density, RGB feature, DA2-metric
			ASP_RGBD, // constant density, RGBxD feature, DA2-metric
			DASP // depth-adaptive density, RGB+Normal feature, DA3-metric
		};
	}
	typedef DensityModes::Type DensityMode;

	namespace ColorSpaces
	{
		enum Type {
			RGB, HSV, LAB, HN
		};
	}
	typedef ColorSpaces::Type ColorSpace;

	struct Parameters
	{
		Parameters();

		unsigned int random_seed;

		/** camera parameters */
		Camera camera;

		ColorSpace color_space;

		DensityMode density_mode;

		float weight_color;
		float weight_spatial;
		float weight_normal;

		/** Number of iterations for superpixel k-means clustering */
		unsigned int iterations;

		/** Superpixel cluster search radius factor */
		float coverage;

		/** Desired radius of a surface element */
		float base_radius;

		/** Desired number of superpixels */
		unsigned int count;
                
                /** For density computation */
                float width;
                float height;
                
		/** Actual count of superpixels */
		unsigned int count_actual;

		/** Method used to compute seed points */
		SeedMode seed_mode;

		bool gradient_adaptive_density;
		bool use_density_depth;

		/** Ignores pixels which are too far away or where the depth gradient is too big */
		bool ignore_pixels_with_bad_visibility;

		bool is_conquer_enclaves;

		float segment_threshold;

		bool is_repair_depth;
		bool is_smooth_depth;
		bool is_smooth_density;
		bool is_improve_seeds;

		bool enable_clipping;
		float clip_x_min, clip_x_max;
		float clip_y_min, clip_y_max;
		float clip_z_min, clip_z_max;

		bool enable_roi_2d;
		float roi_2d_x_min, roi_2d_x_max;
		float roi_2d_y_min, roi_2d_y_max;

		/** Pixel scala at depth
		 * Radius [px] of a surface element of size base radius [m] and
		 * at given depth [kinect] on the image sensor
		 */
		float computePixelScala(uint16_t depth) const {
			return (depth == 0) ? 0.0f : (camera.focal / camera.convertKinectToMeter(depth) * base_radius);
		}

	};

}

#endif
