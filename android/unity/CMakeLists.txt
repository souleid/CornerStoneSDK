# Copyright 2025 souleid
cmake_minimum_required(VERSION 3.22)
project(cornerstone_static CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT MSVC)
  add_compile_options(-fPIC -fvisibility=hidden)
endif()

if(ANDROID AND NOT DEFINED ANDROID_PLATFORM)
  set(ANDROID_PLATFORM android-23)
endif()

set(CS_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../..")
set(CS_SRC_DIR "${CS_ROOT_DIR}/src")

include_directories("${CS_ROOT_DIR}")

file(GLOB_RECURSE CS_SOURCES
  "${CS_SRC_DIR}/*.cc"
  "${CS_SRC_DIR}/*.cpp"
  "${CS_ROOT_DIR}/unity/*.cpp"
)

list(REMOVE_ITEM CS_SOURCES "${CS_SRC_DIR}/main.cpp")

if(NOT CS_SOURCES)
  message(FATAL_ERROR "No sources found in: ${CS_SRC_DIR}")
endif()

add_library(CornerStoneSDK SHARED ${CS_SOURCES})

# Optimize per config.
target_compile_options(CornerStoneSDK PRIVATE
  $<$<CONFIG:Release>:-O2>
  $<$<CONFIG:RelWithDebInfo>:-O2 -g>
  $<$<CONFIG:Debug>:-O0 -g>
)

# Prefer static STL on Android for single-binary deploy.
set_target_properties(CornerStoneSDK PROPERTIES
  OUTPUT_NAME "CornerStoneSDK"
)

# Linker options (static libstdc++ if you prefer).
target_link_options(CornerStoneSDK PRIVATE -static-libstdc++)