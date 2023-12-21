set(POT_DEMO pot-demo)
set(POT_DEMO_C pot-demo_c)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)


# make binary
add_executable( ${POT_DEMO}
        main.cpp
        )

target_link_libraries(${POT_DEMO} PRIVATE ${POT_LIB_NAME} ${POT_EXTRA_LIBS})

add_executable(${POT_DEMO_C}
        main.c
)

target_link_libraries(${POT_DEMO_C} PRIVATE ${POT_LIB_NAME} ${POT_EXTRA_LIBS})
