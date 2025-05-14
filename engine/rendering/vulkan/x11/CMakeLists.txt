if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -funroll-loops")
endif()

file(GLOB_RECURSE VK_X11_SRC CONFIGURE_DEPENDS *.cpp)

add_library(vkx11 SHARED ${VK_X11_SRC})
find_package(Vulkan REQUIRED)
target_link_libraries(vkx11 PRIVATE Vulkan::Vulkan lnx lnxx11)

target_include_directories(vkx11 PUBLIC ../include)

add_custom_command(TARGET vkx11 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:vkx11>
        $<TARGET_FILE_DIR:Hyperflow>
)