#!/bin/bash

set -o errexit
set -o nounset
IFS=$'\n\t'

BUILD_DIRECTORY=_build

rm -rf "${BUILD_DIRECTORY}"
mkdir "${BUILD_DIRECTORY}"
pushd "${BUILD_DIRECTORY}"

cmake ..
make

popd

"${BUILD_DIRECTORY}/mazecast"
