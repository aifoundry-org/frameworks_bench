FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive

ENV LANG=C.UTF-8 \
    LC_ALL=C.UTF-8

RUN apt-get update && \
    apt-get install -y --no-install-recommends git cmake ninja-build gperf \
    ccache dfu-util device-tree-compiler wget python3-dev python3-venv python3-tk \
    xz-utils file make gcc gcc-multilib g++-multilib libsdl2-dev libmagic1 && \
    rm -rf /var/lib/apt/lists/*

USER 1000:1000
WORKDIR /workspace
