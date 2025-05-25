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

target_include_directories(lnxx11 PUBLIC include)
target_include_directories(lnxx11 PUBLIC ../include)
target_include_directories(lnxx11 PUBLIC ../../include)
