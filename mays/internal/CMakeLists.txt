target_sources(${PROJECT_NAME}
  INTERFACE
    check.h
    concepts.h
)

target_sources(${PROJECT_NAME}_tests
  PRIVATE
    check_test.cc
)
