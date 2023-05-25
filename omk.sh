#!/bin/bash

: ${OMK_INSTALL_DIR:=`pwd`/install}
: ${OMK_CC:=cc}
: ${OMK_CXX:=CC}

### Don't touch anything that follows this line. ###
OMK_CURRENT_DIR=`pwd`
OMK_BUILD_DIR=${OMK_CURRENT_DIR}/build

mkdir -p ${OMK_BUILD_DIR} 2> /dev/null

cmake -DCMAKE_C_COMPILER=${OMK_CC} -DCMAKE_CXX_COMPILER=${OMK_CXX}  \
  -DCMAKE_INSTALL_PREFIX=${OMK_INSTALL_DIR} \
  -B ${OMK_BUILD_DIR} -S ${OMK_CURRENT_DIR}
  
cmake --build ${OMK_BUILD_DIR} --target install -j10
