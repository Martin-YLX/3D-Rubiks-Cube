#!/bin/bash

# 清理旧的构建文件
rm -rf build
mkdir -p build

# 进入build目录
cd build

# 运行CMake配置
cmake ..

# 编译项目
make

# 返回原目录
cd .. 
