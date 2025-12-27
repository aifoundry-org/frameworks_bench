FROM ubuntu:22.04

ARG DEBIAN_FRONTEND=noninteractive

ENV LANG=C.UTF-8 \
    LC_ALL=C.UTF-8

RUN adduser --disabled-password --gecos '' ubuntu -u 1000

RUN apt-get update && \
    apt-get install -y --no-install-recommends git cmake ninja-build gperf python3-pip\
    ccache dfu-util device-tree-compiler wget python3-dev python3-venv python3-tk \
    xz-utils file make gcc gcc-multilib g++-multilib libsdl2-dev libmagic1 && \
    rm -rf /var/lib/apt/lists/*

RUN usermod -aG sudo ubuntu

USER ubuntu
WORKDIR /workspace
