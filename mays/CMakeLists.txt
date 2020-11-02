add_subdirectory(internal)

target_sources(${PROJECT_NAME}
  INTERFACE
    array_size.h
    average.h
    clamp.h
    divide_round_up.h
    divide_round_nearest.h
    nabs.h
    sign_of.h
)

target_sources(${PROJECT_NAME}_tests
  PRIVATE
    array_size_test.cc
    average_test.cc
    clamp_test.cc
    divide_round_up_test.cc
    divide_round_nearest_test.cc
    nabs_test.cc
    sign_of_test.cc
)
