# configure a header file to pass some of the CMake settings to the source code
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/building_config.h.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/building_config.h"
)

set(HEADER_FILES
        atom_type_lists.h
        eam_one_way.h
        eam_parser.h
        eam_phi.h
        eam.h
        interpolation_lists.hpp
        interpolation_object.h
        types.h
        utils.h
        data_structure/array_map.hpp
        )

set(SOURCE_FILES
        atom_type_lists.cpp
        eam_one_way.cpp
        eam_parser.cpp
        eam_phi.cpp
        eam.cpp
        interpolation_object.cpp
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
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)

set(TARGETS_EXPORT_NAME ${POT_LIB_NAME}-targets)
set(POT_CMAKE_DIR lib/cmake/pot)

##########################
##### install files
##########################
# install bin and libs.
install(TARGETS ${POT_LIB_NAME}
        EXPORT ${TARGETS_EXPORT_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )

# install header.
install(DIRECTORY "./"
        DESTINATION "include" # todo adding prefxi "pot": fatal error: 'types.h' file not found
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
        )

# install export target and config for find_package
install(EXPORT ${TARGETS_EXPORT_NAME} DESTINATION ${POT_CMAKE_DIR} NAMESPACE pot::)
include(CMakePackageConfigHelpers)
configure_package_config_file(
        "pot-config.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/pot-config.cmake"
        INSTALL_DESTINATION ${POT_CMAKE_DIR}
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/pot-config.cmake" DESTINATION ${POT_CMAKE_DIR})
