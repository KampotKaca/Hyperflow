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

set(CMAKE_CXX_STANDARD 23)

#region Platforms

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_definitions(-DPLATFORM_WINDOWS)
    option(WINDOWS "Windows" ON)
    add_subdirectory(platforms/windows)

    option(VULKAN "vulkan" ON)
    add_definitions(-DRENDERER_VULKAN)

    if(BUILD_D3D STREQUAL "ON")
        option(D3D "d3d" ON)
        add_definitions(-DRENDERER_D3D)
    endif ()

elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-DPLATFORM_LINUX)
    option(LINUX "Linux" ON)
    add_subdirectory(platforms/linux)

    option(VULKAN "vulkan" ON)
    add_definitions(-DRENDERER_VULKAN)

    execute_process(
            COMMAND sh -c "echo $XDG_SESSION_TYPE"
            OUTPUT_VARIABLE DISPLAY_SERVER_TYPE
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(NOT DISPLAY_SERVER_TYPE)
        if(DEFINED ENV{DISPLAY})
            set(DISPLAY_SERVER_TYPE "x11")
        else()
            message(FATAL_ERROR "Could not detect display server type")
        endif()
    endif()

    if(DISPLAY_SERVER_TYPE STREQUAL "wayland")
        find_package(PkgConfig)
        pkg_check_modules(WAYLAND wayland-client)

        if(WAYLAND_FOUND)
            message(STATUS "Using Wayland display server")
            add_definitions(-DWAYLAND)
            add_subdirectory(platforms/wayland)
        else()
            message(FATAL_ERROR "Wayland session detected but wayland-client not found")
        endif()
    else()
        find_package(X11 REQUIRED)
        if(X11_FOUND)
            message(STATUS "Using X11 display server")
            add_definitions(-DX11)
            add_subdirectory(platforms/x11)
        else()
            message(FATAL_ERROR "X11 libraries not found")
        endif()
    endif()

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
add_subdirectory(external/glfw)
add_subdirectory(platforms/platform)
add_subdirectory(components)

add_subdirectory(others/exception)

add_library(${PROJECT_NAME} ${ENGINE_SRC})
target_precompile_headers(${PROJECT_NAME} PRIVATE "include/hshared.h")

# Using keyword-based signature consistently
target_link_libraries(${PROJECT_NAME}
        PUBLIC
        glm
        stb
        logc
        ryml
        ext
        components
        PRIVATE
        rpmalloc
        glfw
        platform
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
    target_compile_definitions("${PROJECT_NAME}" PUBLIC VK_DLL="vkwin")
elseif (LINUX)
    target_link_libraries(${PROJECT_NAME} PRIVATE lnx)

    if(X11_FOUND)
        target_link_libraries(${PROJECT_NAME} PRIVATE lnxx11)
        target_compile_definitions("${PROJECT_NAME}" PUBLIC VK_DLL="vkx11")
    elseif (WAYLAND_FOUND)
        target_link_libraries(${PROJECT_NAME} PRIVATE lnxwayland)
        target_compile_definitions("${PROJECT_NAME}" PUBLIC VK_DLL="vkwayland")
    else ()
        message(FATAL_ERROR "Unsupported display server detected")
    endif ()
endif ()

# Set include directories
target_include_directories(${PROJECT_NAME}
        PUBLIC
        src/
        include/
        external/stb/include
        external/glm
        external/logc/include
        external/rpmalloc
        external/rapidyaml/src
        external/glfw/include
        PRIVATE
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        include/
        platforms/include
        rendering/include
        components/include
        ../meshconvertor/include
)

get_target_property(LIBS ${PROJECT_NAME} LINK_LIBRARIES)
message(STATUS "Linking libraries: ${LIBS}")

target_compile_definitions("${PROJECT_NAME}" PUBLIC RES_PATH="${CMAKE_BINARY_DIR}/res/")