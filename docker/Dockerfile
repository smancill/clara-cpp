# SPDX-FileCopyrightText: © The Clara Framework Authors
#
# SPDX-License-Identifier: MIT-0

FROM debian:bullseye-slim AS build

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install --assume-yes apt-utils

# Useful packages
RUN apt-get update && apt-get install -y \
        ca-certificates \
        curl \
        procps \
        wget \
    && rm -rf /var/lib/apt/lists/*

# Build tools
RUN apt-get update && apt-get install -y \
        cmake \
        g++ \
        make \
    && rm -rf /var/lib/apt/lists/*

# Build dependencies
RUN apt-get update && apt-get install -y \
        libzmq3-dev \
        libprotobuf-dev \
        protobuf-compiler \
    && rm -rf /var/lib/apt/lists/*

ENV CLARA_HOME="/usr/local/clara"
ENV PATH="${CLARA_HOME}/bin:${PATH}"
ENV LD_LIBRARY_PATH="${CLARA_HOME}/lib"

RUN mkdir -p /usr/local/src
WORKDIR /usr/local/src

# Build Clara
ADD . clara-cpp
RUN cd clara-cpp \
    && ./configure --prefix=${CLARA_HOME} --disable-tests \
    && make && make install \
    && rm -rf build

WORKDIR /root
VOLUME ["${CLARA_HOME}/data/input", "${CLARA_HOME}/data/output", "${CLARA_HOME}/log"]

EXPOSE 7781-7785


FROM debian:bullseye-slim

# Useful packages
RUN apt-get update && apt-get install -y \
        procps \
    && rm -rf /var/lib/apt/lists/*

# Run dependencies
RUN apt-get update && apt-get install -y \
        libzmq5 \
        libprotobuf23 \
    && rm -rf /var/lib/apt/lists/*

ENV CLARA_HOME="/usr/local/clara"
ENV PATH="${CLARA_HOME}/bin:${PATH}"
ENV LD_LIBRARY_PATH="${CLARA_HOME}/lib"

COPY --from=build ${CLARA_HOME} ${CLARA_HOME}

WORKDIR /root
VOLUME ["${CLARA_HOME}/data/input", "${CLARA_HOME}/data/output", "${CLARA_HOME}/log"]

EXPOSE 7781-7785
