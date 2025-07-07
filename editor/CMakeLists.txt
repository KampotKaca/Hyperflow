cmake_minimum_required(VERSION 3.30)

project(Hyperflow_Editor)
set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE EDITOR_SRC CONFIGURE_DEPENDS src/*.cpp)

add_subdirectory(external/ImGui)

add_library(${PROJECT_NAME} ${EDITOR_SRC})

target_precompile_headers(${PROJECT_NAME}
        PRIVATE
        "../engine/include/hshared.h"
)
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ON)

target_link_libraries(${PROJECT_NAME}
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
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        src/
        include/
        PRIVATE
        external/stb/include
)