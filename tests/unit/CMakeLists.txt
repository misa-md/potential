# configure a header file to pass some of the CMake settings to the source code
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/test_config.h.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/test_config.h"
)

set(TEST_HEADER_FILES
        test_config.h
        )

set(TEST_SOURCE_FILES
        main.cpp
        data_structure/array_map_test.cpp
        atom_type_lists_test.cpp
        interpolation_lists_sync_test.cpp
        )

# set binary path and lib storage path wile compiling process.
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_executable(${POT_UINT_TEST_NAME} ${TEST_SOURCE_FILES} ${TEST_HEADER_FILES})

# Extra linking for the project.
# Extra linking for the project.
target_link_libraries(${POT_UINT_TEST_NAME}
        PRIVATE GTest::gtest GTest::gtest_main ${POT_LIB_NAME} ${POT_EXTRA_LIBS}
        )
# This is so you can do 'make test' to see all your tests run, instead of
# manually running the executable runUnitTests to see those specific tests.
add_test(NAME unit-test COMMAND ${POT_UINT_TEST_NAME})


##########################
##### install files
##########################
install(TARGETS ${POT_UINT_TEST_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )
