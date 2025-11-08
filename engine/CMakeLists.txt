project(Hyperflow_Engine)

if(D3D)
    add_subdirectory(rendering/d3d)
endif ()

if (VULKAN)
    add_subdirectory(rendering/vulkan)
endif ()

file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS src/*.cpp)

add_subdirectory(platform)
add_subdirectory(components)

add_subdirectory(others/exception)

add_library(${PROJECT_NAME} ${ENGINE_SRC})
target_sources(${PROJECT_NAME}
        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES include/hshared.h
)

target_precompile_headers(${PROJECT_NAME} PRIVATE "include/hshared.h")
include("${CMAKE_SOURCE_DIR}/cmake/AssetFolder.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/General.cmake")
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ON)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        stb
        lz4_static
        ext
        components
        PRIVATE
        ryml
        rpmalloc
        platform
        miniaudio
)

if(D3D)
    add_dependencies(${PROJECT_NAME} d3d)
endif ()

if(VULKAN)
    add_dependencies(${PROJECT_NAME} vk)
endif ()

target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        BUILDING_HYPERFLOW
)

# Set include directories
target_include_directories(${PROJECT_NAME}
        PRIVATE
        "${CMAKE_SOURCE_DIR}/external/rapidyaml/src"
        "${CMAKE_SOURCE_DIR}/external/stb/include"
        "${CMAKE_SOURCE_DIR}/external/rpmalloc"
        "${CMAKE_SOURCE_DIR}/external/glfw/include"
        "${CMAKE_SOURCE_DIR}/external/miniaudio"

        platform/include
        rendering/include
        "${CMAKE_SOURCE_DIR}/modelconvertor/include"
)

get_target_property(LIBS ${PROJECT_NAME} LINK_LIBRARIES)
message(STATUS "Linking libraries: ${LIBS}")