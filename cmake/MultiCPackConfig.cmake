include("release/CPackConfig.cmake")

set(CPACK_INSTALL_CMAKE_PROJECTS
        "debug;libcurl-simple-https;ALL;/"
        "release;libcurl-simple-https;ALL;/"
)
