project(platform)

if (WINDOWS)
    add_subdirectory(windows)
elseif (LINUX)
    add_subdirectory(linux)
endif ()

file(GLOB_RECURSE PLATFORM_SRC CONFIGURE_DEPENDS src/*.cpp)
add_library(${PROJECT_NAME} ${PLATFORM_SRC})
include("${CMAKE_SOURCE_DIR}/cmake/General.cmake")
target_precompile_headers(${PROJECT_NAME} PRIVATE "include/hex_platform.h")
#set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ON)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        stb
        ext
        PRIVATE
        glfw
)

if (WINDOWS)
    target_link_libraries(${PROJECT_NAME} PRIVATE win)
elseif (LINUX)
    target_link_libraries(${PROJECT_NAME} PRIVATE lnx)
endif ()

include("${CMAKE_SOURCE_DIR}/cmake/Vulkan.cmake")

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ../include
        ../../include
        ${CMAKE_SOURCE_DIR}/external/glfw/include
)