get_filename_component(EXEC_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

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

include(GNUInstallDirs)
target_include_directories(
        "${LIBRARY_NAME}"
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/${PROJECT_NAME}>"
        "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries("${LIBRARY_NAME}" INTERFACE "${PROJECT_NAME}_compiler_flags")

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
target_include_directories(
        "${LIBRARY_NAME}"
        PRIVATE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_link_libraries(
        "${EXEC_NAME}"
        PRIVATE
        "${PROJECT_NAME}_compiler_flags"
        "${LIBRARY_NAME}"
        "${PROJECT_NAME}"
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

set(_export_file "${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}_export.h")
generate_export_header("${LIBRARY_NAME}" EXPORT_FILE_NAME "${_export_file}")

# install rules
set(installable_libs "${EXEC_NAME}")
if (TARGET "${DEPENDANT_LIBRARY}")
    list(APPEND installable_libs "${DEPENDANT_LIBRARY}")
endif ()
install(FILES "${_export_file}" ${Header_Files}
        TYPE "INCLUDE")
install(TARGETS ${installable_libs}
        EXPORT "${EXEC_NAME}Targets"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
