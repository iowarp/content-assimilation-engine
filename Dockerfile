FROM ubuntu:latest
LABEL maintainer="hyoklee@hdfgroup.org"
LABEL version="0.0.0"
LABEL description="IOWarp Content Assimilation Engine"

ARG DEBIAN_FRONTEND=noninteractive

SHELL ["/bin/bash", "-c"]
RUN apt update && apt install

RUN apt install -y \
          bash \
          cmake \
          ceph-dev \
          curl \
          curl-dev \
          fuse-dev \
          fuse3-dev \
          g++ \
          git \
          gtest-dev \
          isa-l-dev \
          json-c-dev \
          krb5-dev \
          libxml2-dev \
          linux-headers \
          make \
          openssl \
          openssl-dev \
          procps \
          py3-pip \
          py3-setuptools \
          py3-wheel \
          python3-dev \
          python3-pip \
          python3.12-venv \
          readline-dev \
          sudo \
          tinyxml-dev \
          util-linux-dev \
          uuidgen \
          zlib-dev

ENV USER="root"
ENV HOME="/root"

RUN git clone https://github.com/iowarp/content-assimilation-engine.git && \
    cd content-assimilation-engine && \
    python3 -m venv /root && \
    /root/bin/pip install -r requirements.txt
