#!/bin/bash
# 确保已经加载了模块
#module load gcc/11.2.0
#g++ --version
# 在运行cmake之前，强制删除所有CMake缓存和生成的Makefile
#echo "--- Cleaning CMake cache and old Makefiles ---"
#rm -rf CMakeCache.txt CMakeFiles/ Makefile cmake_install.cmake
# 现在运行cmake，它会因为找不到缓存而重新检测编译器
echo "--- Configuring with CMake... ---"
cmake .
make clean all -j16
