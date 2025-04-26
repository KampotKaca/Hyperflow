file(GLOB_RECURSE EXT_SRC CONFIGURE_DEPENDS src/*.cpp)
add_library(ext ${EXT_SRC})

target_include_directories(ext PUBLIC include)