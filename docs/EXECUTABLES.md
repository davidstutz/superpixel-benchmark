# Executables

As discussed in [Building](docs/BUILDING.md) and depending on the chosen options,
several executables are provided (usually found in `bin`). Most of these provide
easy-to-use command line tools for the various algorithms. However, some utility
command line tools for evaluation and visualization are also provided.

For algorithms written in MatLab, Bash scripts are provided to easily run the
algorithms from the command line. This was mainly used for parameter optimization
and evaluation. However, it is recommended to use the corresponding MatLab
functions instead.

The same holds for PF, which is writen in Java, and WP, written in Python.

**Examples of calling the executables can be found in `examples`!**

## Algorithms in C++

All command line tools for algorithms in C++ have the following options in common:

    $ ../bin/cis_cli --help
    Allowed options:
      -h [ --help ]                   produce help message
      -i [ --input ] arg              folder containing the images to process
      # Algorithm specific options ...
      -o [ --csv ] arg                save segmentation as CSV file
      -v [ --vis ] arg                visualize contours
      -x [ --prefix ] arg             output file prefix
      -w [ --wordy ]                  verbose/wordy/debug

`--input` is additionally a positional option. The algorithm specific options can
be displayed using the `--help` options. For details on the specific options, the reader
is referred to the corresponding publication(s), see [here](docs/ALGORITHMS.md) for
references.

The `--csv` option will output the superpixel segmentations as `.csv` files in
the provided directory, which is created if it does not exist. The naming follows
the naming of the images found in the directory specified by `--input`. Similarly,
`--vis` outputs visualizations in the provided directory, which is also created
if it does not exist.

`--prefix` can be used to specify a prefix, then the output files (CSV files and
visualizations) are prefixed with the given string. `--wordy` will cause the
tool to provide more detailed output while running (i.e. be verbose).

Examples:

    $ build
    $ cmake ..
    $ make
    # reSEEDS (built by default)
    ../bin/reseeds_cli --input ../data/BSDS500/images/test/ --superpixels 1200 --bins 5 --neighborhood 0 --confidence 0.1 --spatial-weight 0 --iterations 25 --color-space 0 -o ../output/reseeds -w
    # ERGC (built by default)
    $ ../bin/ergc_cli --input ../data/BSDS500/images/test/ --superpixels 1200 --color-space 1 --perturb-seeds 0 --compacity 0 -o ../output/ergc -w
    # ETPS (built by default)
    ../bin/etps_cli --input ../data/BSDS500/images/test/ --superpixels 1200 --regularization-weight 0.01 --length-weight 0.1 --size-weight 1 --iterations 25 -o ../output/etps -w

## Utilities in C++

`eval_boundaries2labels_cli` converts boundary maps saved as `.csv` files to superpixel segmentations as `.csv`:

    $ ../bin/eval_boundaries2labels_cli --help
    Allowed options:
      --help                     produce help message
      -i [ --input-csv ] arg     folder containing the CSV files to process
      -m [ --input-images ] arg  folder containing the corresponding images to 
                                 process
      -r [ --overwrite ]         Overwrite original files
      -o [ --csv ] arg (=output) save segmentation as CSV file
      -w [ --wordy ]             wordy/verbose

An example can be found in `examples/bash/tp.sh`.

`eval_connected_relabel_cli` takes superpixel segmentations as `.csv` files and
relabels them such that superpixels represent connected components. The original
files can either be overwritten, or the relabeled superpixel segmentations can
be saved in a separate directory:

    $ ../bin/eval_connected_relabel_cli --help
    Allowed options:
      --help                     produce help message
      -i [ --input-csv ] arg     folder containing the corresponding images to 
                                 process
      -m [ --input-images ] arg  dummy option!
      -r [ --overwrite ]         Overwrite original files
      -o [ --csv ] arg (=output) save segmentation as CSV file
      -w [ --wordy ]             wordy/verbose

An example can be found in `examples/bash/tp.sh`.

`eval_parameter_optimization` demonstrates the parameter optimization procedure
used in [1]. The parameters are partly the original parameters used in the paper.

    [1] D. Stutz, A. Hermans, B. Leibe.
        Superpixels: An Evaluation of the State-of-the-Art.
        Computing Research Repository, abs/1612.01601.

**Note that it should not be required to use this tool in most cases, unless
parameters need to be optimized on new datasets or for new algorithms. THE PARAMETERS
USED IN [1] ARE DETAILED [HERE](docs/PARAMETERS.md).**

