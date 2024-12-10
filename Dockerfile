FROM ubuntu:latest
LABEL maintainer="hyoklee@hdfgroup.org"
LABEL version="0.0.0"
LABEL description="IOWarp Content Assimilation Engine"

ARG DEBIAN_FRONTEND=noninteractive

SHELL ["/bin/bash", "-c"]
RUN apt update && apt install

RUN apt install -y \
    bzip2 \
    gcc \
    g++ \
    gfortran \
    git \
    python3 \
    xz-utils

ENV USER="root"
ENV HOME="/root"
ENV SPACK_DIR="${HOME}/spack"
ENV HERMES_DEPS_DIR="${HOME}/hermes_deps"
ENV HERMES_DIR="${HOME}/hermes"

RUN git clone https://github.com/spack/spack ${SPACK_DIR} && \
    . "${SPACK_DIR}/share/spack/setup-env.sh" && \
    spack external find && \
    spack install hermes@master+vfd+mpiio^mpich

RUN git clone https://github.com/grc-iit/jarvis-cd.git && \
    cd jarvis-cd && \
    pip install -e . -r requirements.txt

RUN git clone https://github.com/grc-iit/scspkg.git && \
    cd scspkg && \
    pip install -e . -r requirements.txt
