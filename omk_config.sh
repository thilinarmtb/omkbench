#!/bin/bash

: ${OMEM_INSTALL_DIR:=`pwd`/install}
: ${OMEM_CC:=cc}
: ${OMEM_CXX:=CC}

### Don't touch anything that follows this line. ###
OMEM_CURRENT_DIR=`pwd`
OMEM_BUILD_DIR=${OMEM_CURRENT_DIR}/build

mkdir -p ${OMEM_BUILD_DIR} 2> /dev/null

cmake -DCMAKE_C_COMPILER=${OMEM_CC} -DCMAKE_CXX_COMPILER=${OMEM_CXX}  \
  -DCMAKE_INSTALL_PREFIX=${OMEM_INSTALL_DIR} \
  -B ${OMEM_BUILD_DIR} -S ${OMEM_CURRENT_DIR}
  
cmake --build ${OMEM_BUILD_DIR} --target install -j10
