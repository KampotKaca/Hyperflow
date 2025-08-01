﻿
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    file(GLOB_RECURSE ADDITIONAL_SRC CONFIGURE_DEPENDS windows/*.cpp)
    message(STATUS "VULKAN WINDOWS")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")

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
            add_definitions(-DWAYLAND)
            file(GLOB_RECURSE ADDITIONAL_SRC CONFIGURE_DEPENDS wayland/*.cpp)
            message(STATUS "VULKAN WAYLAND")
        else()
            message(FATAL_ERROR "Wayland session detected but wayland-client not found")
        endif()
    else()
        find_package(X11 REQUIRED)
        if(X11_FOUND)
            add_definitions(-DX11)
            file(GLOB_RECURSE ADDITIONAL_SRC CONFIGURE_DEPENDS x11/*.cpp)
            message(STATUS "VULKAN X11")
        else()
            message(FATAL_ERROR "X11 libraries not found")
        endif()
    endif()
endif ()

file(GLOB_RECURSE VULKAN_SRC CONFIGURE_DEPENDS src/*.cpp)

add_subdirectory(external/vma)

add_library(vk SHARED ${VULKAN_SRC} ${ADDITIONAL_SRC})
target_precompile_headers(vk PRIVATE "include/hvk_shared.h")
set_target_properties(vk PROPERTIES UNITY_BUILD ON)

target_link_libraries(vk
        PRIVATE
        glm
        logc
        ext
        vma
)

find_package(Vulkan REQUIRED)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_libraries(vk PRIVATE Vulkan::Vulkan)
    else ()
        message(FATAL_ERROR "Vulkan libraries on windows can only be compiled with gcc")
    endif()
else ()
    target_link_libraries(vk PRIVATE Vulkan::Vulkan)
endif ()

target_include_directories(vk PUBLIC include)
target_include_directories(vk PUBLIC ../../include)
target_compile_definitions(vk PRIVATE RENDERER_DLL)

add_custom_command(TARGET vk POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:vk>
        $<TARGET_FILE_DIR:Hyperflow>
)