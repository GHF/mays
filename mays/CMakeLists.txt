add_subdirectory(internal)

target_sources(${PROJECT_NAME}
  INTERFACE
    nabs.h
)

target_sources(${PROJECT_NAME}_tests
  PRIVATE
    nabs_test.cc
)
