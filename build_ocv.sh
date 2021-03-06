#!/bin/bash -e

# Script to build opencv lib on Windows Host
# To run this script use git-bash


myRepo=$(pwd)

#CMAKE_GENERATOR_OPTIONS=-G"Visual Studio 16 2019"
#CMAKE_GENERATOR_OPTIONS=-G"Visual Studio 15 2017 Win64"
CMAKE_GENERATOR_OPTIONS=(-G"Visual Studio 16 2019" -A x64)  # CMake 3.14+ is required
RepoSource=opencv

mkdir -p build_opencv
pushd build_opencv
CMAKE_OPTIONS=(-DBUILD_PERF_TESTS:BOOL=OFF \
-DBUILD_TESTS:BOOL=OFF \
-DWITH_QT:BOOL=ON \
-DWITH_TIFF:BOOL=OFF \
-DWITH_QUIRC:BOOL=OFF \
-DWITH_PROTOBUF:BOOL=OFF \
-DWITH_IMGCODEC_PFM:BOOL=OFF \
-DWITH_JASPER:BOOL=OFF \
-DWITH_MSMF:BOOL=OFF \
-DWITH_OPENGL:BOOL=ON \
-DBUILD_DOCS:BOOL=OFF \
-DWITH_CUDA:BOOL=OFF \
-DBUILD_EXAMPLES:BOOL=OFF \
-DINSTALL_CREATE_DISTRIB=ON)

set -x
cmake "${CMAKE_GENERATOR_OPTIONS[@]}" "${CMAKE_OPTIONS[@]}" -DBUILD_LIST="imgcodecs,highgui,videoio" \
-DCMAKE_INSTALL_PREFIX="$myRepo/install/$RepoSource" \
-DQt5_DIR="c:/Qt/5.15.2/msvc2019_64/lib/cmake/Qt5" \
"$myRepo/$RepoSource"
echo "************************* $Source_DIR -->debug"
cmake --build .  --config debug -j4
echo "************************* $Source_DIR -->release"
cmake --build .  --config release -j4
cmake --build .  --target install --config release
cmake --build .  --target install --config debug
popd