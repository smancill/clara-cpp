FROM debian:stretch-slim AS build

RUN apt-get update && apt-get install -y \
        cmake \
        g++ \
        git \
        make \
        ssh-client \
    && rm -rf /var/lib/apt/lists/*

RUN apt-get update && apt-get install -y \
        libzmq3-dev \
        libprotobuf-dev \
        protobuf-compiler \
    && rm -rf /var/lib/apt/lists/*

RUN mkdir -p ~/.ssh && ssh-keyscan github.com >> ~/.ssh/known_hosts

ENV CLARA_HOME="/usr/local/clara"
ENV PATH="${CLARA_HOME}/bin:${PATH}"
ENV LD_LIBRARY_PATH="${CLARA_HOME}/lib"

RUN mkdir -p /usr/local/src
WORKDIR /usr/local/src

ARG xmsg_rev="master"
RUN git clone https://github.com/JeffersonLab/xmsg-cpp.git \
    && cd xmsg-cpp \
    && git checkout $xmsg_rev \
    && ./configure --prefix=${CLARA_HOME} \
    && make && make install \
    && rm -rf /usr/local/src/xmsg-cpp

ADD . clara-cpp
RUN cd clara-cpp \
    && ./configure --prefix=${CLARA_HOME} \
    && make && make install \
    && rm -rf build

WORKDIR /root
VOLUME ["${CLARA_HOME}/data/input", "${CLARA_HOME}/data/output", "${CLARA_HOME}/log"]

EXPOSE 7781 7782 7783 7785


FROM debian:stretch-slim

RUN apt-get update && apt-get install -y \
        libzmq5 \
        libprotobuf10 \
    && rm -rf /var/lib/apt/lists/*

ENV CLARA_HOME="/usr/local/clara"
ENV PATH="${CLARA_HOME}/bin:${PATH}"
ENV LD_LIBRARY_PATH="${CLARA_HOME}/lib"

COPY --from=build ${CLARA_HOME} ${CLARA_HOME}

WORKDIR /root
VOLUME ["${CLARA_HOME}/data/input", "${CLARA_HOME}/data/output", "${CLARA_HOME}/log"]

EXPOSE 7781 7782 7783 7785
