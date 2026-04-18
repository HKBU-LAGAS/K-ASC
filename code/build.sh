#!/bin/bash
echo "--- Configuring with CMake... ---"
cmake .
make clean all -j16
