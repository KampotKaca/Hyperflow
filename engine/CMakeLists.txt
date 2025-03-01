#region Platforms

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_definitions(-DPLATFORM_WINDOWS)
    option(DESKTOP "Desktop" ON)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-DPLATFORM_LINUX)
    option(DESKTOP "Desktop" ON)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    add_definitions(-DPLATFORM_MAC)
    option(DESKTOP "Desktop" ON)
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
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


add_library(Engine ${ENGINE_SRC} ${PLATFORM})

# Using keyword-based signature consistently
target_link_libraries(Engine PRIVATE glm)
target_link_libraries(Engine PRIVATE stb)
target_link_libraries(Engine PRIVATE logc)

# Link winmm only on Windows
#if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
#    target_link_libraries(Engine PRIVATE winmm)
#endif()

target_include_directories(Engine PUBLIC src/)
target_include_directories(Engine PUBLIC include/)
target_include_directories(Engine PUBLIC external/stb/include)
target_include_directories(Engine PUBLIC external/glm)
target_include_directories(Engine PUBLIC external/logc/include)
