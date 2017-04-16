# Submission

We encourage authors to submit new or adapted superpixel algorithms to keep the
benchmark alive. To submit an implementation, follow these steps:

* [Preparing the Datasets](#datasets)
* [Preparing the Implementation](#implementation)
* [Performing Parameter Optimization](#parameter-optimization)
* [Performing Evaluation](#evaluation)
* [Submit Implementation and Results](#submit-implementation)

A full example:

* [Example](EXAMPLE.md)

## Datasets

**We are currently working on publishing the converted datasets in the data repository, 
[davidstutz/superpixel-benchmark-data](https://github.com/davidstutz/superpixel-benchmark-data) 
- BSDS500 is still missing. **

For NYUV2, SBD, SUNRGBD and Fash, the following steps are sufficient to get started:

* Clone [davidstutz/superpixel-benchmark-data](https://github.com/davidstutz/superpixel-benchmark-data);
* Extract the datasets into `superpixel-benchmark/data/`;
* Check the directory structure - ideally, the NYUV2 dataset should be found in
`superpixel-benchmark/data/NYUV2` with subdirectories `images`, `csv_groundTruth`
and `depth` (eahc with `test` and `train` subdirectories).

For BSDS500 which is not yet included in [davidstutz/superpixel-benchmark-data](https://github.com/davidstutz/superpixel-benchmark-data),
follow the instructions in [Datasets](DATASETS.md). This requires a working 
installation of MatLab.

Similarly, for Fash which is also not included in [davidstutz/superpixel-benchmark-data](https://github.com/davidstutz/superpixel-benchmark-data),
follow the instructions in [Datasets](DATASETS.md).

Algorithms can comfortably evaluated on individual datasets, not all datasets
need to be downloaded or extracted.

## Implementation

**The bare minimum:**
Evaluation itself only requires the superpixel segmentations as CSV files in a directory
where the names correspond to the names of the images. For example, see `eval_summary_cli`
in [Executables](EXECUTABLES.md).

For fairness, the generated superpixel segmentations are expected to be
post-processed using the provided connected components algorithms, i.e.
`SuperpixelTools::relabelConnectedSuperpixels` in C++ (see `lib_eval`) or
`sp_fast_connected_relabel` in MatLab (see `lib_tools`).

If the algorithm is based on initial markers or a grid-like initialization,
`SuperpixelTools::computeRegionSizeFromSuperpixels` or 
`SuperpixelTools::computeHeightWidthFromSuperpixels` in C++ (see `lib_eval`) and
`sp_region_size_superpixels` or `sp_height_width_superpixels` in MatLab (see `lib_tools`)
should be used. 

**Recommended:**
In addition to the above constraints, all implementations in the benchmark 
provide an easy-to-use command line tool.
It is highly recommended to provide a similar command line tool with at least 
the following two command line options:

* `-h`: display all available options with descriptions;
* `-i`: accepts a directory containing multiple PNG, JPG or JPEG images;
* `-o`: accepts a directory which is created and used to store the
superpixel segmentations for each image in CSV format - the CSV files should
be named according to the images and contain the superpixel labels as integers.
* `-v`: accepts a directory which is created and used to store visualizations
of the generated superpixel segmentations, e.g. using `lib_eval/visualization.h`;
* `-w`: verbose mode detailing the number of superpixels generated for each image
found in the directory provided by `-i`.

Examples of simple C++ command line tools:

* `reseeds_cli`
* `w_cli`
* `slic_cli`
* `cw_cli`
* ...

Examples of non-C++ command line tools:

* `wp_cli` (Python)
* `pf_cli` (Java)
* `rw_cli` (MatLab)
* ...

Examples for converting superpixel boundaries to superpixel segmentations:

* `w_cli`
* `tp_cli`

## Parameter Optimization

Parameters should be optimized or chosen on the validation sets. 
Depending on the algorithm/implementation to be submitted, there are two cases:

* Good parameters are already known (e.g. from theory, from similar experiments
or by construction);
* or the parameters need to be optimized, i.e. found.

In the first case, this step can be skipped. Next step: [Performing Evaluation](#evaluation).

For the second case, `eval_parameter_optimization_cli` might be used to perform
parameter optimization. Details can be found in `eval_parameter_optimization_cli/main.cpp`
or in the full examples: [Example](EXAMPLE.md).

## Evaluation

Evaluation consists of two steps. In the first step, `eval_summary_cli` is used
to evaluate the generated superpixel segmentations against the ground truth.
The result is a evaluation summary consisting of several metrics and statistics.
Details on `eval_summary_cli` can also be found in [Executables](EXECUTABLES.md).
To match the experiments presented in the paper, evaluation has to be run for
the following numbers of superpixels:

    200, 300, 400, 600, 800, 1000, 1200, 1400, 1600, 1800, 2000, 2400, 2800, 3200, 3600, 4000, 4600, 5200

Using the `--append-file` option, the results can be gathered in a single CSV file.
This file is required for the next step.

Given this single CSV file, `eval_average_cli` is used to compute the average statistics
for Boundary Recall, Undersegmentation Error and Explained Variation. Note that
in the first draft, these metrics were called Average Boundary Recall, Average Undersegmentation
Error and Average Explained Variation. Note that in the latest version, these
metrics are called Average Miss Rate, Average Undersegmentation Error and
Average Unexplained Variation. Also see [Executables](EXECUTABLES.md) for details.
Note that `eval_average_cli` assumes results to be available in the interval
[200, 5200] as discussed above.

Examples for both `eval_summary_cli` and `eval_average_cli`:

* `evaluate_w.sh`
* `compare_fh_refh.sh`
* `compare_seeds_reseeds.sh`

## Submit Implementation

In the spirit of reproducible and fair research, a submission 
consists of the source code, the command line tool, a bash script used for evaluation
as well as the results of the evaluation in form of the summary (as CSV file) and
the average metrics (as CSV file). The source code and the command line tool should be
accompanied with building instructions if applicable and the evaluation results
provided should be reproducible when running the evaluation script.

As also pointed out above, the following aspects
are critical for a fair comparison:

* Parameters where optimized on the validation sets;
* the computed superpixel segmentation are post-processed using one of the
provided connected components algorithm;
* to choose a grid-like initialization (if applicable) the provided
schemes are to be used;
* and evaluation is performed for the number of superpixels as discussed above
in the range [200, 5200].

Please provide the source code and scripts, e.g. zipped, by mail. As we encourage
authors to also make their algorithms publicly available, the source code
and results can also be made available through a public repository (e.g. GitHub, BitBucket etc.).
The easiest way will be to work with a fork of this repository, i.e. davidstutz/superpixel-benchmark.
For up-to-date contact information see 
[davidstutz.de/projects/superpixel-benchmark](http://davidstutz.de/projects/superpixel-benchmark/)
or the repository's root `README.md`.