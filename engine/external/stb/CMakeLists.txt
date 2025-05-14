set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -funroll-loops")

file(GLOB_RECURSE STB_SRC CONFIGURE_DEPENDS src/*.c)
add_library(stb ${STB_SRC})

target_include_directories(stb PUBLIC include)