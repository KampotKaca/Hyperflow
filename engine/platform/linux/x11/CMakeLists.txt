project(lnxx11)

file(GLOB_RECURSE X11_SRC CONFIGURE_DEPENDS src/*.cpp)
add_library(${PROJECT_NAME} ${X11_SRC})
include("${CMAKE_SOURCE_DIR}/cmake/General.cmake")

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        stb
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ../include
        ../../include
)
