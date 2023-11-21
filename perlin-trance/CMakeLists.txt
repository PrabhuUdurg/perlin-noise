cmake_minimum_required(VERSION 3.24)
project(perlin_trance)

set(CMAKE_CXX_STANDARD 14)

find_package(glfw3 REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})

include_directories(glad/include)
include_directories(glad/include/glad)
include_directories(glad/include/KHR)

add_executable(perlin_trance
        glad/include/glad/glad.h
        glad/include/KHR/khrplatform.h
        glad/src/glad.c
        perlin-noise.cpp)

target_link_libraries(perlin_trance glfw)

