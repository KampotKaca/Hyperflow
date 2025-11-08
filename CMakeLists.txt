cmake_minimum_required(VERSION 4.0)

file(READ "appconfig.h" CONFIG_HEADER_CONTENTS)
string(REGEX MATCH "#define VERSION \"([0-9]+\\.[0-9]+\\.[0-9]+)\"" VERSION_MATCH "${CONFIG_H_CONTENTS}")

# Set CMake project version
if(VERSION_MATCH)
    project(Hyperflow VERSION ${CMAKE_MATCH_1})
else()
    project(Hyperflow VERSION 0.0.0)
endif()

set(CMAKE_CXX_STANDARD 23)
set(PRODUCTION_BUILD_DIR "" CACHE STRING "Path to export stripped build")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Og")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wnull-dereference")

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=vla -Werror=vla-cxx-extension")
        option(CLANG "Clang" ON)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Og -g -march=native -static -static-libgcc -static-libstdc++ -pthread")
        option(GNU "Gnu" ON)
    endif()

elseif(CMAKE_BUILD_TYPE STREQUAL "Release")

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -funroll-loops")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native -flto -funroll-loops")
        option(CLANG "Clang" ON)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -funroll-loops -s -static -static-libgcc -static-libstdc++")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native -funroll-loops")
        option(GNU "Gnu" ON)
    endif()
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_definitions(-DPLATFORM_WINDOWS)
    option(WINDOWS "Windows" ON)

elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-DPLATFORM_LINUX)
    option(LINUX "Linux" ON)

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
            option(WAYLAND "Wayland" ON)
        else()
            message(FATAL_ERROR "Wayland session detected but wayland-client not found")
        endif()
    else()
        find_package(X11 REQUIRED)
        if(X11_FOUND)
            message(STATUS "Using X11 display server")
            add_definitions(-DX11)
            option(X11 "X11" ON)
        else()
            message(FATAL_ERROR "X11 libraries not found")
        endif()
    endif()

else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
    set(ENV{DISABLE_RTSS_LAYER} "1")
    set(ENV{MANGOHUD} "0")
    add_definitions(-DENABLE_STATISTICS=1)

elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DRELEASE)
    set(ENV{MANGOHUD} "1")
endif()

option(BUILD_D3D "build_d3d" ON)
option(BUILD_VULKAN "build_vulkan" ON)

if(BUILD_D3D)
    if (WINDOWS)
        option(D3D "d3d" ON)
        add_definitions(-DRENDERER_D3D)
    endif ()
endif ()

if(BUILD_VULKAN)
    option(VULKAN "vulkan" ON)
    add_definitions(-DRENDERER_VULKAN)
    add_subdirectory(external/vma)
endif ()

option(ENABLE_EDITOR "Build the editor instead of the application" OFF)

#glfw flags
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)

add_subdirectory(external/glfw)
add_subdirectory(external/glm)
add_subdirectory(external/stb)
add_subdirectory(external/logc)
add_subdirectory(external/rpmalloc)
add_subdirectory(external/rapidyaml)
add_subdirectory(external/svector)
add_subdirectory(external/lz4/build/cmake)
add_subdirectory(external/miniaudio)

add_subdirectory(modelconvertor)
add_subdirectory(shaderconvertor)
add_subdirectory(engine)
add_subdirectory(application)

if(ENABLE_EDITOR)
    add_subdirectory(external/imgui)
    add_subdirectory(editor)
    add_definitions(-DEDITOR)
    set_target_properties(Hyperflow_Editor PROPERTIES LINK_SEARCH_START_STATIC ON LINK_SEARCH_END_STATIC ON)
    set(EXECUTABLE_TARGET Hyperflow_Editor)
    message(STATUS "Editor enabled!")
else ()
    set(EXECUTABLE_TARGET Hyperflow)
endif ()