if (NOT (POT_MPI_ENABLE_FLAG))
    set(POT_TEST_MPI_ENABLE_FLAG OFF)  # if we disabled POT_MPI_ENABLE_FLAG, we cann't also use POT_TEST_MPI_ENABLE_FLAG.
endif ()

MESSAGE(STATUS "MPI in test: ${POT_TEST_MPI_ENABLE_FLAG}")

add_subdirectory(unit)
