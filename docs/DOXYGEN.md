# Doxygen

Doxygen documentation is available for `lib_eval`, the C++ command line tools
and some OpenCV wrappers for individual algorithms.

**Doxygen documentation can be found at [davidstutz.github.io/superpixel-benchmark](http://davidstutz.github.io/superpixel-benchmark/).**

## Building Doxygen

While this repository contains a built version of the documentation, it can be
updated or re-built by calling

    doxygen config.doxygen

from the root directory. Note that Doxygen needs to be installed; use

    sudo apt-get install doxygen

on Ubuntu 14.04. Afterwards, copy the images in `docs/` into `docs/html`!