# Benchmark

The benchmark combines metrics from different references focussing on different
aspects of superpixel segmentations. Detailed equations can be found in the paper
or in the [Doxygen Documentation](https://davidstutz.github.io/superpixel-benchmark/),
specifically in the class [Evaluation](https://davidstutz.github.io/superpixel-benchmark/classEvaluation.html).

Note: Evaluation should be done using the executables discussed in [Executables](EXECUTABLES.md).
However, the metrics implemented in the [Evaluation](https://davidstutz.github.io/superpixel-benchmark/classEvaluation.html).
class can also be used directly from within C++.

## Boundary Recall

Boundary Recall is part of the precision-recall framework introduced in [2]
and quantifies the fraction of boundary pixels correctly captured by
a superpixel segmentation. Higher Boundary Recall describes better adherence to
image boundaries.

    [2] D. Martin, C. Fowlkes, J. Malik.
        Learning to detect natural image boundaries using local brightness, color, and texture cues.
        IEEE Transactions on Pattern Analysis and Machine Intelligence 26 (5) (2004) 530–549.

Note: `lib_eval/evaluation.h` also includes an implementation of Boundary Precision.

## Undersegmentation Error

Undersegmentation Error was first introduced in [3] and quantifies the leakage of
superpixels across ground truth segments. However, the original formulation penalizes
large superpixels (see [3, 4]) covering multiple ground truth segments which may be misleading.
Therefore, the formulation of [5] is used. For each superpixel and ground truth segment pair
we count the pixels in the overlap or the difference set, whichever is smaller.
These pixels constitute the leakage of superpixels across the ground truth segments
(assuming that superpixels are assigned to ground truth segments with highest overlap).

    [3] A. Levinshtein, A. Stere, K. N. Kutulakos, D. J. Fleet, S. J. Dickinson, K. Siddiqi.
        TurboPixels: Fast superpixels using geometric flows.
        IEEE Transactions on Pattern Analysis and Machine Intelligence 31 (12) (2009) 2290–2297.
    [4] R. Achanta, A. Shaji, K. Smith, A. Lucchi, P. Fua, S. Susstrunk.
        SLIC superpixels compared to state-of-the-art superpixel methods.
        IEEE Transactions on Pattern Analysis and Machine Intelligence 34 (11) (2012) 2274–2281.
    [5] P. Neubert, P. Protzel. 
        Superpixel benchmark and comparison.
        Forum Bildverarbeitung, 2012.

Note: `lib_eval/evaluation.h` includes two different implementations of Undersegmentation
Error: the definition by Neubert and Protzel and the definition by Levinshtein.

## Achievable Segmentation Accuracy

Achievable Segmentation Accuracy [6] quantifies the segmentation performance achievable
when using superpixels instead of pixels. To this end, each superpixel is assigned to the
ground truth segment with highest overlap. The number of pixels correctly classified
this way is used to compute Achievable Segmentation Accuracy.

    [6] M. Y. Lui, O. Tuzel, S. Ramalingam, R. Chellappa.
        Entropy rate superpixel segmentation.
        IEEE Conference on Computer Vision and Pattern Recognition, 2011, pp. 2097–2104.

## Explained Variation

Explained Variation [7] computes the variance within each superpixels, weights it by
the size of the superpixel and normalizes the sum by the total image variance. This
way, Explained Variation quantifies the fraction of variance within the image that
is captured (i.e. "explained") by the superpixel segmentation.

    [7] A. P. Moore, S. J. D. Prince, J. Warrell, U. Mohammed, G. Jones.
        Superpixel lattices.
        IEEE Conference on Computer Vision and Pattern Recognition, 2008, pp. 1–8.

## Compactness

Compactness [8] compares the area of each superpixel with the area of a circle with the
same perimeter.

    [8] A. Schick, M. Fischer, R. Stiefelhagen.
        Measuring and evaluating the compactness of superpixels.
        International Conference on Pattern Recognition, 2012, pp. 930–934.

Note: The implementation in `lib_eval/evaluation.h` uses a simple estimate of the
perimeter of superpixels. Therefore, results may not be comparable to those in [8].

## Intra-Cluster Variation

Intra-Cluster Variation [9] computes the average standard deviation of the superpixels.
However, this is not done in relation to the overall image variation.

    [9] W. Benesova, M. Kottman.
        Fast superpixel segmentation using morphological processing.
        Conference on Machine Vision and Machine Learning, 2014.

## Mean Distance to Edge

Mean Distance to Edge [9] averages the distance of each boundary pixel in the ground
truth segmentation to the nearest boundary pixel in the superpixel segmentation.
Assuming the distances to encode the binary relationship of true positives or false
negatives, Mean Distance to Edge resembles Boundary Recall.

    [9] W. Benesova, M. Kottman.
        Fast superpixel segmentation using morphological processing.
        Conference on Machine Vision and Machine Learning, 2014.

## Contour Density

Contour Density [10] quantifies the fraction of pixels that are boundary pixels
in the superpixel segmentation

    [10] V. Machairas, M. Faessel, D. Cardenas-Pena, T. Chabardes, T. Walter, E. Decenciere.
        Waterpixels.
        Transactions on Image Processing 24 (11) (2015) 3707–3716.

## Average Metrics

In order to render Boundary Recall, Undersegmentation Error and Explained Variation independent
of the number of generated superpixels, the area above (or below in the case of
Undersegmentation Error) the corresponding curves is used to summarize performance.