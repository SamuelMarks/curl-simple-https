include("release/CPackConfig.cmake")

set(CPACK_INSTALL_CMAKE_PROJECTS
        "debug;libcurl_simple_https;ALL;/"
        "release;libcurl_simple_https;ALL;/"
)
