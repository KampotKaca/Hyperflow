if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -funroll-loops")
endif()

file(GLOB_RECURSE LNX_SRC CONFIGURE_DEPENDS src/*.cpp)
add_library(lnx ${LNX_SRC})

target_link_libraries(lnx
        PUBLIC
        glm
        stb
        logc
        rpmalloc
        ext
)

target_include_directories(lnx PUBLIC include)
target_include_directories(lnx PUBLIC ../include)
target_include_directories(lnx PUBLIC ../../include)