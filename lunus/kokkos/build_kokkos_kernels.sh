#!/bin/bash 

if [ -z ${KOKKOS_PATH+x} ]; then
  echo "Unable to build kokkos-kernels without setting KOKKOS_PATH. Aborting." 
else
  export Kokkos_DIR=${KOKKOS_PATH}
fi

KOKKOS_KERNELS_INSTALL_DIR=${PWD}/kokkos-kernels-install

pushd kokkos-kernels

if [[ -d build ]]; then
  rm -rf build
fi

mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=${KOKKOS_KERNELS_INSTALL_DIR}
make -j$( nproc )
make install
