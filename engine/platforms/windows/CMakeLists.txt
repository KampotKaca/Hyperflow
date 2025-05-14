if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -funroll-loops")
endif()

file(GLOB_RECURSE WIN_SRC CONFIGURE_DEPENDS src/*.cpp)
add_library(win ${WIN_SRC})

target_link_libraries(win
        PUBLIC
        glm
        stb
        logc
        rpmalloc
        ext
)

target_link_libraries(win PRIVATE winmm Dbghelp)

target_include_directories(win PUBLIC include)
target_include_directories(win PUBLIC ../include)
target_include_directories(win PUBLIC ../../include)