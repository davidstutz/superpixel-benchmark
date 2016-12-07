#ifndef DASP_EVAL
#define DASP_EVAL

#include <Superpixels.hpp>
#include <Slimage/Slimage.hpp>
#include <map>
#include <vector>

namespace dasp {
namespace eval {

float UndersegmentationError(const slimage::Image1i& labels_relevant, const slimage::Image1i& labels_retrieved);

float Area(const Superpixels& u);

float Area3D(const Superpixels& u);

std::pair<float,std::vector<float>> IsoperimetricQuotient(const Superpixels& u);

std::pair<float,std::vector<float>> IsoperimetricQuotient3D(const Superpixels& u);

float ExplainedVariationColor(const Superpixels& sp);

float ExplainedVariationDepth(const Superpixels& sp);

float ExplainedVariationPosition(const Superpixels& sp);

float ExplainedVariationNormal(const Superpixels& sp);

float CompressionErrorColor(const Superpixels& sp);

float CompressionErrorDepth(const Superpixels& sp);

float CompressionErrorPosition(const Superpixels& sp);

float CompressionErrorNormal(const Superpixels& sp);

float MeanNeighbourDistance(const Superpixels& sp);

}
}

#endif
