project(tinyobj)

file(GLOB_RECURSE TINY_OBJ_SRC CONFIGURE_DEPENDS *.cpp)
add_library(${PROJECT_NAME} ${TINY_OBJ_SRC})
target_sources(${PROJECT_NAME}
        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES include/tiny_obj_loader.h
)

target_include_directories(${PROJECT_NAME} PUBLIC include)