cmake_minimum_required(VERSION 3.30)
project(Hyperflow_Engine VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")

#region Platforms
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_definitions(-DPLATFORM_WINDOWS)
    option(DESKTOP "Desktop" ON)
elseif  (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-DPLATFORM_LINUX)
    option(DESKTOP "Desktop" ON)
elseif  (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    add_definitions(-DPLATFORM_MAC)
    option(DESKTOP "Desktop" ON)
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DRELEASE)
endif()
#endregion

file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS src/*.cpp)

if(DESKTOP)
    set(PLATFORM platforms/platform_windows.cpp)
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

add_subdirectory(external/glm)
add_subdirectory(external/stb)
add_subdirectory(external/logc)

add_library(${PROJECT_NAME} ${ENGINE_SRC} ${PLATFORM})

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
#if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
#    target_link_libraries(Engine PRIVATE winmm)
#endif()

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

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR}/include/)