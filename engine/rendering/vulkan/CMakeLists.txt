project(vk)

if(WINDOWS)
    file(GLOB_RECURSE ADDITIONAL_SRC CONFIGURE_DEPENDS windows/*.cpp)
elseif (LINUX)

    if(WAYLAND)
        file(GLOB_RECURSE ADDITIONAL_SRC CONFIGURE_DEPENDS wayland/*.cpp)
    elseif(X11)
        file(GLOB_RECURSE ADDITIONAL_SRC CONFIGURE_DEPENDS x11/*.cpp)
    endif()
endif ()

file(GLOB_RECURSE VULKAN_SRC CONFIGURE_DEPENDS src/*.cpp)

add_library(${PROJECT_NAME} SHARED ${VULKAN_SRC} ${ADDITIONAL_SRC})
include("${CMAKE_SOURCE_DIR}/cmake/General.cmake")
target_precompile_headers(${PROJECT_NAME} PRIVATE "include/hvk_shared.h")
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ON)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        ext
        vma
)

find_package(Vulkan REQUIRED)

if(WINDOWS)
    if (GNU)
        target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)
    else ()
        message(FATAL_ERROR "Vulkan libraries on windows can only be compiled with gcc")
    endif()
else ()
    target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC ../../include)
target_compile_definitions(${PROJECT_NAME} PRIVATE RENDERER_DLL)
include("${CMAKE_SOURCE_DIR}/cmake/ExportAsDll.cmake")