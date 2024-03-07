#!/bin/sh
if [ -d "CMakeFiles" ];then rm -rf CMakeFiles; fi
if [ -f "Makefile" ];then rm -f Makefile; fi
if [ -f "cmake_install.cmake" ];then rm -f cmake_install.cmake; fi
if [ -f "CMakeCache.txt" ];then rm -f CMakeCache.txt; fi

MCUXDIR=/data/cadeniyi/sdk

cmake -DSdkRootDirPath=$MCUXDIR -DCMAKE_C_FLAGS="-DIMX8MM" -DCMAKE_TOOLCHAIN_FILE="$MCUXDIR/tools/cmake_toolchain_files/armgcc.cmake" -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=release  .
make  -j 2>&1 | tee build_log.txt
