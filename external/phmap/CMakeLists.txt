project(phmap)
file(GLOB_RECURSE PHMAP_H CONFIGURE_DEPENDS *.h)

add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME}
        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
        FILES ${PHMAP_H}
)