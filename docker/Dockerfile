FROM ubuntu:latest
LABEL maintainer="hyoklee@hdfgroup.org"
LABEL version="0.0.0"
LABEL description="IOWarp Content Assimilation Engine"

ARG DEBIAN_FRONTEND=noninteractive

SHELL ["/bin/bash", "-c"]
RUN apt update --fix-missing && apt install

RUN apt install -y \
          bash \
          cmake \
          curl \
          g++ \
          git \
          libcurl4-openssl-dev \
          libreadline-dev \
          libssl-dev \
          libsystemd-dev \
          libtinyxml2-dev \
          libxml2-dev \
          make \
          openssl \
          procps \
          python3-dev \
          python3-pip \
          python3.12-venv \
          sudo \
          uuid-dev \
          xrootd-client xrootd-server python3-xrootd

ENV USER="root"
ENV HOME="/root"
COPY . .
RUN python3 -m venv /root && \
    /root/bin/pip install -r requirements.txt
