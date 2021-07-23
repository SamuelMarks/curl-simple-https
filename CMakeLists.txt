cmake_minimum_required(VERSION 3.19)
project(curl_simple_https LANGUAGES C)
string(TOLOWER "${PROJECT_NAME}" PROJECT_LOWER_NAME)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)
if (BUILD_SHARED_LIBS)
    set(LIBRARY_TYPE_FLAG "SHARED")
else ()
    set(LIBRARY_TYPE_FLAG "STATIC")
endif ()

add_library("${PROJECT_LOWER_NAME}_compiler_flags" INTERFACE)
target_compile_features("${PROJECT_LOWER_NAME}_compiler_flags" INTERFACE "c_std_90")

set(gcc_like "$<COMPILE_LANG_AND_ID:C,CXX,ARMClang,AppleClang,Clang,GNU>")
set(msvc "$<COMPILE_LANG_AND_ID:C,CXX,MSVC>")
# -g -fsanitize=address -fno-omit-frame-pointer
target_compile_options(
        "${PROJECT_LOWER_NAME}_compiler_flags"
        INTERFACE
        "$<${gcc_like}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>"
        "$<${msvc}:$<BUILD_INTERFACE:-W3;-WX;-Zi;-permissive->>"
)

set(Header_Files "src/curl_simple_https.h")
source_group("Header Files" FILES "${Header_Files}")

set(Source_Files "src/curl_simple_https.c")
source_group("Source Files" FILES "${Source_Files}")

add_library("${PROJECT_NAME}" "${LIBRARY_TYPE_FLAG}" "${Header_Files}" "${Source_Files}")

find_package(CURL CONFIG REQUIRED)
target_link_libraries(
        "${PROJECT_NAME}"
        PRIVATE
        CURL::libcurl
)

set_target_properties(
        "${PROJECT_NAME}"
        PROPERTIES
        LINKER_LANGUAGE
        C
)

include(GenerateExportHeader)
set(_export_file "${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}_export.h")
generate_export_header("${PROJECT_NAME}" EXPORT_FILE_NAME "${_export_file}")

# setup the version numbering
set_property(TARGET "${PROJECT_NAME}" PROPERTY VERSION "1.0.0")
set_property(TARGET "${PROJECT_NAME}" PROPERTY SOVERSION "1")

# install rules
set(installable_libs "${PROJECT_NAME}" "${PROJECT_LOWER_NAME}_compiler_flags")
if (TARGET "${DEPENDANT_LIBRARY}")
    list(APPEND installable_libs "${DEPENDANT_LIBRARY}")
endif ()
install(TARGETS ${installable_libs}
        DESTINATION "bin"
        EXPORT "${PROJECT_NAME}Targets")
install(FILES "${_export_file}" "${Header_Files}" DESTINATION "include")
