project(logc)
file(GLOB_RECURSE LOG_C_SRC CONFIGURE_DEPENDS *.c)

add_definitions(-DLOG_USE_COLOR)
add_compile_options(-fdiagnostics-color=always)

add_library(${PROJECT_NAME} ${LOG_C_SRC})
target_sources(${PROJECT_NAME}
        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
        FILES log.h
)