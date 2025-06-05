file(GLOB_RECURSE PLATFORM_SRC CONFIGURE_DEPENDS src/*.cpp)
add_library(platform ${PLATFORM_SRC})
target_precompile_headers(platform PRIVATE "../include/hex_platform.h")

target_link_libraries(platform
        PUBLIC
        glm
        stb
        logc
        rpmalloc
        ext
        PRIVATE
        glfw
)

find_package(Vulkan)
if(VULKAN_FOUND)
    message(STATUS "VULKAN PLATFORM ADDED")
    add_definitions(-DVULKAN)
    target_link_libraries(platform PRIVATE Vulkan::Vulkan)
endif ()

target_include_directories(platform PUBLIC include)
target_include_directories(platform PUBLIC ../include)
target_include_directories(platform PUBLIC ../../include)