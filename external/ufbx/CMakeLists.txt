project(ufbx LANGUAGES C)

file(GLOB_RECURSE UFBX_SRC CONFIGURE_DEPENDS *.c)
add_library(${PROJECT_NAME} ${UFBX_SRC})
target_sources(${PROJECT_NAME}
        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES include/ufbx.h
)

target_include_directories(${PROJECT_NAME} PUBLIC include)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE C)