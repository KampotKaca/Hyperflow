project(shaderconvertor)

set(SHADER_CONVERTOR_SRC src/hshaderconvertor.cpp)

if (VULKAN)
    list(APPEND SHADER_CONVERTOR_SRC src/hshadervulkan.cpp)
endif ()

if (D3D)
    list(APPEND SHADER_CONVERTOR_SRC src/hshaderd3d.cpp)
endif ()

add_executable(${PROJECT_NAME} ${SHADER_CONVERTOR_SRC})
include("${CMAKE_SOURCE_DIR}/cmake/General.cmake")

target_include_directories(${PROJECT_NAME}
        PRIVATE
        includes/
        include/
        ${CMAKE_SOURCE_DIR}/engine/include
        ${CMAKE_SOURCE_DIR}/external/shaderc/glslc/src
        ${CMAKE_SOURCE_DIR}/external/shaderc/libshaderc/include
)

if (VULKAN)
    target_link_libraries(${PROJECT_NAME} PRIVATE glslc)
    target_include_directories(${PROJECT_NAME}
            PRIVATE
            ${CMAKE_SOURCE_DIR}/external/shaderc/glslc/src
            ${CMAKE_SOURCE_DIR}/external/shaderc/libshaderc/include
    )
endif ()
