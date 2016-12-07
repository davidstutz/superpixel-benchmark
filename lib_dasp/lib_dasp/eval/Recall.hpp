/*
 * Recall.hpp
 *
 *  Created on: Mar 26, 2012
 *      Author: david
 */

#ifndef DASP_RECALL_HPP_
#define DASP_RECALL_HPP_

#include <Slimage/Slimage.hpp>

namespace dasp
{

struct PrecisionRecallStats
{
	PrecisionRecallStats()
	: num_retrieved(0), num_retrieved_and_relevant(0),
	  num_relevant(0), num_relevant_and_retrieved(0) {}

	/** Total number of retrieved instances */
	unsigned int num_retrieved;

	/** Number of retrieved instances which are also relevant */
	unsigned int num_retrieved_and_relevant;

	/** Total number of relevant instances */
	unsigned int num_relevant;

	/** Number of relevant instances which are also retrieved  */
	unsigned int num_relevant_and_retrieved;

	/** Number of retrieved items that are not relevant */
	unsigned int numFalsePositives() const {
		return num_retrieved - num_retrieved_and_relevant;
	}

	/** Number of relevant items not retrieved */
	unsigned int numFalseNegatives() const {
		return num_relevant - num_relevant_and_retrieved;
	}

	/** fraction of retrieved instances that are relevant */
	float precision() const {
		return num_retrieved == 0 ? 1 : static_cast<float>(num_retrieved_and_relevant) / static_cast<float>(num_retrieved);
	}

	/** fraction of relevant instances that are retrieved */
	float recall() const {
		return num_relevant == 0 ? 1 : static_cast<float>(num_relevant_and_retrieved) / static_cast<float>(num_relevant);
	}

	/** F1-score is the geometric mean of precision and recall */
	float f1score() const {
		float p = precision();
		float r = recall();
		return 2.0f * p * r / (p + r);
	}

};

PrecisionRecallStats PrecisionRecall(const slimage::Image1ub& img_relevant, const slimage::Image1ub& img_retrievend, int d);

float ComputeRecallBox(const slimage::Image1ub& img_exp, const slimage::Image1ub& img_act, int d);

float ComputeRecallGaussian(const slimage::Image1ub& img_exp, const slimage::Image1ub& img_act, float sigma);

slimage::Image3ub CreateRecallImage(const slimage::Image1ub& img_relevant, const slimage::Image1ub& img_retrieved, int d);

}

#endif
