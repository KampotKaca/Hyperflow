if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
endif()

file(GLOB_RECURSE VULKAN_SRC CONFIGURE_DEPENDS src/*.cpp)

add_library(vk SHARED ${VULKAN_SRC})

target_link_libraries(vk
        PRIVATE
        glm
        logc
        ext
)

find_package(Vulkan REQUIRED)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_libraries(vk PRIVATE Vulkan::Vulkan win)
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