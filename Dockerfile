FROM ubuntu:22.04 AS builder

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive \
    apt-get install --assume-yes --no-install-recommends \
    cmake \
    libboost-all-dev \
    sqlite3

WORKDIR /usr/src/dminstall

COPY include include/
COPY src src/
RUN mkdir lib
RUN mkdir build && cd $_

RUN cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTS=OFF \
    -DBUILD_SHARED_LIBS=OFF \
    && cmake --build .

# Need to figure out where this would store/deploy anything