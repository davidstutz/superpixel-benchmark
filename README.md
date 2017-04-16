# Superpixels: An Evaluation of the State-of-the-Art

[![Build Status](https://travis-ci.org/davidstutz/superpixel-benchmark.svg?branch=master)](https://travis-ci.org/davidstutz/superpixel-benchmark)

This repository contains the source code used for evaluation in [1], a large-scale 
comparison of state-of-the-art superpixel algorithms.

**[ArXiv](https://arxiv.org/abs/1612.01601) | 
[Project Page](http://davidstutz.de/projects/superpixel-benchmark/) | 
[Datasets](https://github.com/davidstutz/superpixel-benchmark-data) | 
[Doxygen Documentation](https://davidstutz.github.io/superpixel-benchmark/)**

This repository subsumes earlier work on comparing superpixel algorithms:
[davidstutz/gcpr2015-superpixels](https://github.com/davidstutz/gcpr2015-superpixels), 
[davidstutz/superpixels-revisited](https://github.com/davidstutz/superpixels-revisited).

**Please cite the following work if you use this benchmark or the provided tools or implementations:**

    [1] D. Stutz, A. Hermans, B. Leibe.
        Superpixels: An Evaluation of the State-of-the-Art.
        Computing Research Repository, abs/1612.01601.

Also make also sure to cite additional papers when using datasets or superpixel algorithms.

**Updates:**

* An implementation of the average metrics, i.e. Average Boundary Recall (called
Average Miss Rate in the updated paper), Average Undersegmentation Error
and Average Explained Variation (called Average Unexplained Variation in the updated paper)
is provided in `lib_eval/evaluation.h` and an easy-to-use command line tool is provided,
see `eval_average_cli` and the corresponding documentation and examples in
[Executables](docs/EXECUTABLES.md) and [Examples](docs/EXAMPLE.md) respectively.
* As of Mar 29, 2017 the paper was accepted for publication at 
[CVIU](https://www.journals.elsevier.com/computer-vision-and-image-understanding/).
* The converted (i.e. pre-processed) NYUV2, SBD and SUNRGBD datasets are now available
in the [data repository](https://github.com/davidstutz/superpixel-benchmark-data).
* The source code of MSS has been added.
* The source code of PF and SEAW has been added.
* Doxygen documentation is now available [here](http://davidstutz.github.io/superpixel-benchmark/).
* The presented paper was in preparation for a longer period of time — 
  some recent superpixel algorithms are not included in the comparison. These include 
  [SCSP](https://github.com/freifeld/fastSCSP) and [LRW](https://github.com/shenjianbing/lrw14).

## Table of Contents

* [Introduction](#introduction)
* [Algorithms](#algorithms)
    * [Submission](#submission)
* [Documentation](docs/README.md)
    * [Datasets](docs/DATASETS.md)
    * [Algorithms](docs/ALGORITHMS.md)
        * [Submission](docs/SUBMISSION.md)
    * [Benchmark](docs/BENCHMARK.md)
    * [Building](docs/BUILDING.md)
        * [Building CIS](docs/BUILDING_CIS.md)
    * [Executables](docs/EXECUTABLES.md)
    * [Examples](docs/EXAMPLES.md)
    * [Parameters](docs/PARAMETERS.md)
    * [Results](docs/RESULTS.md)
        * [Data](docs/DATA.md)
* [License](#license)

## Introduction

Superpixels group pixels similar in color and other low-level properties.
In this respect, superpixels address two problems inherent to the processing of 
digital images: firstly, pixels are merely a result of discretization; 
and secondly, the high number of pixels in large images prevents many algorithms
from being computationally feasible. Superpixels were introduced as more natural 
entities - grouping pixels which perceptually belong together while heavily reducing
the number of primitives.

This repository can be understood as supplementary material for an extensive 
evaluation of 28 algorithms on 5 datasets regarding visual quality, performance,
runtime, implementation details and robustness - as presented in [1]. To ensure 
a fair comparison, parameters have been optimized on separate training sets; as 
the number of generated superpixels heavily influences parameter optimization, 
we additionally enforced connectivity. Furthermore, to evaluate superpixel algorithms 
independent of the number of superpixels, we propose to integrate over commonly 
used metrics such as Boundary Recall, Undersegmentation Error and Explained Variation. 
Finally, we present a ranking of the superpixel algorithms considering multiple 
metrics and independent of the number of generated superpixels, as shown below.

![Algorithm ranking.](RANKING.png?raw=true "Algorithm ranking.")

The table shows the average ranks across the 5 datasets, taking into account Average
Boundary Recall (ARec) and Average Undersegmentation Error (AUE) - lower is better 
in both cases, see [Benchmark](docs/BENCHMARK.md).
The confusion matrix shows the rank distribution of the algorithms across the datasets.

## Algorithms

The following algorithms were evaluated in [1], and most of them are included in
this repository:

Included                                   | Algorithm    | Reference
-------------------------------------------|--------------|-----------
:ballot_box_with_check:                    | CCS          | [Ref. & Web](http://www.emrahtasli.com/research/spextraction/)
[Instructions](docs/BUILDING_CIS.md)       | CIS          | [Ref.](http://www.csd.uwo.ca/~olga/Papers/eccv2010final.pdf) & [Web](http://www.csd.uwo.ca/faculty/olga/)
:ballot_box_with_check:                    | CRS          | [Ref.](http://link.springer.com/chapter/10.1007%2F978-3-642-40395-8_21#page-1) & [Web](http://www.vsi.cs.uni-frankfurt.de/research/superpixel-segmentation/)
:ballot_box_with_check:                    | CW           | [Ref.](https://www.tu-chemnitz.de/etit/proaut/rsrc/cws_pSLIC_ICPR.pdf) & [Web](https://www.tu-chemnitz.de/etit/proaut/forschung/cv/segmentation.html.en)
:ballot_box_with_check:                    | DASP         | [Ref.](http://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=6460572) & [Web](https://github.com/Danvil/dasp)
:ballot_box_with_check:                    | EAMS         | [Ref.](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.8.5341&rep=rep1&type=pdf), [Ref.](http://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=977560), [Ref.](https://courses.csail.mit.edu/6.869/handouts/PAMIMeanshift.pdf) & [Web](http://coewww.rutgers.edu/riul/research/code/EDISON/)
:ballot_box_with_check:                    | ERS          | [Ref.](http://www.merl.com/publications/docs/TR2011-035.pdf) & [Web](http://mingyuliu.net/)
:ballot_box_with_check:                    | FH           | [Ref.](http://www.cs.cornell.edu/~dph/papers/seg-ijcv.pdf) & [Web](https://cs.brown.edu/~pff/segment/index.html)
:ballot_box_with_check:                    | MSS          | [Ref.](http://avestia.com/MVML2014_Proceedings/papers/67.pdf)
:ballot_box_with_check:                    | PB           | [Ref.](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=6126393&tag=1) & [Web](http://yuhang.rsise.anu.edu.au/yuhang/misc.html)
:ballot_box_with_check:                    | preSLIC      | [Ref.](https://www.tu-chemnitz.de/etit/proaut/rsrc/cws_pSLIC_ICPR.pdf) & [Web](https://www.tu-chemnitz.de/etit/proaut/forschung/cv/segmentation.html.en)
:ballot_box_with_check:                    | reSEEDS      | [Web](http://davidstutz.de/projects/superpixelsseeds/)
:ballot_box_with_check:                    | SEAW         | [Ref.](http://patrec.cs.tu-dortmund.de/pubs/papers/Strassburg2015-OIS) & [Web](https://github.com/JohannStrassburg/InfluenceSegImageParsingCode)
:ballot_box_with_check:                    | SEEDS        | [Ref.](http://arxiv.org/pdf/1309.3848v1.pdf) & [Web](http://www.mvdblive.org/seeds/)
:ballot_box_with_check:                    | SLIC         | [Ref.](http://www.kev-smith.com/papers/SLIC_Superpixels.pdf) & [Web](http://ivrl.epfl.ch/research/superpixels)
:ballot_box_with_check:                    | TP           | [Ref.](http://www.cs.toronto.edu/~babalex/09.pami.turbopixels.pdf) & [Web](http://www.cs.toronto.edu/~babalex/research.html)
:ballot_box_with_check:                    | TPS          | [Ref.](http://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=6298495) & [Web](http://hzfu.github.io/subpage/codes.html)
:ballot_box_with_check:                    | vlSLIC       | [Web](http://www.vlfeat.org/overview/slic.html)
:ballot_box_with_check:                    | W            | [Web](http://docs.opencv.org/2.4/modules/imgproc/doc/miscellaneous_transformations.html?highlight=watershed#watershed)
:ballot_box_with_check:                    | WP           | [Ref.](http://cmm.ensmp.fr/~machairas/waterpixels.html) & [Web](http://cmm.ensmp.fr/~machairas/waterpixels.html)
:ballot_box_with_check:                    | PF           | [Ref.](http://users.dickinson.edu/~jmac/publications/fast-superpixels-WMVC09.pdf) & [Web](http://users.dickinson.edu/~jmac/publications/PathFinder.zip)
:ballot_box_with_check:                    | LSC          | [Ref.](http://www.cv-foundation.org/openaccess/content_cvpr_2015/papers/Li_Superpixel_Segmentation_Using_2015_CVPR_paper.pdf) & [Web](http://jschenthu.weebly.com/projects.html)
:ballot_box_with_check:                    | RW           | [Ref.](http://cns.bu.edu/~lgrady/grady2004multilabel.pdf) & [Web](http://cns.bu.edu/~lgrady/software.html)
:ballot_box_with_check:                    | QS           | [Ref.](http://vision.cs.ucla.edu/papers/vedaldiS08quick.pdf) & [Web](http://www.vlfeat.org/overview/quickshift.html)
:ballot_box_with_check:                    | NC           | [Ref.](http://ttic.uchicago.edu/~xren/publication/xren_iccv03_discrim.pdf) & [Web](http://www.cs.sfu.ca/~mori/research/superpixels)
:ballot_box_with_check:                    | VCCS         | [Ref.](http://www.cv-foundation.org/openaccess/content_cvpr_2013/papers/Papon_Voxel_Cloud_Connectivity_2013_CVPR_paper.pdf) & [Web](http://pointclouds.org/documentation/tutorials/supervoxel_clustering.php)
:ballot_box_with_check:                    | POISE        | [Ref.](http://web.engr.oregonstate.edu/~lif/Middle_Child_ICCV15.pdf) & [Web](http://rehg.org/poise/)
:ballot_box_with_check:                    | VC           | [Ref.](http://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=6186738) & [Web](http://www-personal.umich.edu/~jwangumi/software.html)
:ballot_box_with_check:                    | ETPS         | [Ref.](http://www.cs.toronto.edu/~yaojian/cvpr15.pdf) & [Web](https://bitbucket.org/mboben/spixel)
:ballot_box_with_check:                    | ERGC         | [Ref.](https://hal.archives-ouvertes.fr/hal-00945893/document), [Ref.](http://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=7025886) & [Web](https://sites.google.com/site/pierrebuyssens/code/ergc)

### Submission

To keep the benchmark alive, we encourage authors to make their implementations
publicly available and integrate them into this benchmark. We are happy to help with the
integration and update the results published in [1] and on the 
[project page](http://davidstutz.de/projects/superpixel-benchmark/).
Also see the [Documentation](docs/README.md) for details.

## License

Note that part of the provided algorithms come with different licenses, see [Algorithms](docs/ALGORITHMS.md)
for details. Also note that the datasets come with different licenses, see [Datasets](docs/DATASETS.md)
for details.

Further, note that the additional dataset downloads as in [Datasets](docs/DATASETS.md)
follow the licenses of the original datasets.

The remaining source code provided in this repository is licensed as follows:

Copyright (c) 2016, David Stutz All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
