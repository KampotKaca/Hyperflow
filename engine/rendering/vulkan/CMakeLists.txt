if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
add_subdirectory(windows)
message(STATUS "Windows")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")

    find_package(PkgConfig)
    pkg_check_modules(WAYLAND wayland-client)

    if(WAYLAND_FOUND)
        add_definitions(-DWAYLAND)
        add_subdirectory(wayland)
        message(STATUS "WAYLAND")
    else()
        find_package(X11 REQUIRED)
        if(X11_FOUND)
            add_definitions(-DX11)
            add_subdirectory(x11)
            message(STATUS "X11")
        else()
            message(FATAL_ERROR "No linux display manager found")
        endif ()

    endif()
endif ()

file(GLOB_RECURSE VULKAN_SRC CONFIGURE_DEPENDS src/*.cpp)

add_subdirectory(external/vma)

add_library(vk SHARED ${VULKAN_SRC})

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_dependencies(vk vkwin)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(WAYLAND)
        add_dependencies(vk vkwayland)
    elseif (X11)
        add_dependencies(vk vkx11)
    endif ()
endif ()

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