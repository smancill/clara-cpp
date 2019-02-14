# CLARA

A service-oriented framework to provide a computing environment for efficient
Big Data processing.

[![Build Status](https://travis-ci.org/JeffersonLab/clara-cpp.svg?branch=master)](https://travis-ci.org/JeffersonLab/clara-cpp)
[![Javadoc](https://img.shields.io/badge/doxygen-master-blue.svg?style=flat)](https://claraweb.jlab.org/clara/api/cpp/)


## Documentation

The reference documentation is available at <https://claraweb.jlab.org>.


## Build notes

### Dependencies

CLARA requires a C++14 compiler and the [xMsg](https://github.com/JeffersonLab/xmsg-cpp)
pub-sub messaging system. Follow the xMsg project instructions to install the build
toolchain, the required dependencies and the xMsg library.

The main [CLARA Java](https://github.com/JeffersonLab/clara-java) implementation
should also be installed in `$CLARA_HOME`
to run C++ services with the standard orchestrator and the CLARA shell.

### Installation

To build with CMake a configure wrapper script is provided:

    ./configure --prefix="$CLARA_HOME"
    make
    make install

This will install CLARA C++ into the location of your CLARA distribution.


## Authors

For assistance contact authors:

* Vardan Gyurjyan    (<gurjyan@jlab.org>)
* Sebastián Mancilla (<smancill@jlab.org>)
* Ricardo Oyarzún    (<oyarzun@jlab.org>)
