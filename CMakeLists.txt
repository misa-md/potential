cmake_minimum_required(VERSION 3.0)
include(config.cmake)

project(${AppName})

include(config.cmake)
include(dependency.cmake)

add_subdirectory(src)

add_subdirectory(examples)

##############################
## test configure
##############################
if (TEST_BUILD_ENABLE_FLAG)
    MESSAGE(STATUS "TEST is enabled")
    enable_testing()
    add_subdirectory(tests)
    add_subdirectory(benchmarks)
endif ()
