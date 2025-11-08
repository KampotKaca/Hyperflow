cmake_minimum_required(VERSION 3.30)
project(Hyperflow)

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
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Og -g -march=native -static -static-libgcc -static-libstdc++ -pthread")
    endif()

elseif(CMAKE_BUILD_TYPE STREQUAL "Release")

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -funroll-loops")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native -flto -funroll-loops")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -funroll-loops -s -static -static-libgcc -static-libstdc++")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native -funroll-loops")
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

#editor
set(EDITOR BOOL TRUE)
add_definitions(-DEDITOR)
add_subdirectory(editor)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)

elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DRELEASE)
endif()

add_subdirectory(modelconvertor)
add_subdirectory(shaderconvertor)
add_subdirectory(engine)
add_subdirectory(application)