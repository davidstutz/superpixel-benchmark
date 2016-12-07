# README

The original README of the SBD [1] dataset can be found below. The 
converted dataset will be made available if possible.

    [1] S. Gould, R. Fulton, D. Koller.
        Decomposing a scene into geometric and semantically consistent regions.
        International Conference on Computer Vision, 2009, pp. 1–8.

Instructions to convert the dataset manually can be found in `docs/DATASETS.md`.

## Original README

README FOR ICCV09DATA

This dataset contains 715 images sourced from LabelMe, MSRC, PASCAL
VOC, and Geometric Context. Labels were obtained using Amazon's
Mechanical Turk (AMT). The labels are:

  - horizons.txt         : image dimensions and location of horizon
  - labels/*.regions.txt : integer matrix indicating each pixel's
                           semantic class (sky, tree, road, grass,
                           water, building, mountain, or foreground
                           object). A negative number indicates
                           unknown.
 - labels/*.surfaces.txt : integer matrix indicating each pixel's
                           geometric class (sky, horizontal, or
                           vertical).
 - labels/*.layers.txt   : integer matrix indicating distinct
                           image regions.

If you use this data, please cite our ICCV 2009 paper:

 - Decomposing a Scene into Geometric and Semantically Consistent Regions
   Stephen Gould, Richard Fulton, Daphne Koller, ICCV 2009

Stephen Gould
sgould@stanford.edu

