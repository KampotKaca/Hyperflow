set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -funroll-loops")

file(GLOB_RECURSE RPMALLOC_SRC CONFIGURE_DEPENDS *.c)
add_library(rpmalloc ${RPMALLOC_SRC})