For parameter optimization, the command line tools for all algorithms are normed
in the sense that they provide the parameters `-i` and `-o` for input and output.
Having a close look at `eval_parameter_optimization_cli/main.cpp` shows that 
the parameters for the different algorithms are hard-coded, e.g.

    /**
     * Connector for parameter optimization of reSEEDS.
     * 
     * @param img_directory
     * @param gt_directory
     * @param base_directory
     */
    void connector_reSEEDS(boost::filesystem::path img_directory, 
            boost::filesystem::path gt_directory, boost::filesystem::path base_directory,
            std::vector<int> superpixels) {

        for (unsigned int k = 0; k < superpixels.size(); k++) {
            ParameterOptimizationTool tool(img_directory, gt_directory,
                    base_directory / boost::filesystem::path(std::to_string(superpixels[k])),
                RELATIVE_PATH + "/bin/reseeds_cli", FAIR);

            tool.addIntegerParameter("superpixels", "--superpixels", std::vector<int>{superpixels[k]});
            tool.addIntegerParameter("bins", "--bins", std::vector<int>{1, 3, 5, 7}); // 4
            tool.addIntegerParameter("neighborhood", "--neighborhood", std::vector<int>{0, 1}); // 2
            tool.addFloatParameter("confidence", "--confidence", std::vector<float>{0.1f}); // 1
            tool.addFloatParameter("spatial-weight", "--spatial-weight", std::vector<float>{0.0f, 0.25f, 0.5f}); // 3
            tool.addIntegerParameter("iterations", "--iterations", std::vector<int>{1, 10, 25}); // 3
            tool.addIntegerParameter("color-space", "--color-space", std::vector<int>{0, 1, 2}); // 3

            tool.optimize();
        }
    }

Also note that the Java path as well as the MatLab path and the directory containing
the executables are hard-coded:

    std::string RELATIVE_PATH = ".";
    std::string MATLAB_EXECUTABLE = "/home/david/MATLAB/R2014b/bin/matlab";
    std::string JAVA_EXECUTABLE = "/home/david/jdk-1.8.0_45/release/java";

The executable directory is `RELATIVE_PATH + "/bin"`. Then, only the algorithm 
needs to be specified together with the remaining options:

    $ ../bin/eval_parameter_optimization_cli --help
    Allowed options:
      --img-directory arg                   image directory
      --gt-directory arg                    ground truth directory
      --base-directory arg                  base directory
      --algorithm arg                       algorithm to optimize: reseeds, 
      --depth-directory arg                 depth directory
      --intrinsics-directory arg            intrinsics directory
      --matlab-executable arg (=../../MATLAB/R2014b/release/matlab)
                                            matlab executable path
      --java-executable arg (=../../jdk-1.8.0_45/release/java)
                                            java executable
      --not-fair                            do not use fair parameters
      --help                                produce help message

`eval_summary_cli` may the most important tool provided. It bundles all evaluation
metrics discussed in [1] plus additional metrics and, given a directory containing
superpixel segmentations as `.csv` files and directories with the corresponding images
and ground truth segmentations as `.csv` files, summarizes the performance of
the superpixel segmentations. The provided options are:

    $ ../bin/eval_summary_cli --help
    Allowed options:
      --sp-directory arg    superpixel segmentation directory
      --img-directory arg   image directory
      --gt-directory arg    ground truth directory
      --append-file arg     append file
      --vis                 visualize results
      --help                produce help message

Usage examples can be found in `examples/bash`. For `examples/bash/reseeds.sh`
the created summary looks as follows:

    metric,mean[0],mean[1],mean[2],mean[3],mean[4],mean_min,mean_max
    ue,0.0289073,0.0303683,0.0298518,0.0306024,0.0354846,0.019728,0.0461874
    oe,0.970836,0.971261,0.969193,0.967363,0.963669,0.954067,0.979919
    rec,0.981173,0.976853,0.979881,0.981565,0.97807,0.965346,0.98946
    pre,0.0580179,0.0563268,0.0588434,0.0611075,0.0646213,0.0412821,0.0831838
    ue_np,0.0574405,0.0603835,0.0592958,0.0607787,0.0704587,0.0393508,0.0914618
    ue_levin,9.14557,11.1276,13.0967,12.9077,12.8244,2.14357,28.7954
    asa,0.971093,0.969632,0.970149,0.969398,0.964516,0.953813,0.980272
    sse_rgb,583.115,583.115,583.115,583.115,583.669,578.606,578.606
    sse_xy,84.9599,84.9599,84.9599,84.9599,85.0727,85.0815,85.0815
    co,0.101888,0.101888,0.101888,0.101888,0.101926,0.101995,0.101995
    ev,0.924642,0.924642,0.924642,0.924642,0.924634,0.925177,0.925177
    mde,0.206509,0.232733,0.216243,0.20705,0.22207,0.163584,0.284383
    icv,20.1905,20.1905,20.1905,20.1905,20.1899,20.1428,20.1428
    cd,0.653127,0.653127,0.653127,0.653127,0.653058,0.65289,0.65289
    reg,0.24697,0.24697,0.24697,0.24697,0.247334,0.247376,0.247376
    sp,1201.33,1201.33,1201.33,1201.33,1201.33,1201.33,1201.33
    sp_size,128.525,128.525,128.525,128.525,128.525,128.525,128.525
    sp_size_variation,76.7064,76.7064,76.7064,76.7064,76.7555,76.7697,76.7697

