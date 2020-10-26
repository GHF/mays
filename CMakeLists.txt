# Required by FetchContent_MakeAvailable
cmake_minimum_required(VERSION 3.14)

# Detect when this library isn't bundled as a subproject
if(NOT DEFINED PROJECT_NAME)
  set(TOP_LEVEL_PROJECT ON)
endif()

project(mays LANGUAGES CXX)

# Build unit tests when built standalone
if(TOP_LEVEL_PROJECT)
  include(FetchContent)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v2.13.2)
  FetchContent_MakeAvailable(Catch2)
  list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)

  set(TEST_NAME ${PROJECT_NAME}_tests)
  add_executable(${TEST_NAME}
    catch2_main.cc
  )
  target_link_libraries(${TEST_NAME}
    PRIVATE
      Catch2::Catch2
  )

  enable_testing()
  include(Catch)
  catch_discover_tests(${TEST_NAME})
endif(TOP_LEVEL_PROJECT)
