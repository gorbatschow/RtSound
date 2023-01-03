cmake_minimum_required(VERSION 3.16.0)

project(RtSoundIO)

set(CXX_CMAKE_STANDARD 20)
set(CXX_CMAKE_STANDARD_REQUIRED ON)

set(RTSOUNDIO_SOURCES
    RtSoundIO.cpp
    RtSoundProvider.cpp
    RtSoundBaseGen.cpp
    RtSoundToneGen.cpp
    RtSoundNoiseGen.cpp
)

add_library(${PROJECT_NAME} ${RTSOUNDIO_SOURCES})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_include_directories(${PROJECT_NAME} PUBLIC .)
target_link_libraries(${PROJECT_NAME} PUBLIC rtaudio)
