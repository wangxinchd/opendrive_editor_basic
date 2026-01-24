#!/bin/bash

# 判断 build 是否存在
if [ -d "build" ]; then
    rm -rf build
fi

mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
cd ..

echo "Build completed"