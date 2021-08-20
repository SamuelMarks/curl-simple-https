get_filename_component(EXEC_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
string(REPLACE " " "_" EXEC_NAME "${EXEC_NAME}")

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

include(GenerateExportHeader)

set(Header_Files "cli.h")
source_group("Header Files" FILES "${Header_Files}")

set(Source_Files "cli.c")
source_group("Source Files" FILES "${Source_Files}" "main.c")

############################
# Library target formation #
############################

set(LIBRARY_NAME "${EXEC_NAME}_lib")

add_library("${LIBRARY_NAME}" "${Header_Files}" "${Source_Files}")

target_link_libraries("${LIBRARY_NAME}" INTERFACE "${PROJECT_LOWER_NAME}_compiler_flags")
target_link_libraries("${LIBRARY_NAME}" PUBLIC "libcurl_simple_https")

set_target_properties(
        "${LIBRARY_NAME}"
        PROPERTIES
        LINKER_LANGUAGE
        C
)

###############################
# Executable target formation #
###############################

add_executable("${EXEC_NAME}" "main.c")

target_link_libraries("${EXEC_NAME}" INTERFACE "${PROJECT_LOWER_NAME}_compiler_flags")

target_link_libraries("${EXEC_NAME}" PRIVATE "${LIBRARY_NAME}")
target_include_directories(
        "${LIBRARY_NAME}"
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}>"
        "$<INSTALL_INTERFACE:include>"
)

set_target_properties(
        "${EXEC_NAME}"
        PROPERTIES
        LINKER_LANGUAGE
        C
)

#######################################
# Export / installation configuration #
#######################################

set(_export_file "${CMAKE_CURRENT_SOURCE_DIR}/${LIBRARY_NAME}_export.h")
generate_export_header("${LIBRARY_NAME}" EXPORT_FILE_NAME "${_export_file}")

# setup the version numbering
set_property(TARGET "${EXEC_NAME}" PROPERTY VERSION "1.0.0")
set_property(TARGET "${EXEC_NAME}" PROPERTY SOVERSION "1")

# install rules
set(installable_libs "${EXEC_NAME}" "${PROJECT_LOWER_NAME}_compiler_flags")
if (TARGET "${DEPENDANT_LIBRARY}")
    list(APPEND installable_libs "${DEPENDANT_LIBRARY}")
endif ()
install(TARGETS ${installable_libs}
        DESTINATION "bin"
        EXPORT "${EXEC_NAME}Targets")
