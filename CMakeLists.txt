cmake_minimum_required(VERSION 3.16.0)

project(rtsound)

set(CXX_CMAKE_STANDARD 20)
set(CXX_CMAKE_STANDARD_REQUIRED ON)

set(RTSOUND_SRC
    src/RtSoundIO.cpp
)

add_library(${PROJECT_NAME} ${RTSOUND_SRC})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_include_directories(${PROJECT_NAME} PUBLIC .)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC rtaudio)
