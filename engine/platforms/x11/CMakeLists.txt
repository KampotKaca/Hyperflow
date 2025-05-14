if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -funroll-loops")
endif()

file(GLOB_RECURSE X11_SRC CONFIGURE_DEPENDS src/*.cpp)
add_library(lnxx11 ${X11_SRC})

target_link_libraries(lnxx11
        PUBLIC
        glm
        stb
        logc
        rpmalloc
        ext
)

target_link_libraries(lnxx11
        PRIVATE
        X11
        Xxf86vm
        Xrandr
        Xi
        dl
)

target_include_directories(lnxx11 PUBLIC include)
target_include_directories(lnxx11 PUBLIC ../include)
target_include_directories(lnxx11 PUBLIC ../../include)
