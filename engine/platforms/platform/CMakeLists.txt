file(GLOB_RECURSE PLATFORM_SRC CONFIGURE_DEPENDS src/*.cpp)
add_library(platform ${PLATFORM_SRC})

target_link_libraries(platform
        PUBLIC
        glm
        stb
        logc
        rpmalloc
        ext
        PRIVATE
        glfw
)

target_include_directories(platform PUBLIC include)
target_include_directories(platform PUBLIC ../include)
target_include_directories(platform PUBLIC ../../include)