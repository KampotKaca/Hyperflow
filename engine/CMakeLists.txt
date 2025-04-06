cmake_minimum_required(VERSION 3.30)

file(READ "config.h" CONFIG_H_CONTENTS)
string(REGEX MATCH "#define VERSION \"([0-9]+\\.[0-9]+\\.[0-9]+)\"" VERSION_MATCH "${CONFIG_H_CONTENTS}")

#option(BUILD_D3D "build_d3d" ON)
#option(BUILD_VULKAN "build_vulkan" ON)

# Set CMake project version
if(VERSION_MATCH)
    project(Hyperflow_Engine VERSION ${CMAKE_MATCH_1})
else()
    project(Hyperflow_Engine VERSION 0.0.0)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")

#region Platforms

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_definitions(-DPLATFORM_WINDOWS)
    option(WINDOWS "Windows" ON)
    file(GLOB_RECURSE PLATFORM CONFIGURE_DEPENDS platforms/windows/*.cpp)

    if(BUILD_D3D STREQUAL "ON")
        option(D3D "d3d" ON)
        add_definitions(-DRENDERER_D3D)
    endif ()

elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-DPLATFORM_LINUX)
    option(LINUX "Linux" ON)

    find_package(X11)
    if(X11_FOUND)
        add_definitions(-DX11)
        file(GLOB_RECURSE PLATFORM CONFIGURE_DEPENDS platforms/linux/x11/*.cpp)
    else()
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(WAYLAND wayland-client)
        file(GLOB_RECURSE PLATFORM CONFIGURE_DEPENDS platforms/linux/wayland/*.cpp)

        if(WAYLAND_FOUND)
            add_definitions(-DWAYLAND)
        else()
            message(FATAL_ERROR "Unsupported display server detected")
        endif()
    endif()

else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

if(NOT D3D STREQUAL "ON")
    option(VULKAN "vulkan" ON)
    add_definitions(-DRENDERER_VULKAN)
endif ()

if(D3D STREQUAL "ON")
    file(GLOB_RECURSE RENDERER CONFIGURE_DEPENDS rendering/d3d/*.cpp)
elseif (VULKAN STREQUAL "ON")
    file(GLOB_RECURSE RENDERER CONFIGURE_DEPENDS rendering/vulkan/*.cpp)
endif ()

#endregion

file(GLOB_RECURSE RENDERER_SRC CONFIGURE_DEPENDS rendering/src/*.cpp)
file(GLOB_RECURSE PLATFORM_SRC CONFIGURE_DEPENDS platforms/src/*.cpp)
file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS src/*.cpp)

add_subdirectory(external/glm)
add_subdirectory(external/stb)
add_subdirectory(external/logc)

add_library(${PROJECT_NAME} ${ENGINE_SRC} ${PLATFORM} ${RENDERER} ${PLATFORM_SRC} ${RENDERER_SRC})

# Using keyword-based signature consistently
target_link_libraries(${PROJECT_NAME}
        PUBLIC
        glm
        stb
        logc
)

target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        BUILDING_HYPERFLOW
)

# Link winmm only on Windows
if (WINDOWS)
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm)
elseif (LINUX)
    target_link_libraries(${PROJECT_NAME}
            PRIVATE
            X11
            Xxf86vm
            Xrandr
            Xi
            dl
    )
endif ()

if (D3D)
    target_link_libraries(${PROJECT_NAME}
            PRIVATE
            d3d11
            dxgi
    )
endif ()

if(VULKAN)
    find_package(Vulkan REQUIRED)
    target_link_libraries(${PROJECT_NAME}
            PRIVATE
            Vulkan::Vulkan
    )
endif ()

# Set include directories
target_include_directories(${PROJECT_NAME}
        PUBLIC
        src/
        include/
        external/stb/include
        external/glm
        external/logc/include
        PRIVATE
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        include/
        platforms/include
        rendering/include
)