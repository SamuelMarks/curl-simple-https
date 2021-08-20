function(get_curl LINK_LIBRARIES)
    set(CURL_NO_OLDIES ON)

    # curl version >=7.57 can have config files
    find_package(CURL QUIET COMPONENTS libcurl CONFIG)

    # This conditional if/else block from https://gitlab.kitware.com/cmake/cmake/-/issues/18378#note_467156
    if (CURL_FOUND)
        message(STATUS "Using ${CURL_LIBRARIES} v${CURL_VERSION}")
        get_target_property(LIBCURL_TYPE CURL::libcurl TYPE)
        # CURL_STATICLIB is missing for curl versions <7.61.1
        if (CURL_VERSION VERSION_LESS 7.61.1 AND LIBCURL_TYPE STREQUAL "STATIC_LIBRARY")
            set_target_properties(CURL::libcurl PROPERTIES
                    INTERFACE_COMPILE_DEFINITIONS CURL_STATICLIB)
        endif ()
    else ()
        # try to use libcurl.pc if installed
        set(CMAKE_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES} lib)
        set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES} _imp.lib -d.lib -d_imp.lib)
        find_package(PkgConfig)
        if (PkgConfig_FOUND)
            set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH ON)
            pkg_search_module(CURL libcurl)
            if (CURL_FOUND)
                # CMake <3.12 workaround
                if (NOT CURL_LINK_LIBRARIES)
                    find_library(CURL_LINK_LIBRARIES NAMES ${CURL_LIBRARIES}
                            HINTS "${CURL_LIBDIR}" NO_DEFAULT_PATH)
                    if (NOT CURL_LINK_LIBRARIES)
                        message(FATAL_ERROR "curl is not installed; install libcurl then try again")
                    endif ()
                endif ()
                if (NOT CURL_LINK_LIBRARIES MATCHES "_imp.lib$|${CMAKE_SHARED_LIBRARY_SUFFIX}$")
                    list(REMOVE_ITEM CURL_STATIC_LIBRARIES ${CURL_LIBRARIES})
                    add_library(CURL::libcurl STATIC IMPORTED)
                    set_target_properties(CURL::libcurl PROPERTIES
                            INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${CURL_STATIC_INCLUDE_DIRS}"
                            INTERFACE_COMPILE_DEFINITIONS CURL_STATICLIB
                            INTERFACE_LINK_LIBRARIES "${CURL_STATIC_LIBRARIES}"
                            IMPORTED_LINK_INTERFACE_LANGUAGES C
                            IMPORTED_LOCATION "${CURL_LINK_LIBRARIES}")
                    link_directories(CURL_STATIC_LIBRARY_DIRS)
                else ()
                    add_library(CURL::libcurl SHARED IMPORTED)
                    set_target_properties(CURL::libcurl PROPERTIES
                            INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${CURL_INCLUDE_DIRS}")
                    if (WIN32)
                        set_target_properties(CURL::libcurl PROPERTIES
                                IMPORTED_IMPLIB "${CURL_LINK_LIBRARIES}")
                    else ()
                        set_target_properties(CURL::libcurl PROPERTIES
                                IMPORTED_LOCATION "${CURL_LINK_LIBRARIES}")
                    endif ()
                endif ()
                message(STATUS "Using ${CURL_LIBRARIES} v${CURL_VERSION}")
            endif ()
        endif ()
        # try old module from CMake distribution
        if (NOT CURL_FOUND)
            find_package(CURL REQUIRED COMPONENTS libcurl MODULE)
            if (NOT CURL_LIBRARIES MATCHES "_imp.lib$|${CMAKE_SHARED_LIBRARY_SUFFIX}$")
                add_library(CURL::libcurl STATIC IMPORTED)
                set_target_properties(CURL::libcurl PROPERTIES
                        INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${CURL_INCLUDE_DIRS}"
                        INTERFACE_COMPILE_DEFINITIONS CURL_STATICLIB
                        IMPORTED_LINK_INTERFACE_LANGUAGES C
                        IMPORTED_LOCATION "${CURL_LIBRARIES}")
            else ()
                add_library(CURL::libcurl SHARED IMPORTED)
                set_target_properties(CURL::libcurl PROPERTIES
                        INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${CURL_INCLUDE_DIRS}")
                if (WIN32)
                    set_target_properties(CURL::libcurl PROPERTIES
                            IMPORTED_IMPLIB "${CURL_LIBRARIES}")
                else ()
                    set_target_properties(CURL::libcurl PROPERTIES
                            IMPORTED_LOCATION "${CURL_LIBRARIES}")
                endif ()
            endif ()
            set(CURL_VERSION ${CURL_VERSION_STRING})
        endif ()
    endif ()

    if (NOT CURL_FOUND)
        message(FATAL_ERROR "curl is not installed; install libcurl then try again")
    endif ()

    set(${LINK_LIBRARIES} "${CURL_LINK_LIBRARIES}" PARENT_SCOPE)
endfunction()
