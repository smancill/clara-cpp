# CLARA

A service-oriented framework to provide a computing environment for efficient
Big Data processing.

[![Build Status](https://travis-ci.org/JeffersonLab/clara-cpp.svg?branch=master)](https://travis-ci.org/JeffersonLab/clara-cpp)
[![Javadoc](https://img.shields.io/badge/doxygen-master-blue.svg?style=flat)](https://claraweb.jlab.org/clara/api/cpp/)


## Documentation

The reference documentation is available at <https://claraweb.jlab.org>.


## Build notes

CLARA requires a C++14 compiler and CMake 3.5+

#### Ubuntu 16.04 and 18.04

Install GCC and CMake from the repositories:

    $ sudo apt install build-essential cmake

#### macOS

Install Xcode command line tools:

    $ xcode-select --install

Install CMake using [Homebrew](https://brew.sh/):

    $ brew install cmake

### Dependencies

CLARA is built on top of the [xMsg](https://github.com/JeffersonLab/xmsg-cpp)
pub-sub messaging system, which requires
[Protocol Buffers](https://developers.google.com/protocol-buffers/docs/downloads)
and [ZeroMQ](http://zeromq.org/intro:get-the-software).

xMsg is bundled with CLARA and it will be built automatically.

#### Ubuntu 16.04 and 18.04

Install from the repositories:

    $ sudo apt install libzmq3-dev libprotobuf-dev protobuf-compiler

#### macOS

Use Homebrew:

    $ brew install zeromq protobuf

### Installation

To build with CMake a configure wrapper script is provided:

    $ ./configure --prefix="$CLARA_HOME"
    $ make
    $ make install

This will install CLARA C++ into the location of your CLARA distribution.

The main [CLARA Java](https://github.com/JeffersonLab/clara-java) implementation
should also be already installed in `$CLARA_HOME`
to run C++ services with the standard orchestrator and the CLARA shell.


## Authors

* Vardan Gyurjyan
* Sebastián Mancilla
* Ricardo Oyarzún

For assistance send email to [clara@jlab.org](mailto:clara@jlab.org).
