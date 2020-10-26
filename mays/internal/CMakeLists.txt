target_sources(${PROJECT_NAME}
  INTERFACE
    check.h
)

target_sources(${PROJECT_NAME}_tests
  PRIVATE
    check_test.cc
)
