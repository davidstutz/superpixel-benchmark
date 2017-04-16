#include <fstream>
#include <opencv2/opencv.hpp>
#include <boost/program_options.hpp>
#include <boost/timer.hpp>
#include "io_util.h"
#include "superpixel_tools.h"
#include "visualization.h"
#include "vlslic_opencv.h"

/** \brief Command line tool for running vlSLIC.
 * Usage:
 * \code{sh}
 *   $ ../bin/vlslic_cli --help
 *   Allowed options:
 *     -h [ --help ]                         produce help message
 *     -i [ --input ] arg                    the folder to process (can also be 
 *                                           passed as positional argument)
 *     -s [ --superpixels ] arg (=400)       number of superpixels
 *     -m [ --minimum-region-size ] arg (=1) minimum region size allowed
 *     -c [ --compactness ] arg (=40)        compactness = regularization trades off
 *                                           color for spatial closeness
 *     -t [ --iterations ] arg (=10)         iterations
 *     -o [ --csv ] arg                      specify the output directory (default 
 *                                           is ./output)
 *     -v [ --vis ] arg                      visualize contours
 *     -x [ --prefix ] arg                   output file prefix
 *     -w [ --wordy ]                        verbose/wordy/debug
 * \endcode
 * \author David Stutz
 */
int main(int argc, const char** argv) {
    
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("input,i", boost::program_options::value<std::string>(), "the folder to process (can also be passed as positional argument)")
        ("superpixels,s", boost::program_options::value<int>()->default_value(400), "number of superpixels")
        ("minimum-region-size,m", boost::program_options::value<int>()->default_value(1), "minimum region size allowed")
        ("compactness,c", boost::program_options::value<double>()->default_value(40.0), "compactness = regularization trades off color for spatial closeness")
        ("iterations,t", boost::program_options::value<int>()->default_value(10), "iterations")
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
    double regularization = parameters["compactness"].as<double>();
    int iterations = parameters["iterations"].as<int>();
    int min_region_size = parameters["minimum-region-size"].as<int>();
    
    // To be comparable to oriSLIC, see lib_slic/README.md and lib_vlfeat/README2.md!
    regularization *= regularization;
    
    std::multimap<std::string, boost::filesystem::path> images;
    std::vector<std::string> extensions;
    IOUtil::getImageExtensions(extensions);
    IOUtil::readDirectory(input_dir, extensions, images);
    
    float total = 0;
    for (std::multimap<std::string, boost::filesystem::path>::iterator it = images.begin(); 
            it != images.end(); ++it) {
        
        cv::Mat image = cv::imread(it->first);
        cv::Mat labels;
        
        int region_size = SuperpixelTools::computeRegionSizeFromSuperpixels(image, 
                superpixels);
        
        boost::timer timer;
        VLSLIC_OpenCV::computeSuperpixels(image, region_size, regularization, 
                min_region_size, iterations, labels);
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
