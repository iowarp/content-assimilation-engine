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
          curl \
          g++ \
          git \
          libxml2-dev \
          make \
          openssl \
          openssl-dev \
          procps \
          python3-dev \
          python3-pip \
          python3.12-venv \
          sudo \
          uuid-dev

ENV USER="root"
ENV HOME="/root"

RUN git clone https://github.com/iowarp/content-assimilation-engine.git && \
    cd content-assimilation-engine && \
    python3 -m venv /root && \
    /root/bin/pip install -r requirements.txt
