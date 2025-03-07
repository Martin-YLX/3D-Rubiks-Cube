cmake_minimum_required(VERSION 3.10)
project(3DCube)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/glad
        ${PROJECT_SOURCE_DIR}/include/glfw
        ${PROJECT_SOURCE_DIR}/include/glm
)

set(SOURCES
        src/main.cpp
        src/Cube.cpp
        src/Camera.cpp
        src/Shader.cpp
        src/glad.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} glfw OpenGL::GL glm::glm)

# 将着色器文件复制到构建目录，确保运行时可加载
file(COPY ${CMAKE_SOURCE_DIR}/src/shader.vert DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/src/shader.frag DESTINATION ${CMAKE_BINARY_DIR})