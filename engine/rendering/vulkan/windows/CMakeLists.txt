file(GLOB_RECURSE VK_WIN_SRC CONFIGURE_DEPENDS *.cpp)

add_library(vkwin SHARED ${VK_WIN_SRC})
find_package(Vulkan REQUIRED)
target_link_libraries(vkwin PRIVATE Vulkan::Vulkan win)

target_include_directories(vkwin PUBLIC ../include)

add_custom_command(TARGET vkwin POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:vkwin>
        $<TARGET_FILE_DIR:Hyperflow>
)