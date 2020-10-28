add_subdirectory(internal)

target_sources(${PROJECT_NAME}
  INTERFACE
    average.h
    nabs.h
)

target_sources(${PROJECT_NAME}_tests
  PRIVATE
    average_test.cc
    nabs_test.cc
)
