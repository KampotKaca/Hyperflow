cmake_minimum_required(VERSION 3.30)

file(READ "config.h" CONFIG_H_CONTENTS)
string(REGEX MATCH "#define VERSION \"([0-9]+\\.[0-9]+\\.[0-9]+)\"" VERSION_MATCH "${CONFIG_H_CONTENTS}")

option(BUILD_D3D "build_d3d" ON)
option(BUILD_VULKAN "build_vulkan" ON)

# Set CMake project version
if(VERSION_MATCH)
    project(Hyperflow_Engine VERSION ${CMAKE_MATCH_1})
else()
    project(Hyperflow_Engine VERSION 0.0.0)
endif()

#region Platforms

if (WINDOWS)
    add_subdirectory(platforms/windows)

    option(VULKAN "vulkan" ON)
    add_definitions(-DRENDERER_VULKAN)

    if(BUILD_D3D STREQUAL "ON")
        option(D3D "d3d" ON)
        add_definitions(-DRENDERER_D3D)
    endif ()

elseif (LINUX)
    add_subdirectory(platforms/linux)

    option(VULKAN "vulkan" ON)
    add_definitions(-DRENDERER_VULKAN)

    execute_process(
            COMMAND sh -c "echo $XDG_SESSION_TYPE"
            OUTPUT_VARIABLE DISPLAY_SERVER_TYPE
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(WAYLAND)
        add_subdirectory(platforms/wayland)
    elseif (X11)
        add_subdirectory(platforms/x11)
    else ()
        message(FATAL_ERROR "Linux window server not found")
    endif ()

else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

if(D3D STREQUAL "ON")
    add_subdirectory(rendering/d3d)
endif ()
if (VULKAN STREQUAL "ON")
    add_subdirectory(rendering/vulkan)
endif ()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DENABLE_STATISTICS=1)
endif ()

#endregion

file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS src/*.cpp)

add_subdirectory(external/glm)
add_subdirectory(external/stb)
add_subdirectory(external/logc)
add_subdirectory(external/rpmalloc)
add_subdirectory(external/rapidyaml)
add_subdirectory(external/svector)
add_subdirectory(external/glfw)
add_subdirectory(external/lz4/build/cmake)
add_subdirectory(external/miniaudio)
add_subdirectory(platforms/platform)
add_subdirectory(components)

add_subdirectory(others/exception)

add_library(${PROJECT_NAME} ${ENGINE_SRC})
target_sources(${PROJECT_NAME}
        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES include/hshared.h
)

target_precompile_headers(${PROJECT_NAME} PRIVATE "include/hshared.h")
include(${CMAKE_SOURCE_DIR}/cmake/AssetFolder.cmake)
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ON)

# Using keyword-based signature consistently
target_link_libraries(${PROJECT_NAME}
        PUBLIC
        glm
        stb
        logc
        lz4_static
        ext
        components
        svector
        PRIVATE
        ryml
        rpmalloc
        glfw
        platform
        miniaudio
)

if(D3D)
    add_dependencies(${PROJECT_NAME} d3d)
endif ()

if(VULKAN)
    add_dependencies(${PROJECT_NAME} vk)
endif ()

target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        BUILDING_HYPERFLOW
)

if (WINDOWS)
    target_link_libraries(${PROJECT_NAME} PRIVATE win)
elseif (LINUX)
    target_link_libraries(${PROJECT_NAME} PRIVATE lnx)

    if(X11_FOUND)
        target_link_libraries(${PROJECT_NAME} PRIVATE lnxx11)
    elseif (WAYLAND_FOUND)
        target_link_libraries(${PROJECT_NAME} PRIVATE lnxwayland)
    else ()
        message(FATAL_ERROR "Unsupported display server detected")
    endif ()
endif ()

# Set include directories
target_include_directories(${PROJECT_NAME}
        PUBLIC
        src/
        include/
        external/glm
        external/logc/include
        external/svector/include
        PRIVATE
        external/rapidyaml/src
        external/stb/include
        external/rpmalloc
        external/glfw/include
        external/miniaudio
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        platforms/include
        rendering/include
        ${CMAKE_SOURCE_DIR}/modelconvertor/include
)

get_target_property(LIBS ${PROJECT_NAME} LINK_LIBRARIES)
message(STATUS "Linking libraries: ${LIBS}")