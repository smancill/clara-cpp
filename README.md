# CLARA

A service-oriented framework to provide a computing environment for efficient
Big Data processing.

## Build notes

#### Dependencies

CLARA requires a C++14 compiler and the [xMsg](https://github.com/JeffersonLab/xmsg-cpp)
pub-sub messaging system. Follow the project instructions to install the build
toolchain, the required dependencies and the xMsg library.

### Installation

To build with CMake a configure wrapper script is provided:

    ./configure --prefix=<INSTALL_DIR>
    make
    make install


## Authors

For assistance contact authors:

* Vardan Gyurjyan    (<gurjyan@jlab.org>)
* Sebastián Mancilla (<smancill@jlab.org>)
* Ricardo Oyarzún    (<oyarzun@jlab.org>)
