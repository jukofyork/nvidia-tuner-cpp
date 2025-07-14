cmake_minimum_required(VERSION 3.16)
project(nvidia-tuner)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find NVIDIA ML library manually
find_library(NVML_LIBRARY
    NAMES nvidia-ml libnvidia-ml.so.1
    PATHS 
        /usr/lib/x86_64-linux-gnu
        /usr/lib64
        /usr/local/cuda/lib64
        /usr/local/cuda/lib
        /opt/cuda/lib64
        /opt/cuda/lib
    DOC "NVIDIA Management Library"
)

find_path(NVML_INCLUDE_DIR
    NAMES nvml.h
    PATHS
        /usr/include
        /usr/local/cuda/include
        /opt/cuda/include
        /usr/include/nvidia/gdk
    DOC "NVIDIA Management Library headers"
)

if(NOT NVML_LIBRARY)
    message(FATAL_ERROR "NVIDIA ML library not found. Please install nvidia drivers and libnvidia-ml-dev")
endif()

if(NOT NVML_INCLUDE_DIR)
    message(FATAL_ERROR "NVIDIA ML headers not found. Please install libnvidia-ml-dev")
endif()

message(STATUS "Found NVML library: ${NVML_LIBRARY}")
message(STATUS "Found NVML include dir: ${NVML_INCLUDE_DIR}")

add_executable(nvidia-tuner
    src/main.cpp
    src/cli.cpp
    src/gpu_device.cpp
    src/temperature_controller.cpp
    src/utils.cpp
)

target_include_directories(nvidia-tuner PRIVATE 
    ${NVML_INCLUDE_DIR}
    src
)

target_link_libraries(nvidia-tuner 
    ${NVML_LIBRARY}
    pthread
    dl
)

# Release build optimizations
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(nvidia-tuner PRIVATE -O3 -flto)
    target_link_options(nvidia-tuner PRIVATE -flto)
endif()