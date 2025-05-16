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