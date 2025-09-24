file(GLOB_RECURSE TINY_OBJ_SRC CONFIGURE_DEPENDS src/*.cpp src/*.c)
add_library(tiny_obj ${TINY_OBJ_SRC})

target_include_directories(tiny_obj PUBLIC include)