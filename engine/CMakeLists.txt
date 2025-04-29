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

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
endif()

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

    find_package(X11)
    if(X11_FOUND)
        add_definitions(-DX11)
        add_subdirectory(platforms/x11)
    else()
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(WAYLAND wayland-client)

        if(WAYLAND_FOUND)
            add_definitions(-DWAYLAND)
        else()
            message(FATAL_ERROR "Unsupported display server detected")
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
add_subdirectory(others/exception)

add_library(${PROJECT_NAME} ${ENGINE_SRC})

# Using keyword-based signature consistently
target_link_libraries(${PROJECT_NAME}
        PUBLIC
        glm
        stb
        logc
        rpmalloc
        ext
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

# Link winmm only on Windows
if (WINDOWS)
    target_link_libraries(${PROJECT_NAME} PRIVATE win)
    target_compile_definitions("${PROJECT_NAME}" PUBLIC VK_DLL="vkwin")
elseif (LINUX)
    target_link_libraries(${PROJECT_NAME} PRIVATE lnx)

    if(X11)
        target_link_libraries(${PROJECT_NAME} PRIVATE lnxx11)
        target_compile_definitions("${PROJECT_NAME}" PUBLIC VK_DLL="vkx11")
    elseif (WAYLAND)

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
        PRIVATE
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        include/
        platforms/include
        rendering/include
)

get_target_property(LIBS ${PROJECT_NAME} LINK_LIBRARIES)
message(STATUS "Linking libraries: ${LIBS}")

target_compile_definitions("${PROJECT_NAME}" PUBLIC RES_PATH="${CMAKE_BINARY_DIR}/res/")