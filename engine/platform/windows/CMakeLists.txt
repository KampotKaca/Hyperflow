project(win)

file(GLOB_RECURSE WIN_SRC CONFIGURE_DEPENDS src/*.cpp)
add_library(${PROJECT_NAME} ${WIN_SRC})
include("${CMAKE_SOURCE_DIR}/cmake/General.cmake")

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        stb
        logc
)

target_link_libraries(${PROJECT_NAME} PRIVATE winmm Dbghelp)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ../include
        ../../include
)