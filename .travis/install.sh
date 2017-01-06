#!/bin/bash

set -eu

############################################################################
# Install xMsg (using its own Travis install script)
############################################################################

XMSG_REPO=https://github.com/JeffersonLab/xmsg-cpp.git
XMSG_DIR="${DEPS_DIR}"/xmsg-cpp

git clone "${XMSG_REPO}" "${XMSG_DIR}"
"${XMSG_DIR}"/.travis/install.sh

cd "${XMSG_DIR}"
./configure --build-type="${BUILD_TYPE}" --prefix="${INSTALL_DIR}"
make -j"${JOBS}"
make install
