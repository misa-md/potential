# configure a header file to pass some of the CMake settings to the source code
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/building_config.h.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/building_config.h"
)

set(HEADER_FILES
        atom_type_lists.h
        eam_one_way.h
        eam_phi.h
        eam.h
        interpolation_lists.hpp
        interpolation_object.h
        types.h
        utils.h
        data_structure/array_map.hpp
        parser/parser.h
        parser/setfl_parser.h
        )

set(SOURCE_FILES
        atom_type_lists.cpp
        eam_one_way.cpp
        eam_phi.cpp
        eam.cpp
        interpolation_object.cpp
        parser/setfl_parser.cpp
        parser/parser.cpp
        types.cpp
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