file(GLOB_RECURSE COMPONENTS_SRC CONFIGURE_DEPENDS src/*.cpp)
add_library(components ${COMPONENTS_SRC})
target_precompile_headers(platform PRIVATE "include/hexternal.h")

target_link_libraries(components
        PUBLIC
        glm
        stb
        logc
        rpmalloc
        ext
)

target_include_directories(components PUBLIC include)
target_include_directories(components PUBLIC ../include)
