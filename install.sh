#!/bin/bash
case $1 in
    "")
	PREFIX="dminstall"
	;;
    *)
	PREFIX="$1"
	;;
esac

rm -rf build/*
cmake -S . -B build -DBUILD_SHARED_LIBS=YES -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --install build --prefix "$PREFIX"
