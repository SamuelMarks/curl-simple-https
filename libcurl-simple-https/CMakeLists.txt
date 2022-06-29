get_filename_component(LIBRARY_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

set(Header_Files "curl_simple_https.h")
source_group("Header Files" FILES "${Header_Files}")

set(Source_Files "curl_simple_https.c")
source_group("Source Files" FILES "${Source_Files}")

add_library("${LIBRARY_NAME}" "${LIBRARY_TYPE_FLAG}" "${Header_Files}" "${Source_Files}")

include(GNUInstallDirs)
target_include_directories(
        "${LIBRARY_NAME}"
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

if (MSVC)
  find_package(CURL CONFIG REQUIRED)
  set(CURL_LINK_LIBRARIES "CURL::libcurl")
else ()
  include(${CMAKE_SOURCE_DIR}/cmake/modules/FindCurlCustom.cmake)
  get_curl(CURL_LINK_LIBRARIES)
endif ()

target_link_libraries(
        "${LIBRARY_NAME}"
        PUBLIC
        "${CURL_LINK_LIBRARIES}"
)

set_target_properties(
        "${LIBRARY_NAME}"
        PROPERTIES
        LINKER_LANGUAGE
        C
)

include(GenerateExportHeader)
set(_export_file "${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}_export.h")
generate_export_header("${LIBRARY_NAME}" EXPORT_FILE_NAME "${_export_file}")

# setup the version numbering
set_property(TARGET "${LIBRARY_NAME}" PROPERTY VERSION "${${PROJECT_NAME}_VERSION}")
set_property(TARGET "${LIBRARY_NAME}" PROPERTY SOVERSION "${${PROJECT_NAME}_VERSION_MAJOR}")

# install rules
set(installable_libs "${LIBRARY_NAME}")
if (TARGET "${DEPENDANT_LIBRARY}")
  list(APPEND installable_libs "${DEPENDANT_LIBRARY}")
endif (TARGET "${DEPENDANT_LIBRARY}")
install(TARGETS ${installable_libs}
        EXPORT "${LIBRARY_NAME}Targets"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(FILES "${_export_file}" ${Header_Files}
        TYPE "INCLUDE")
install(EXPORT "${LIBRARY_NAME}Targets" DESTINATION "${CMAKE_INSTALL_DATADIR}/${LIBRARY_NAME}")
