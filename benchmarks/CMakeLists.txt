# set binary path and lib storage path wile compiling process.
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set(BENCHMARK_TARGET pot_benchmark)
add_executable(${BENCHMARK_TARGET} main.cpp)

if (TARGET benchmark::benchmark AND TARGET benchmark::benchmark_main)
    target_link_libraries(${BENCHMARK_TARGET}
            PRIVATE benchmark::benchmark benchmark::benchmark_main ${POT_LIB_NAME} ${POT_EXTRA_LIBS})
else ()
    target_link_libraries(${BENCHMARK_TARGET}
            PRIVATE benchmark benchmark_main  ${POT_LIB_NAME} ${POT_EXTRA_LIBS})
endif ()
