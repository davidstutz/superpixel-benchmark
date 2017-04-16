# Example

The following will be a complete example of how to help us integrate a new
superpixel algorithms into the benchmark. The following steps are discussed:

* [Preparing the BSDS500 Dataset](#dataset)
* [Preparing the Implementation](#implementation)
* [Performing Parameter Optimization](#parameter-optimization)
* [Performing Evaluation](#evaluation)
* [Submit Implementation and Results](#submit-implementation)

As example, we will use the watershed algorithm, W, as it is available through
OpenCV. Note that this assumes that the algorithm is implemented in C/C++.

The source code of the example can be found in `examples/submission`.

Similar examples for other programming languages are:

* `wp_cli` (Python)
* `pf_cli` (Java)
* `rw_cli` (MatLab)
* ...

**Prerequisites:**

The examples assumes the benchmark to be installed in the home directory, e.g.
through

    $ git clone https://github.com/davidstutz/superpixel-benchmark --recursive
    $ cd superpixel-benchmark

We assume the benchmark to be built, see [Building](BUILDING.md) for instructions.
At least `lib_eval` needs to be built. Adapt `CMakeLists.txt` if it is not desired
to built any other algorithms.

Datasets are assumed to be put into `~/superpixel-benchmark/data`; for the BSDS500
dataset approximately 1GB of space is required.

## Dataset

As example, we will use the BSDS500 dataset which needs to be converted manually
as it is currently not available in the [data repository](https://github.com/davidstutz/superpixel-benchmark-data).

To convert the BSDS500 dataset, we follow `lib_tools/bsds500_convert_script.m`:

* Download the dataset from [here](https://www2.eecs.berkeley.edu/Research/Projects/CS/vision/grouping/resources.html).
* Extract the contents of `BSR/BSDS500/data` into `~/superpixel-benchmark/data/BSDS500`
such that the subdirectories are `groundTruth` and `images`.
* In `lib_tools/bsds500_convert_script.m` set `BSDS500_DIR` to the full (i.e. absolute)
path to `~/superpixel-benchmark/data/BSDS500/` with trailing `/`.
* Open MatLab, navigate to `~/superpixel-benchmark/lib_tools/` and run the script.
This may take some time.

After following the above instructions, `~/superpixel-benchmark/data/BSDS500/` will
additionally contain `csv_groundTruth`.

## Implementation

In our example, W is implemented as part of OpenCV. Thus, only an appropriate command
line tool is required. In order to also demonstrate parameter optimization, an 
extra parameter is added by smoothing the image with a Gaussian kernel before
running W. The following listing shows the command line tool `w_cli`
reduced to the essentials:

    #include <fstream>
    #include <opencv2/opencv.hpp>
    #include <boost/filesystem.hpp>
    #include <boost/program_options.hpp>
    #include <boost/timer.hpp>
    #include <bitset>
    #include "io_util.h"
    #include "superpixel_tools.h"
    #include "visualization.h"

    /** \brief Command line tool for running an adapted W as example used in 
     * docs/EXAMPLE.md to show how to submit a new algorithm to the benchmark.
     * Usage:
     * \code{sh}
     *   $ ../bin/example_command_line_tool --help
     * Allowed options:
     *   -h [ --help ]                   produce help message
     *   -i [ --input ] arg              the folder to process (can also be passed as 
     *                                   positional argument)
     *   -s [ --superpixels ] arg (=400) number of superpixles
     *   -g [ --sigma ] arg (=0)         sigma used for smoothing (no smoothing if 
     *                                   zero)
     *   -o [ --csv ] arg                specify the output directory (default is 
     *                                   ./output)
     *   -v [ --vis ] arg                visualize contours
     *   -x [ --prefix ] arg             output file prefix
     *   -w [ --wordy ]                  verbose/wordy/debug
     * \endcode
     * \author David Stutz
     */
    int main(int argc, const char** argv) {

        boost::program_options::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "produce help message")
            ("input,i", boost::program_options::value<std::string>(), "the folder to process (can also be passed as positional argument)")
            ("superpixels,s", boost::program_options::value<int>()->default_value(400), "number of superpixles")
            ("sigma,g", boost::program_options::value<float>()->default_value(0.0f), "sigma used for smoothing (no smoothing if zero)")
            ("csv,o", boost::program_options::value<std::string>()->default_value(""), "specify the output directory (default is ./output)")
            ("vis,v", boost::program_options::value<std::string>()->default_value(""), "visualize contours")
            ("prefix,x", boost::program_options::value<std::string>()->default_value(""), "output file prefix")
            ("wordy,w", "verbose/wordy/debug");

        boost::program_options::positional_options_description positionals;
        positionals.add("input", 1);

        boost::program_options::variables_map parameters;
        boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(positionals).run(), parameters);
        boost::program_options::notify(parameters);

        if (parameters.find("help") != parameters.end()) {
            std::cout << desc << std::endl;
            return 1;
        }

        boost::filesystem::path output_dir(parameters["csv"].as<std::string>());
        if (!output_dir.empty()) {
            if (!boost::filesystem::is_directory(output_dir)) {
                boost::filesystem::create_directories(output_dir);
            }
        }

        boost::filesystem::path vis_dir(parameters["vis"].as<std::string>());
        if (!vis_dir.empty()) {
            if (!boost::filesystem::is_directory(vis_dir)) {
                boost::filesystem::create_directories(vis_dir);
            }
        }

        boost::filesystem::path input_dir(parameters["input"].as<std::string>());
        if (!boost::filesystem::is_directory(input_dir)) {
            std::cout << "Image directory not found ..." << std::endl;
            return 1;
        }

        std::string prefix = parameters["prefix"].as<std::string>();

        bool wordy = false;
        if (parameters.find("wordy") != parameters.end()) {
            wordy = true;
        }

        int superpixels = parameters["superpixels"].as<int>();

        std::multimap<std::string, boost::filesystem::path> images;
        std::vector<std::string> extensions;
        IOUtil::getImageExtensions(extensions);
        IOUtil::readDirectory(input_dir, extensions, images);

        float total = 0;
        for (std::multimap<std::string, boost::filesystem::path>::iterator it = images.begin(); 
                it != images.end(); ++it) {

            cv::Mat image = cv::imread(it->first);

            float sigma = parameters["sigma"].as<float>();
            if (sigma > 0.01) {
                int size = std::ceil(sigma*4) + 1;
                cv::GaussianBlur(image, image, cv::Size (size, size), sigma, sigma);
            }

            int region_size = SuperpixelTools::computeRegionSizeFromSuperpixels(image, 
                    superpixels);
            cv::Mat markers(image.rows, image.cols, CV_32SC1, cv::Scalar(0));

            int label = 1;
            for (int i = region_size/2; i < image.rows; i += region_size) {
                for (int j = region_size/2; j < image.cols; j += region_size) {
                    markers.at<int>(i, j) = label;
                    label++;
                }
            }

            cv::Mat labels;

            boost::timer timer;
            cv::watershed(image, markers);
            SuperpixelTools::assignBoundariesToSuperpixels(image, markers, labels);    
            float elapsed = timer.elapsed();
            total += elapsed;

            int unconnected_components = SuperpixelTools::relabelConnectedSuperpixels(labels);

            if (wordy) {
                std::cout << SuperpixelTools::countSuperpixels(labels) << " superpixels for " << it->first 
                        << " (" << unconnected_components << " not connected; " 
                        << elapsed <<")." << std::endl;
            }

            if (!output_dir.empty()) {
                boost::filesystem::path csv_file(output_dir 
                        / boost::filesystem::path(prefix + it->second.stem().string() + ".csv"));
                IOUtil::writeMatCSV<int>(csv_file, labels);
            }

            if (!vis_dir.empty()) {
                boost::filesystem::path contours_file(vis_dir 
                        / boost::filesystem::path(prefix + it->second.stem().string() + ".png"));
                cv::Mat image_contours;
                Visualization::drawContours(image, labels, image_contours);
                cv::imwrite(contours_file.string(), image_contours);
            }
        }

        if (wordy) {
            std::cout << "Average time: " << total / images.size() << "." << std::endl;
        }

        if (!output_dir.empty()) {
            std::ofstream runtime_file(output_dir.string() + "/" + prefix + "runtime.txt", 
                    std::ofstream::out | std::ofstream::app);

            runtime_file << total / images.size() << "\n";
            runtime_file.close();
        }

        return 0;
    }

The corresponding CMake file might look like this:

    find_package(OpenCV REQUIRED)
    find_package(Boost COMPONENTS system filesystem program_options REQUIRED)

    include_directories(../lib_eval/
        ${OpenCV_INCLUDE_DIRS} 
        ${Boost_INCLUDE_DIRS}
    )
    add_executable(w_cli main.cpp)
    target_link_libraries(w_cli
        eval
        ${Boost_LIBRARIES}
        ${OpenCV_LIBS}
    )

The CMake file can be found in `w_cli`; then `CMakeLists.txt` in the root
of the repository can be adapted to include

    add_subdirectory(w_cli)

after the `lib_eval` directory was added. After building, the command line tool
can be called as follows:

    $ ../bin/example_command_line_tool --help
    Allowed options:
      -h [ --help ]                   produce help message
      -i [ --input ] arg              the folder to process (can also be passed as 
                                      positional argument)
      -s [ --superpixels ] arg (=400) number of superpixles
      -g [ --sigma ] arg (=0)         sigma used for smoothing (no smoothing if 
                                      zero)
      -o [ --csv ] arg                specify the output directory (default is 
                                      ./output)
      -v [ --vis ] arg                visualize contours
      -x [ --prefix ] arg             output file prefix
      -w [ --wordy ]                  verbose/wordy/debug

## Parameter Optimization

The parameter optimization tools as defined in `lib_eval/parameter_optimization_tool.h`
can be used as in the following command line tool:
 
    #include <opencv2/opencv.hpp>
    #include <boost/filesystem.hpp>
    #include <boost/timer.hpp>
    #include <boost/program_options.hpp>
    #include <glog/logging.h>

    #include "io_util.h"
    #include "parameter_optimization_tool.h"

    /** \brief Example of parameter optimization for an adapted W as used in the
     * example for submitting a new algorithm to the benchmark in docs/EXAMPLE.md.
     * $ ../bin/example_parameter_optimization --help
     * \code{sh}
     * Allowed options:
     *   --img-directory arg   image directory
     *   --gt-directory arg    ground truth directory
     *   --base-directory arg  base directory
     *   --help                produce help message
     * \endcode
     * \author David Stutz
     */
    int main(int argc, const char** argv) {

        boost::program_options::options_description desc("Allowed options");
        desc.add_options()
            ("img-directory", boost::program_options::value<std::string>(), "image directory")
            ("gt-directory", boost::program_options::value<std::string>(), "ground truth directory")
            ("base-directory", boost::program_options::value<std::string>(), "base directory")
            ("help", "produce help message");

        boost::program_options::positional_options_description positionals;
        positionals.add("img-directory", 1);
        positionals.add("gt-directory", 1);
        positionals.add("base-directory", 1);

        boost::program_options::variables_map parameters;
        boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).positional(positionals).run(), parameters);
        boost::program_options::notify(parameters);

        if (parameters.find("help") != parameters.end()) {
            std::cout << desc << std::endl;
            return 1;
        }

        boost::filesystem::path img_directory(parameters["img-directory"].as<std::string>());
        if (!boost::filesystem::is_directory(img_directory)) {
            std::cout << "Image directory does not exist." << std::endl;
            return 1;
        }

        boost::filesystem::path gt_directory(parameters["gt-directory"].as<std::string>());
        if (!boost::filesystem::is_directory(gt_directory)) {
            std::cout << "Ground truth directory does not exist." << std::endl;
            return 1;
        }

        boost::filesystem::path base_directory(parameters["base-directory"].as<std::string>());
        if (!boost::filesystem::is_directory(base_directory)) {
            boost::filesystem::create_directories(base_directory);
        }

        std::vector<int> superpixels = {400, 1200, 3600};

        for (unsigned int k = 0; k < superpixels.size(); k++) {
            ParameterOptimizationTool tool(img_directory, gt_directory,
                    base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                    "./bin/example_command_line_tool", "");

            tool.addIntegerParameter("superpixels", "--superpixels", std::vector<int>{superpixels[k]});
            tool.addFloatParameter("sigma", "--sigma", std::vector<float>{0.5f, 1.f, 1.5f});
            tool.optimize();
        }

        return 0;
    }

The command line tool can be run using:

    $ ../bin/example_parameter_optimization --help
    Allowed options:
      --img-directory arg   image directory
      --gt-directory arg    ground truth directory
      --base-directory arg  base directory
      --help                produce help message

After changing the directory to the repository root (i.e. `~/superpixel-benchmark/`),
parameter optimization can be run. Note that we changed the directory as the
path to the command line tool is hard-coded in the listing above. Running parameter
optimization looks as follows:

    $ ./bin/example_parameter_optimization data/BSDS500/images/val/ data/BSDS500/csv_groundTruth/val/ experiments/
    Added integer parameter --superpixels: 400
    Added float parameter --sigma: 0.5,1,1.5
    Total: 3 combinations.
    Time remaining: 52.9239 (26.4619) ...
    Added integer parameter --superpixels: 1200
    Added float parameter --sigma: 0.5,1,1.5
    Total: 3 combinations.
    Time remaining: 43.2005 (21.6003) ...
    Added integer parameter --superpixels: 3600
    Added float parameter --sigma: 0.5,1,1.5
    Total: 3 combinations.
    Time remaining: 51.2812 (25.6406) ...

Parameter optimization will create a directory `experiments/val/` with subdirectories
`400/`, `1200/` and `3600/`. Each directory contains a summary of parameter optimization
in `parameter_optimization.csv`. In the above examples, this looks as follows:

    sp_directory,superpixels,sigma,rec_average,ue_np_average,co_average,score,co_score,sp_average
    experiments/400/1490992372,400,1,0.767879,0.105258,0.252559,0.83131,0.639688,389.86
    experiments/400/1490992398,400,1.5,0.763123,0.103071,0.262798,0.830026,0.642172,390.16
    experiments/400/1490992419,400,0.5,0.751224,0.111048,0.245077,0.820088,0.629645,388.14
    best,400,1
    co_best,400,1.5

Where the parameters corresponding to the row `best` are supposed to be taken as
they jointly optimize Boundary Recall and Undersegmentation. In this case, the best
sigma is 1.5 for 400 superpixels; for 1200 and 3600 superpixels the best sigma is 1.
So in the following we will go with sigma equals 1.

## Evaluation

Once the parameters are decided, evaluation is done using `eval_summary_cli` and
`eval_average_cli`. The following bash script does this for numbers of superpixels in
[200, 5200]:

    #!/bin/sh
    for SUPERPIXEL in "200" "300" "400" "600" "800" "1000" "1200" "1400" "1600" "1800" "2000" "2400" "2800" "3200" "3600" "4000" "4600" "5200"
    do
        ./bin/w_cli data/BSDS500/images/test/ --superpixels $SUPERPIXEL -o experiments/test/$SUPERPIXEL -w
        ./bin/eval_summary_cli experiments/test/$SUPERPIXEL data/BSDS500/images/test data/BSDS500/csv_groundTruth/test --append-file experiments/test/w.csv --vis
        find ./output/w/$SUPERPIXEL -type f -name '*[^summary|correlation|results].csv' -delete
    done

    ./bin/eval_average_cli experiments/test/w.csv -o output/w_average.csv

The script will create a directory `experiments/test` where the final results
can be found in `w.csv` and `w_average.csv`. Note that evaluation may, overall,
take several minutes.

## Submit Implementation

Checking the list in [Submission](SUBMISSION.md):

* We optimized parameters on the validation set of the BSDS500 dataset;
* the computed superpixel segmentations are post-processed using 
`SuperpixelTools::relabelConnectedSuperpixels` (after the boundaries computed
by W are converted to superpixel labels using `SuperpixelTools::computeRegionSizeFromSuperpixels`);
* the initial markers are placed on a regular grid according to `SuperpixelTools::computeRegionSizeFromSuperpixels`;
* and evaluation is performed for numbers of superpixels in [200, 5200].

A submission could now exist of all the involved files (as discussed above),
including a guide detailing building and evaluation instructions. This could
for example be put into a separate repository or a fork of this repository.