Note that some statistics have been omitted here. The results can more easily
be analyzed using Excel or Calc:

![Evaluation summary.](docs/SUMMARY.png?raw=true "Evaluation summary.")

`eval_visualization_cli` can be used to visualize superpixel segmentations
available as `.csv` files given the corresponding images:

    $ ../bin/eval_visualization_cli --help
    Allowed options:
      --help                     produce help message
      --csv arg                  superpixel segmentation (as CSV)
      --images arg               image
      --contours                 draw contours
      --contours-on-white        draw contours on white image
      --means                    draw means
      --perturbed-means          draw perturbed means
      --random                   randomly color
      -v [ --vis ] arg (=output) output folder
      -x [ --prefix ] arg        input and output file prefix
      -w [ --wordy ]             verbose/wordy/debug

The offered visualizations are: drawing contours along superpixel boundaries,
coloring the superpixels by their mean color, coloring the superpixels by their
mean colors plus random noise, drawing the contours along superpixel boundaries on
a white background, coloring the superpixels with random colors.

Example:

![Visualizations.](docs/VISUALIZATIONS.png?raw=true "Visualizations.")

## Algorithms in MatLab

For parameter optimization purposes, all algorithms implemented in MatLab provide
both a Bash script and a MatLab function. For example, `tp_cli` contains both
`tp_cli/tp_dispatcher.sh` and `tp_cli/tp_cli.m`.

The MatLab function usually provides the following parameters:

* `folder`: the input folder containing the images to process.
* `csv_folder`: the output folder to save the superpixel segmentations as `.csv`.
* `vis_folder`: the output folder for visualizations.
* `prefix`: prefix used to output files (both CSV and visualizations).
* `wordy`: whether to run in verbose mode.

Details on the algorithm specific algorithms can be found in the corresponding
MatLab functions. Usually, all parameters after `folder` are optional!

Alternatively, the Bash file usually provides options similar to the options
provided by the C++ command line tools. `-i` is the input directory; `-o` the output
CSV directory; `-v` the output visualization directory and `-h` for a help message, e.g.

    $ ../tp_cli/tp_dispatcher.sh -h
    Allowed options:
    -i folder to process
    # Algorithm specific options ...
    -o output folder
    -v folder for visualizatioN
    -x prefix of output files
    -w verbose output
    -e path to MatLab executable
    -a path to add, usually the lib_seaw path

Additionally, it is required to provide the path to the MatLab functions via `-a`,
in this case the path to `tp_cli.m`, as well as the path to the MatLab
executable via `-e`.

Examples of using the Bash scripts assuming MatLab R2015b to be installed in the home
directory:

    $ cd examples
    # EAMS
    $ ../eams_cli/eams_dispatcher.sh -i ../data/BSDS500/images/test/ -b 2 -m 50 -r 1 -o ../output/eams -w -a ../eams_cli -e ~/MATLAB/R2015b/bin/matlab
    # POISE
    $ ../poise_cli/poise_dispatcher.sh -i ../data/BSDS500/images/test/ -s 1200 -t 0.01 -g 2.5 -m 2 -c 0.6 -o ../output/poise -w -a ../poise_cli -e ~/MATLAB/R2015b/bin/matlab

Examples of using the MatLab functions directly:

    % Options correspnd to the above call of EAMS.
    folder = '../data/BSDS500/images/test/';
    bandwidth = 2;
    minimum_size = 50;
    color_space = 1;
    csv_folder = '../output/eams';
    vis_folder = ''; # No visualization ...

    fprintf('Running EAMS:\n');
    fprintf('folder: %s\n', folder);
    fprintf('bandwidth: %f\n', bandwidth);
    fprintf('minimum_size: %d\n', minimum_size);
    fprintf('color_space: %d\n', color_space);
    fprintf('csv_folder: %s\n', csv_folder);
    fprintf('vis_folder: %s\n', vis_folder);

    try
        eams_cli(folder, bandwidth, minimum_size, color_space, csv_folder, vis_folder);
    catch e
        fprintf([e.message '\n'])
    end

## Algorithms in Java

PF is the only algorithm written in Java. A Bash script as described above
is provided. Alternatively, the algorithm can be run as follows:

    java -jar lib_pf/PathFinder.jar data/BSDS500/images/test output/pf 30 1200

## Algorithms in Python

WP is the only algorithm written in Python. A Bash script as described above is provided.
Alternatively, the algorithm can be called directly using Python:

    python lib_wp/demo_waterpixels_smil_with_parser.py --original_image data/BSDS500/images/test/3096.jpg --superpixels 1200 --weight 10 --output output/wp/