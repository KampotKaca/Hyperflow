project(lnx)

if(X11_FOUND)
    add_subdirectory(x11)
elseif (WAYLAND_FOUND)
    add_subdirectory(wayland)
endif ()

file(GLOB_RECURSE LNX_SRC CONFIGURE_DEPENDS src/*.cpp)
add_library(${PROJECT_NAME} ${LNX_SRC})
include("${CMAKE_SOURCE_DIR}/cmake/General.cmake")

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        stb
        ext
)

if(X11_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE lnxx11)
elseif (WAYLAND_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE lnxwayland)
endif ()

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ../include
        ../../include
)
