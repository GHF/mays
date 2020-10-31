add_subdirectory(internal)

target_sources(${PROJECT_NAME}
  INTERFACE
    array_size.h
    average.h
    nabs.h
    sign_of.h
)

target_sources(${PROJECT_NAME}_tests
  PRIVATE
    array_size_test.cc
    average_test.cc
    nabs_test.cc
    sign_of_test.cc
)
