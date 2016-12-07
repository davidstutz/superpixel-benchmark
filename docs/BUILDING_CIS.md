# Building CIS

Unfortunately, the code from CIS cannot be shipped with this library due to the 
restrictive license of the corresponding code.
Therefore, only a command line tool in `cis_cli` is provided.

Installation instructions:

Go to [http://www.csd.uwo.ca/faculty/olga/](http://www.csd.uwo.ca/faculty/olga/) and download the code.

Extract the archive into `lib_cis` in order to obtain the following directory structure (where the `CMakeLists.txt` files are provided by this library):

    lib_cis
    |- vlib
        |- include
        |- utils
        |- CMakeLists.txt (provided by this library, not included in the download described above)
    |- README.txt (original README)
    |- README.md (this README)
    |- maxflow.cpp
    |- ...
    |- superpixels.h (provided by this library, not included in the download described above)
    |- CMakeLists.txt (provided by this library, not included in the download described above)

Comment out the `main` function in `superpixels.cpp`.

Change the declaration of `loadEdges` in `superpixels.cpp` to

    void loadEdges(vector<Value> &weights,int num_pixels,int width,int height,
                   Value lambda, vlib::image<unsigned char> *edges)

or use `-fpermissive` (default) (adapt `CMakeLists.txt` accordingly).

In `loadEdges` comment out the following line:

    image<uchar> *edges = loadPGM(name);

Adapt `computeWeights` and `computeWeightsColor` to accept an additional
parameter named `sigma`, thereby commenting the following line out:

    float sigma = 2.0f;

Depending on the operating system, some changes in `energy.h` are required: Change occurrences of

    add_tweights(y, 0, C);
    add_edge(x, y, B+C, 0);

to

    this->add_tweights(y, 0, C);
    this->add_edge(x, y, B+C, 0);

Now use `-DBUILD_CIS=On` to run CMake.