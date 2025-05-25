file(GLOB_RECURSE VK_WAYLAND_SRC CONFIGURE_DEPENDS *.cpp)

add_library(vkwayland SHARED ${VK_WAYLAND_SRC})
find_package(Vulkan REQUIRED)
target_link_libraries(vkwayland PRIVATE Vulkan::Vulkan lnx lnxwayland)

target_include_directories(vkwayland PUBLIC ../include)

add_custom_command(TARGET vkwayland POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:vkwayland>
        $<TARGET_FILE_DIR:Hyperflow>
)