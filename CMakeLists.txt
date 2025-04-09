cmake_minimum_required(VERSION 3.10)
project(RubiksCube)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置源代码目录
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# 添加编译定义
add_definitions(-DSRC_DIR="${SRC_DIR}")

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# 添加GLAD源文件
add_library(glad STATIC src/glad.c)

# 包含GLAD头文件目录
include_directories(include)

add_executable(RubiksCube main.cpp)

target_link_libraries(RubiksCube
    ${OPENGL_LIBRARIES}
    glfw
    glad
) 