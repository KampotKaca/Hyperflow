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