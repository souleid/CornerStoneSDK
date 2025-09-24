# Copyright 2025 souleid
cmake_minimum_required(VERSION 3.22)
project(cornerstone_static CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT MSVC)
  add_compile_options(-fPIC -fvisibility=hidden)
endif()

# NDK r25 환경에서 platform 미전파 시 안전장치
if(ANDROID AND NOT DEFINED ANDROID_PLATFORM)
  # build_android.py에서 넘겨주지만, 혹시 몰라 기본값 세팅
  set(ANDROID_PLATFORM android-23)
endif()

set(CS_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
set(CS_SRC_DIR "${CS_ROOT_DIR}/src")

include_directories("${CS_ROOT_DIR}")

file(GLOB_RECURSE CS_SOURCES
  "${CS_SRC_DIR}/*.cc"
  "${CS_SRC_DIR}/*.cpp"
)

# 예: main.cpp 제외
list(REMOVE_ITEM CS_SOURCES "${CS_SRC_DIR}/main.cpp")

if(NOT CS_SOURCES)
  message(FATAL_ERROR "No sources found in: ${CS_SRC_DIR}")
endif()

add_library(cornerstone_static STATIC ${CS_SOURCES})
set_target_properties(cornerstone_static PROPERTIES OUTPUT_NAME "cornerstone")

message(STATUS "CornerStoneSDK Android static build")
message(STATUS "Sources : ${CS_SOURCES}")
