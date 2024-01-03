# Docker image
As years pass by and operating systems get upgraded, it's important to have at least a working snapshot of a computational environment.

A docker image has been contributed to help facilitate the use of many of the algorithms. It is not complete in scope, but should act as a starting place for compiling and using the code in this repository.

A `makefile` is supplied with example usage of the container, including an example of how to prepare images for the algorithms.

`ffmpeg`, `graphicsmagick`, and `potrace` are installed in the container as utilities related to image processing. `git` and `python 3.11.4` are also bundled.


## Changes
In addition to the algorithms enabled by default, the following have been changed to `ON`:

- BUILD_VC
- BUILD_FH
- BUILD_MSS
- BUILD_PB
- BUILD_PRESLIC
- BUILD_W
- BUILD_LSC
- BUILD_CCS
- BUILD_DASP
- BUILD_VCCS
- BUILD_REFH
- BUILD_VLSLIC

BUILD_CIS is `OFF` due to licensing
BUILD_CW is `OFF` due to cvGetMatSize declaration error

The Matlab algorithms have been left disabled due to licensing concerns with the language.
If you know how to safely ship matlab in a container (or want to attempt a migration to Octave), contributions are welcome.


# Quick Start
Reference the `makefile` (which acts like a collection of shell-scripts), for docker usage syntax. Adapt it to your needs, as it is meant to provide a reference implementation which may or may not align with your expectations (e.g., folder names, locations, etc).

```bash
make run
```

Will start `bash` in an interactive emphemeral container, with `raw` being a read-only bind-mounted directory, and `in` being where you put `.bmp` images. See `make convert` for an example of how to prepare your images using the docker container.

There is an example of one algorithm's syntax in the [`makefile`][./makefile]:

```bash
make help_mss
```

