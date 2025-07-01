# Docker Image
As years pass by and operating systems get upgraded, it's important to have a working snapshot of a computational environment that demonstrates successful compilation of the code in this repository.
This directory aims to provide such a reference implementation.

Here you will find a `Dockerfile` which makes use of `--build-arg BASE_IMAGE=...` to modify the base operating system used in the image.

The `Dockerfile` relies on `docker.patch` and `timer-patch.sh` to patch the code in a manner which is compatible (as of 2025-06-15) with the following base images:

- `debian:buster:20240612-slim`
- `debian:bullseye:20250520-slim`
- `debian:bookworm:20250520-slim`
- `debian:trixie:20250520-slim`

The patch file applies a number of changes to keep the code backwards-compatible but make use of the latest syntax, libraries, and features of the C++ standard.
- Most of the changes involve detecting the `OpenCV` version and using the appropriate API.
- Several patches involve updating the `eigen3` syntax.
- `PCL` imports are slimmed down for a headless environment, and some methods are patched based on version.
- `boost::timer` patches are performed with `timer-patch.sh`, which became enforced in `debian:trixie` but were warnings prior to that release.
- In `Dockerfile`, there is an inline `sed` to `CImg` to make it compatible with `debian:trixie`.
- In `Dockerfile`, there is an inline `sed` to `CMakeLists.txt` to scope the use of `cmake` from `2.8.12` up to version `3.25`.
- ARM64 compatibility is achieved by patching `ETPS` to use NEON instructions in place of SSE instructions.

The `Dockerfile` uses multi-stage builds to reduce the size of the final image by using the first stage to build the code and the second stage to copy the binaries into the final image and configure the `PATH`.

If you are looking to compile this project on your own operating system, the `Dockerfile` and `docker.patch` + `timer-patch.sh` files can serve as a guide.

If you are looking to run the binaries, they have been pre-built and published to [Docker Hub](https://hub.docker.com/r/mathematicalmichael/superpixels/tags).


A `makefile` is supplied with example usage of the container, including an example of how to prepare images for the algorithms.

Usage relies on the user to create `in/` and `out/` sub-directories, and to place sample images in the `in/` directory (in `.bmp`, `.png`, or `.jpeg`/`.jpg` format).

```bash
mkdir -p in
mkdir -p out
```

## Changes
In addition to the algorithms enabled by default, the following have been changed to `ON`:

- BUILD_VC (non-commercial use only)
- BUILD_FH
- BUILD_MSS
- BUILD_PB (non-commercial use only)
- BUILD_PRESLIC
- BUILD_W
- BUILD_LSC
- BUILD_CCS
- BUILD_CW
- BUILD_DASP
- BUILD_VCCS
- BUILD_REFH
- BUILD_VLSLIC

BUILD_CIS is `OFF` due to licensing
NOTE: BUILD_ETPS (non-commercial use only) is `ON` and has been patched to use NEON instructions on ARM64 in place of the SSE instructions on x86_64.


The Matlab algorithms have been left disabled due to licensing concerns with the language.
If you know how to safely ship matlab in a container (or want to attempt a migration to Octave), contributions are welcome.


# Quick Start

Reference the `makefile` (which here acts like a collection of shell-scripts), for docker build and run syntax. 
Adapt it to your needs, as it is meant to provide a reference implementation which may or may not align with your expectations (e.g., folder names, locations, etc).

For example, to run the `MSS` algorithm, you can use the following command:

```bash
make mss
```

Which will ensure an image is built/tagged, and then runs `mss_cli` to process your `in/` directory and save the results in `out/`.

There is an example of one algorithm's syntax in the [`makefile`][./makefile]:

```bash
make help_mss
```
