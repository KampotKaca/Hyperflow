if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -funroll-loops")
endif()

file(GLOB_RECURSE EXT_SRC CONFIGURE_DEPENDS src/*.cpp)
add_library(ext ${EXT_SRC})

target_include_directories(ext PUBLIC include)
set_target_properties(ext PROPERTIES POSITION_INDEPENDENT_CODE ON)