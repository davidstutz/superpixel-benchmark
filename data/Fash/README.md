# README

The original README of the Fashionista [1] dataset can be found below. The 
converted dataset will be made available if possible.

    [1] K. Yamaguchi, K. M. H, L. E. Ortiz, T. L. Berg.
        Parsing clothing in fashion photographs.
        IEEE Conference on Computer Vision and Pattern Recognition, 2012, pp. 3570–3577.

Instructions to convert the dataset manually can be found in `docs/DATASETS.md`.

**Also note `LICENSE.md`!**

## Original README

Clothing Parser
===============

Kota Yamaguchi, April 2012

Implementation of the following paper:

    Parsing clothing in fashion photographs
    Kota Yamaguchi, Hadi Kiapour, Luis E Ortiz, Tamara L Berg
    Compute Vision and Pattern Recognition 2012.

Also the fully annotated samples of Fashionista dataset is included.

File contents
-------------

    data/              data directory
    lib/               library directory
    lib/+bsr           image segmentation [Arbelaez 2011]
    lib/+improc        image processing utilities
    lib/+liblinear     liblinear package
    lib/+libsvm        libsvm package
    lib/+sbu           clothing parser implementation
    lib/+stat          statistics utilities
    lib/+uci           pose estimator [Yang 2011]
    lib/libdai         libDAI package
    tasks/             application directory
    tmp/               cache directory
    startup.m          application initializer
    README.markdown    this file
    LICENSE            copyright notice


Compile
-------

## Prerequisite

The software is designed for UNIX environment.

Matlab (R2010b or later) and build tools (i.e., gcc, make, etc) must be
installed to the system. In addition, the clothing parser depends on a couple
of libraries. The dependency can be installed through package managers.

Debian/Ubuntu

    apt-get install libjpeg-dev libpng-dev libboost-dev libgmp-dev

Redhat

    yum install libjpeg-devel libpng-devel boost-devel gmp-devel

MacPorts

    port install jpeg libpng boost gmp


## Build

In the matlab shell, `cd` to the project directory, and type

    sbu.make


Implementation
--------------

The software takes advantage of object-oriented design in Matlab. That is, a few
of the matlab class objects in the package defines important functionality to
realize clothing parsing.

## sbu.Photo

The class defines a data instance in parsing application. The class keeps three
kinds of data in its property:

 * image
 * image segmentation
 * pose estimation

To create a new instance, give a new image to the constructor argument.

     im = imread('myphoto.jpg');
     photo = sbu.Photo(im);

This processes a new image with image segmentation algorithm of [Arbelaez 2010]
and pose estimator of [Yang 2011] trained on sbu.Fashionista dataset. It is
also possible to individually compute them.

     im = imread('myphoto.jpg');
     pose = sbu.Pose(pose_estimator.estimate(im));
     segmentation = sbu.Segmentation(im);
     photo = sbu.Photo(im, 'pose', pose, 'segmentation', segmentation);

See `help sbu.Photo` for detail.

## sbu.ClothParser

Main parser implementation.

     parser = sbu.ClothParser;                  % create an empty parser
     parser = sbu.ClothParser.default_parser;   % default trained parser
     photo = parser.parse(im);                  % parse a new image

## uci.PoseEstimator

Pose estimator implementation of [Yang 2011].

     estimator = uci.PoseEstimator              % empty estimator
     estimator = uci.PoseEstimator.default_estimator;  % default estimator
     boxes = estimator.estimate(im);            % estimate a pose
     pose = sbu.Pose(boxes);                    % encapsulate in sbu.Pose object

## sbu.Fashionista

Class to access Fashionista dataset. To load samples:

    photos = sbu.Fashionista.load;

List of clothing labels (id,name) can be obtained by:

    clothings = sbu.Fashionista.clothings;


Usage
-----

Change to the fashionista project directory, and launch matlab there. The
startup script will automatically set up the path to the dependent libraries.


## Parsing a new image

    im = imread('myphoto.jpg');
    parser = sbu.ClothingParser.default_parser;
    photo = parser.parse(im); % all-way parsing
    photo = parser.parse(im,{'t-shirt','pants','shoes'}); % specific parsing
    photo.show('image',im,'labels',sbu.Fashionista.clothings);

Note that parsing can take long due to the expense of the segmentation
algorithm.

See `help sbu.ClothParser` for detail.

## Training a pose estimator

The training requires struct array of UCI format, which can be obtained by
to_uci method of sbu.Photo class.

    estimator = uci.PoseEstimator;
    estimator.train(photos.to_uci);

The train method can take a couple of optional arguments. Check
`help.uci.PoseEstimator`.


## Training a clothing parser

Give an array of sbu.BasePhoto objects.

    parser = sbu.ClothingParser;
    parser.train(photos);

Note that sbu.BasePhoto class is an abstract class. As long as the interface in
described in this class is implemented (such as sbu.Photo class), the parser
should be able to train a model.

## Run experiments in the paper

The protocol is implemented in `tasks/experiment.m`. Call

    experiment.main(10);    % run experiments 10 times
    report_experiments(10); % show summary of 10 experiments

Check `help experiment` for detail. Please note that the pose
estimator of [Yang 2011] has a lot of random components and therefore the above
protocol does not guarantee the exactly same output to the numbers shown in the
paper. However, running the experiment multiple times is likely to show the
similar number in the paper.

## Visualizing data samples

To look at an image

    photo.show;

To look at pose

    photo.pose.show;                         % Plain pose in xy coord
    photo.pose.show('image', photo.image);   % Pose overlaid on an image

To look at segmentation or parsing results

    photo.segmentation.show_superpixels;
    photo.segmentation.show;                        % Plain parse
    photo.segmentation.show('image', photo.image);  % Overlaid parsing

    % Adding a color bar in the plot
    clothings = sbu.Fashionista.clothings;
    photo.segmentation.show('image', photo.image, 'labels', clothings);


Note
----

The code requires a lot of memory (2-3 GB) for parsing a new image. Mostly this
requirement comes from superpixel segmentation. Training of pose estimator
requires a lot more (5-7 GB).


License
-------

The code is distributed under The BSD 3-Clause License.

Links
-----

### Image segmentation

http://www.eecs.berkeley.edu/Research/Projects/CS/vision/grouping/resources.html

### Pose estimator

http://phoenix.ics.uci.edu/software/pose/
http://www.robots.ox.ac.uk/~vgg/data/stickmen/

### libsvm/liblinear

http://www.csie.ntu.edu.tw/~cjlin/libsvm/
http://www.csie.ntu.edu.tw/~cjlin/liblinear/

### libDAI

http://cs.ru.nl/~jorism/libDAI/


Release
-------

v0.1  April 2012  Initial release
v0.2  August 2012 Fixed flipped pose annotations (Thanks, Yi Yang!)
