# CLARA

A service-oriented framework to provide a computing environment for efficient
Big Data processing.


## Build notes

CLARA requires a C++17 compiler and CMake 3.16+

#### Ubuntu 20.04

Install GCC and CMake from the repositories:

    $ sudo apt install build-essential cmake

#### macOS

Install Xcode command line tools:

    $ xcode-select --install

Install CMake using [Homebrew](https://brew.sh/):

    $ brew install cmake

### Dependencies

CLARA uses an internal pub-sub messaging library, which requires
[Protocol Buffers](https://developers.google.com/protocol-buffers/docs/downloads)
and [ZeroMQ](http://zeromq.org/intro:get-the-software).

#### Ubuntu 20.04

Install from the repositories:

    $ sudo apt install libzmq5-dev libprotobuf-dev protobuf-compiler

#### macOS

Use Homebrew:

    $ brew install zeromq protobuf

### Installation

To build with CMake a configure wrapper script is provided:

    $ ./configure --prefix="$CLARA_HOME"
    $ make
    $ make install

This will install CLARA C++ into the location of your CLARA distribution.

The main [CLARA Java](https://github.com/smancill/clara-java) implementation
should also be already installed in `$CLARA_HOME`
to run C++ services with the standard orchestrator and the CLARA shell.


## Authors

* Vardan Gyurjyan
* Sebastián Mancilla
* Ricardo Oyarzún
