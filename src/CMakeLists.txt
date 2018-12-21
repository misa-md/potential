set(HEADER_FILES
        eam_one_way.h
        eam_parser.h
        eam_phi.h
        eam.h
        interpolation_object.h
        )

set(SOURCE_FILES
        eam_one_way.cpp
        eam_parser.cpp
        eam_phi.cpp
        eam.cpp
        interpolation_object.cpp
        )

# set(MAIN_FILE main.cpp)

# set binary path and lib storage path for compiling process.
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

# make lib
add_library(${POT_LIB_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})
target_include_directories(
        ${POT_LIB_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

# todo import

##########################
##### install files
##########################
# install bin and libs.
install(TARGETS ${M_LIB_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )

# install header.
install(DIRECTORY "./"
        DESTINATION "include"
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
        )