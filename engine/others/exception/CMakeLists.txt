file(GLOB_RECURSE EXT_SRC CONFIGURE_DEPENDS src/*.cpp)
add_library(ext ${EXT_SRC})

target_include_directories(ext PUBLIC include)
set_target_properties(ext PROPERTIES POSITION_INDEPENDENT_CODE ON)