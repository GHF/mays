# vim: et:sw=2:ts=2:tw=100
# Required by FetchContent_MakeAvailable
cmake_minimum_required(VERSION 3.14)

# Detect when this library isn't bundled as a subproject
if(NOT DEFINED PROJECT_NAME)
  set(TOP_LEVEL_PROJECT ON)
endif()

project(mays LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

# Build unit tests when built standalone
if(TOP_LEVEL_PROJECT)
  option(TEST_WITH_CXX23 "Build unit tests under C++23" OFF)

  option(CODE_COVERAGE "Enable coverage reporting (Clang only)" OFF)
  if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "(Apple)?Clang")
    set(code_cov_options -fprofile-instr-generate -fcoverage-mapping)
  endif()

  include(FetchContent)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.4.0)
  SET(CATCH_BUILD_STATIC_LIBRARY ON CACHE BOOL "Build Catch2 main")
  FetchContent_MakeAvailable(Catch2)

  # Make include directories be included as system headers to avoid clang-tidy diagnostics
  get_target_property(CATCH2_INC Catch2 INTERFACE_INCLUDE_DIRECTORIES)
  set_target_properties(Catch2 PROPERTIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${CATCH2_INC}")
  list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)

  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  set(TEST_NAME ${PROJECT_NAME}_tests)
  add_executable(${TEST_NAME})
  target_link_libraries(${TEST_NAME}
    PRIVATE
      mays
      Catch2::Catch2WithMain
  )
  if(TEST_WITH_CXX23)
    target_compile_features(${TEST_NAME} PRIVATE cxx_std_23)
  endif()
  target_compile_definitions(${TEST_NAME}
    PRIVATE
      $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:LLVM_ENABLE_RUNTIMES=libunwind>
      $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:LIBCXX_USE_COMPILER_RT=YES>
      $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:LIBCXXABI_USE_COMPILER_RT=YES>
      $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:LIBUNWIND_USE_COMPILER_RT=YES>
  )
  target_compile_options(${TEST_NAME}
    PRIVATE
      # Lots of warnings that are each fatal
      $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Werror;-Wconversion;-Wall;-Wextra;-pedantic;>
      # Force color diagnostics output
      $<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>
      $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-fcolor-diagnostics>
      ${code_cov_options}
  )
  target_link_options(${TEST_NAME}
    PRIVATE
      $<$<OR:$<LINK_LANG_AND_ID:CXX,Clang>,$<LINK_LANG_AND_ID:CXX,AppleClang>>:-stdlib=libc++>
      $<$<OR:$<LINK_LANG_AND_ID:CXX,Clang>,$<LINK_LANG_AND_ID:CXX,AppleClang>>:-rtlib=compiler-rt>
      ${code_cov_options}
  )

  enable_testing()
  include(Catch)
  catch_discover_tests(${TEST_NAME})
endif(TOP_LEVEL_PROJECT)

add_subdirectory(mays)
