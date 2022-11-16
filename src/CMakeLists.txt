# configure a header file to pass some of the CMake settings to the source code
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/building_config.h.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/building_config.h"
)

set(HEADER_FILES
        container/atom_type_lists.h
        eam_one_way.h
        eam_phi.h
        eam.h
        container/interpolation_lists.hpp
        container/interpolation_object.h
        types.h
        utils.h
        data_structure/array_map.hpp
        parser/funcfl_parser.h
        parser/parser.h
        parser/setfl_parser.h
        pot_table_loader.h
        )

set(SOURCE_FILES
        container/atom_type_lists.cpp
        eam_one_way.cpp
        eam_phi.cpp
        eam.cpp
        types.cpp
        container/interpolation_object.cpp
        parser/funcfl_parser.cpp
        parser/parser.cpp
        parser/setfl_parser.cpp
        pot_table_loader.cpp
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
set(POT_LIB_CMAKE_DIR lib/cmake/${POT_LIB_NAME})
set(POT_LIB_CMAKE_NAMESPACE ${POT_LIB_NAME}::)
set(POT_LIB_CMAKE_CONFIG_IN "${POT_LIB_NAME}-config.cmake.in")
set(POT_LIB_CMAKE_CONFIG_OUT "${CMAKE_CURRENT_BINARY_DIR}/${POT_LIB_NAME}-config.cmake")

# lib alias to makes it possible to always use the namespace version consistently
add_library(${POT_LIB_NAME}::${POT_LIB_NAME} ALIAS ${POT_LIB_NAME})

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
install(EXPORT ${TARGETS_EXPORT_NAME} DESTINATION ${POT_LIB_CMAKE_DIR} NAMESPACE ${POT_LIB_CMAKE_NAMESPACE})
include(CMakePackageConfigHelpers)
configure_package_config_file(
        ${POT_LIB_CMAKE_CONFIG_IN} ${POT_LIB_CMAKE_CONFIG_OUT}
        INSTALL_DESTINATION ${POT_LIB_CMAKE_DIR}
)
install(FILES ${POT_LIB_CMAKE_CONFIG_OUT} DESTINATION ${POT_LIB_CMAKE_DIR})
