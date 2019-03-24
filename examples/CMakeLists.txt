set(POT_DEMO pot-demo)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)


# make binary
add_executable( ${POT_DEMO}
        main.cpp
        )

target_link_libraries(${POT_DEMO} PRIVATE ${POT_LIB_NAME} ${POT_EXTRA_LIBS})
