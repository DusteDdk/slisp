FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    clang-19 \
    libclang-19-dev \
    libclang-common-19-dev \
    libclang-cpp19 \
    libclang-cpp19-dev \
    libc++-19-dev \
    libasan8 \
    libsdl2-2.0-0 \
    libsdl2-dev \
    catch2 \
    valgrind \
    git \
    cmake \
    libstdc++-14-dev \
    libc++abi1-19:amd64 \
    libc++abi-19-dev:amd64 \
    locales \
    && locale-gen en_US.UTF-8
ENV LANG=en_US.UTF-8
ENV LC_ALL=en_US.UTF-8

USER ubuntu
WORKDIR /home/ubuntu
