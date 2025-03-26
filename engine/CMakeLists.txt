cmake_minimum_required(VERSION 3.30)
project(Hyperflow_Engine VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")

file(READ "config.h" CONFIG_HEADER_CONTENTS)
string(FIND "${CONFIG_HEADER_CONTENTS}" "#define RENDERER_D3D" RENDERER_D3D)
string(FIND "${CONFIG_HEADER_CONTENTS}" "#define RENDERER_VULKAN" RENDERER_VULKAN)

#region Platforms
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_definitions(-DPLATFORM_WINDOWS)
    option(WINDOWS "Windows" ON)
    file(GLOB_RECURSE PLATFORM CONFIGURE_DEPENDS platforms/windows/*.cpp)

    if(NOT RENDERER_D3D EQUAL -1)
        add_definitions(-DRENDERER_D3D)
        option(D3D "d3d" ON)
        file(GLOB_RECURSE RENDERER CONFIGURE_DEPENDS rendering/d3d/*.cpp)
    endif ()

elseif  (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-DPLATFORM_LINUX)
    option(LINUX "Linux" ON)
    file(GLOB_RECURSE PLATFORM CONFIGURE_DEPENDS platforms/linux/*.cpp)

    if(NOT RENDERER_VULKAN EQUAL -1)
        add_definitions(-DRENDERER_VULKAN)
        option(VULKAN "vulkan" ON)
        file(GLOB_RECURSE RENDERER CONFIGURE_DEPENDS rendering/vulkan/*.cpp)
    endif ()

else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()
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
        dl
)

target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        BUILDING_HYPERFLOW
)

# Link winmm only on Windows
if (WINDOWS)
    target_link_libraries(${PROJECT_NAME} PRIVATE winmm)
elseif (LINUX)
    target_link_libraries(${PROJECT_NAME} PRIVATE X11 Xxf86vm Xrandr Xi)
endif ()

if (D3D)
    target_link_libraries(${PROJECT_NAME}
            PRIVATE
            d3d11
            dxgi
    )
endif ()

if(VULKAN)
    target_link_libraries(${PROJECT_NAME} PRIVATE vulkan)
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
