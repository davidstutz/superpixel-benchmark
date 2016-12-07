/*
 * RepairDepth.hpp
 *
 *  Created on: Mar 27, 2012
 *      Author: david
 */

#ifndef DASP_REPAIRDEPTH_HPP_
#define DASP_REPAIRDEPTH_HPP_

#include <Slimage/Slimage.hpp>

namespace dasp
{

	void RepairDepth(const slimage::Image1ui16& depth, const slimage::Image3ub& color);

	void SmoothDepth(const slimage::Image1ui16& depth, const slimage::Image3ub& color);

}

#endif
