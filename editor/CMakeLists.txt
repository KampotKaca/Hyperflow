project(Hyperflow_Editor)

file(GLOB_RECURSE EDITOR_SRC CONFIGURE_DEPENDS src/*.cpp)

add_executable(${PROJECT_NAME} ${EDITOR_SRC} main.cpp)
target_sources(${PROJECT_NAME}
        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES include/heditorinternal.h
)
include("${CMAKE_SOURCE_DIR}/cmake/General.cmake")
target_precompile_headers(${PROJECT_NAME}
        PRIVATE
        include/heditorinternal.h
)
#set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ON)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        Hyperflow_Engine
        Hyperflow_Application

        stb
        ryml
        ext
        components
        imgui
        PRIVATE
        glfw
)

if(LINUX)
    find_package(X11 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${X11_LIBRARIES})
endif ()

include("${CMAKE_SOURCE_DIR}/cmake/Vulkan.cmake")

target_include_directories(${PROJECT_NAME}
        PUBLIC
        "${CMAKE_SOURCE_DIR}/application/include"
        "${CMAKE_SOURCE_DIR}/engine/include"
        "${CMAKE_SOURCE_DIR}/external/imgui/"
        "${CMAKE_SOURCE_DIR}/external/glfw/include"
)

if(ENABLE_EDITOR)
    include("${CMAKE_SOURCE_DIR}/cmake/Export.cmake")
endif ()