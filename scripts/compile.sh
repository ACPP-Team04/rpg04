#!/usr/bin/env bash
set -euo pipefail

CPP_COMPILER=$1
OS_TARGET=$2


case "${OS_TARGET}" in
  Linux)
    CMAKE_GENERATOR="Unix Makefiles"
    CMAKE_EXTRA_FLAGS='-DCMAKE_CXX_COMPILER=${CPP_COMPILER} -DCMAKE_CXX_FLAGS="--coverage -O0 -g" -DCMAKE_BUILD_TYPE=Debug'
    sudo apt-get update
    sudo apt-get install -y build-essential g++-14 cmake ninja-build python3-pip
    pip install gcovr
    ;;
  Windows)
    CMAKE_GENERATOR="Visual Studio 17 2022"
    CMAKE_EXTRA_FLAGS=""          
    ;;
  *)
    echo "ERROR: Unsupported OS target '${OS_TARGET}'"
    exit 1
    ;;
esac

echo ""
echo "[1/2] Configuring CMake …"
cmake -S . \
      -B build \
      -G "${CMAKE_GENERATOR}" \
      ${CMAKE_EXTRA_FLAGS}

echo ""
echo "[2/2] Building…"
cmake --build build
