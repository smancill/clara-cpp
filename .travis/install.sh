#!/bin/bash

set -eu

############################################################################
# Install xMsg (using its own Travis install script)
############################################################################

XMSG_REPO=https://github.com/JeffersonLab/xmsg-cpp.git
XMSG_DIR="${TRAVIS_BUILD_DIR}"/deps/xmsg-cpp

git clone --depth 1 "${XMSG_REPO}" "${XMSG_DIR}"
"${XMSG_DIR}"/.travis/install.sh

cd "${XMSG_DIR}"
./configure --prefix="${INSTALL_DIR}" --build-type="${BUILD_TYPE}" --disable-tests
make
make install
