target_sources(${PROJECT_NAME}
  INTERFACE
    check.h
    polyfill.h
    type_traits.h
)

target_sources(${PROJECT_NAME}_tests
  PRIVATE
    check_test.cc
)
