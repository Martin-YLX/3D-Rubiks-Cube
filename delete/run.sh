#!/bin/bash

# 获取脚本所在目录
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

# 进入build目录
cd "$SCRIPT_DIR/build"

# 运行程序
./RubiksCube

# 返回原目录
cd "$SCRIPT_DIR" 
