cmake_minimum_required(VERSION 3.30)

project(Hyperflow_Editor)

file(GLOB_RECURSE EDITOR_SRC CONFIGURE_DEPENDS src/*.cpp)

add_subdirectory(external/ImGui)

add_library(Hyperflow_Editor ${EDITOR_SRC})

target_precompile_headers(Hyperflow_Editor
        PRIVATE
        include/heditorinternal.h
)
set_target_properties(Hyperflow_Editor PROPERTIES UNITY_BUILD ON)

find_package(Vulkan REQUIRED)

target_link_libraries(Hyperflow_Editor
        PUBLIC
        Hyperflow_Engine
        glm
        stb
        logc
        ryml
        ext
        components
        PRIVATE
        imgui
        Vulkan::Vulkan
        glfw
)

target_include_directories(Hyperflow_Editor
        PUBLIC
        src/
        include/
        external/ImGui/
        ../engine/external/glfw/include
        ${Vulkan_INCLUDE_DIRS}
)