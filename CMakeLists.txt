# Required by FetchContent_MakeAvailable
cmake_minimum_required(VERSION 3.14)

# Detect when this library isn't bundled as a subproject
if(NOT DEFINED PROJECT_NAME)
  set(TOP_LEVEL_PROJECT ON)
endif()

project(mays LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

# Build unit tests when built standalone
if(TOP_LEVEL_PROJECT)
  include(FetchContent)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v2.13.3)
  FetchContent_MakeAvailable(Catch2)
  list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)

  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  set(TEST_NAME ${PROJECT_NAME}_tests)
  add_executable(${TEST_NAME}
    catch2_main.cc
  )
  target_link_libraries(${TEST_NAME}
    PRIVATE
      mays
      Catch2::Catch2
  )
  target_compile_options(${TEST_NAME}
    PRIVATE
      # Lots of warnings that are each fatal
      $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Werror;-Wconversion;-Wall;-Wextra;-pedantic;>
      # Force color diagnostics output
      $<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>
      $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-fcolor-diagnostics>
  )

  enable_testing()
  include(Catch)
  catch_discover_tests(${TEST_NAME})
endif(TOP_LEVEL_PROJECT)

add_subdirectory(mays)
