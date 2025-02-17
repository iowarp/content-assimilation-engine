FROM ubuntu:latest
LABEL maintainer="hyoklee@hdfgroup.org"
LABEL version="0.0.0"
LABEL description="IOWarp Content Assimilation Engine"

ARG DEBIAN_FRONTEND=noninteractive

SHELL ["/bin/bash", "-c"]
RUN apt update && apt install

RUN apt install -y \
    cmake \
    git \
    python3-pip \
    python3.12-venv

ENV USER="root"
ENV HOME="/root"

RUN git clone https://github.com/iowarp/content-assimilation-engine.git && \
    cd content-assimilation-engine && \
    python3 -m venv /root && \
    /root/bin/pip install -r requirements.txt
