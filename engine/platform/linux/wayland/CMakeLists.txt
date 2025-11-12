project(lnxwayland)

file(GLOB_RECURSE WAYLAND_SRC CONFIGURE_DEPENDS src/*.cpp)
add_library(${PROJECT_NAME} ${WAYLAND_SRC})
include("${CMAKE_SOURCE_DIR}/cmake/General.cmake")

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        stb
)

target_include_directories(${PROJECT_NAME} PUBLIC
        ../include
        "${CMAKE_SOURCE_DIR}/engine/platform/include"
        "${CMAKE_SOURCE_DIR}/engine/include"
)
