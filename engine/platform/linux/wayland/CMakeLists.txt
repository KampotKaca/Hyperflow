file(GLOB_RECURSE WAYLAND_SRC CONFIGURE_DEPENDS src/*.cpp)
add_library(lnxwayland ${WAYLAND_SRC})

target_link_libraries(lnxwayland
        PUBLIC
        glm
        stb
        logc
        rpmalloc
        ext
)

target_include_directories(lnxwayland PUBLIC include)
target_include_directories(lnxwayland PUBLIC ../include)
target_include_directories(lnxwayland PUBLIC ../../include)
