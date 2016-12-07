# Datasets

Evaluation was based on five datasets; these are briefly introduced in the following. 
The pre-processing, formating and conversion tools used are detailed afterwards.

* [BSDS500](bsds500)
* [NYUV2](nyuv2)
* [SBD](sbd)
* [SUNRGBD](sunrgbd)
* [Fash](fash)

**Downloads of the pre-processed datasets will be made available (if possible)
in the data repository: 
[davidstutz/superpixel-evaluation-data](https://github.com/davidstutz/superpixel-evaluation-data)**

Additional details can be found in [1].

    [1] D. Stutz, A. Hermans, B. Leibe.
        Superpixels: An Evaluation of the State-of-the-Art.
        Computing Research Repository, abs/1612.01601.

![Dataset overview.](docs/DATASETS.png?raw=true "Dataset overview.")

The images can also be found in this [extract of [1]](docs/DATASETS.md).

## BSDS500

The Berkeley Segmentation Dataset 500 [2] was the first dataset used for evaluating
superpixel algorithms. It consists of 500 images, each with 5 different ground truth
segmentations of high quality, divided into a training set of 200 images, a validation
set of 100 images and a test set og 200 images. For parameter optimization, the validation
set was used.

    [2] P. Arbelaez, M. Maire, C. Fowlkes, J. Malik.
        Contour detection and hierarchical image segmentation.
        IEEE Transactions on Pattern Analysis and Machine Intelligence 33 (5) (2011) 898-916.

The ground truth was used as provided, however, converted from `.mat` format 
to `.csv` format. The converted dataset is available in the data repository:
[davidstutz/superpixel-evaluation-data](https://github.com/davidstutz/superpixel-evaluation-data).

In order to manually convert the BSDS500 dataset, use `lib_tools/bsds500_convert_script.m`:

1. Download the BSDS500 dataset from [here](https://www2.eecs.berkeley.edu/Research/Projects/CS/vision/grouping/resources.html).
2. Extract the BSR/BSDS500/data folder into data/BSDS500 (overwriting the provided examples in data/BSDS500/images).
Also note that afterwards there are three folders: `groundTruth`, `csv_groundTruth` and `images`.
3. Adapt the path to the directory below, i.e. set `BSDS500_DIR` correctly.
4. Run the script. Note that this may take some time and memory.

## NYUV2

The NYU Depth Dataset V2 [3] includes 1449 images with pre-processed depth.
Semantic ground truth segmentations with instance labels are provided.
Following Ren and Bo [4], the ground truth has been pre-processed to
remove small unlabeled segments. 199 images were randomly chosen to represent the
validation set and 399 images where randomly chosen for testing.

    [3] N. Silberman, D. Hoiem, P. Kohli, R. Fergus.
        Indoor segmentation and support inference from RGBD images.
        European Conference on Computer Vision, 2012, pp. 746–760.
    [4] X. Ren, L. Bo.
        Discriminatively trained sparse code gradients for contour detection.
        Neural Information Processing Systems, 2012, pp. 593–601.

The ground truth was converted to `.csv` files after thinning unlabeled regions.
The converted dataset (i.e. the corresponding subsets used for validation and
testing) is available in the data repository:
[davidstutz/superpixel-evaluation-data](https://github.com/davidstutz/superpixel-evaluation-data).

In order to manually convert the NYUV2 dataset and extract the used validation and
testing subsets, use `lib_tools/nyuv2_convert.script.m`:

1. Download the dataset from [here](http://cs.nyu.edu/~silberman/datasets/nyu_depth_v2.html).
Make sure that the downloaded file is nyu_depth_v2_labeled.mat.
2. Put the file in `data/NYUV2`.
3. Make sure that `data/NYUV2 contains` contains `nyuv2_test_subset.txt`, 
`nyuv2_train_subset.txt`, `nyuv2_test.txt` and `nyuv2_train.txt`.
4. Adapt the variables below, in particular `NYUV2_DIR` to point to the `data/NYUV2` directory.
5. Run the script. Note that this may take some time and memory.

## SBD

The Stanford Background Dataset [5] combines 715 images from several datasets (see [1]
for details). The images are of varying size, quality and scenes. The semantic ground truth
segmentations provided needed to be pre-processed in order to guarantee connected components.
Validation and testing sets of size 238 and 477, respectively, were chosen at random.

    [5] S. Gould, R. Fulton, D. Koller.
        Decomposing a scene into geometric and semantically consistent regions.
        International Conference on Computer Vision, 2009, pp. 1–8.

The ground truth was converted to `.csv` files. The converted dataset 
(and the corresponding division into validation 
and testing) is available in the data repository: 
[davidstutz/superpixel-evaluation-data](https://github.com/davidstutz/superpixel-evaluation-data).

To manually convert the SBD and select validation and testing images, follow
`lib_tools/sbd_convert_script.m`:

1. Download the Stanford Background Dataset from [here](http://dags.stanford.edu/projects/scenedataset.html).
2. Extract the dataset such that `data/SBD` contains two folders: `images` and `labels`.
3. Make sure that `data/SBD` contains `sbd_test.txt` and `sbd_train.txt`.
4. Adapt the variable `SBD_DIR` below to match the path to `data/SBD`.
5. Run the script. Note that this may take some time and memory.

## SUNRGBD

The SUNRGBD dataset [6] contains 10335 images including pre-processed depth. 
Semantic ground truth segmentations are provided and need to be pre-processed
similar to the NYUV2 dataset. Validation set and testing set of size 200 and 400, 
respectively, were chosen at random. Images that are also included in the NYUV2 
dataset were ignored.

    [6] S. Song, S. P. Lichtenberg, J. Xiao.
        SUN RGB-D: A RGB-D scene understanding benchmark suite.
        IEEE Conference on Computer Vision and Pattern Recognition, 2015, pp. 567–576.

The ground truth was converted to `.csv` files. The converted dataset (including the validation/test split)
is available in the data repository:
[davidstutz/superpixel-evaluation-data](https://github.com/davidstutz/superpixel-evaluation-data).

To manually convert the dataset, follow `lib_tools/sunrgbd_convert_script.m`:

1. Download the SUNRGBD dataset from [here](http://rgbd.cs.princeton.edu/).
2. Make sure to download both the SUNRGBD V1 dataset and the SUNRGBDtoolbox containing the annotations.
3. From the SUNRGBDtoolbox extract `SUNRGBD2dseg.mat` and `SUNRGBDMeta.mat` to `data/SUNRGBD`.
4. From the SUNRGBD V1 dataset extract all files into `data/SUNRGBD`; note that this may take quite some time!
It might be wise to extract the contained directories (xtion, realsense, kv1, kv2) separately as this may take some time!
5. Run the script. Note that this may take some time and memory.

## Fash

The Fashionista dataset [7] contains 685 images with semantic ground truth segmentations.
The ground truth segmentations were pre-processed to ensure connected segments.
Validation set and training set of size 222 and 463, respectively, were chosen at random.

    [7] K. Yamaguchi, K. M. H, L. E. Ortiz, T. L. Berg.
        Parsing clothing in fashion photographs.
        IEEE Conference on Computer Vision and Pattern Recognition, 2012, pp. 3570–3577.

The ground truth was converted to `.csv` files. The converted dataset is available
in the data repository:
[davidstutz/superpixel-evaluation-data](https://github.com/davidstutz/superpixel-evaluation-data).

In order to manually convert the dataset, follow the steps in `lib_tools/fash_convert_script.m`:

1. Download the Fashionista dataset from [here](http://vision.is.tohoku.ac.jp/~kyamagu/research/clothing_parsing/).
2. Extract fashionista_v0.2.1.mat into data/Fash.
3. Adapt the below variables to match the path where `data/Fash` can be found.
4. Run the script. Note that this may take some time and memory.

## Licenses

License details can be found in the corresponding `README.md` files included
in the corresponding subdirectories. We also include the original links below:

Dataset  | Link
---------|-----
BSDS500  | [Web](https://www2.eecs.berkeley.edu/Research/Projects/CS/vision/grouping/resources.html)
NYUV2    | [Web](http://cs.nyu.edu/~silberman/datasets/nyu_depth_v2.html)
SBD      | [Web](http://dags.stanford.edu/projects/scenedataset.html)
SUNRGBD  | [Web](http://rgbd.cs.princeton.edu/)
Fash     | [Web](http://vision.is.tohoku.ac.jp/~kyamagu/research/clothing_parsing/)