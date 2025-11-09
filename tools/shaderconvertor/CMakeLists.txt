project(shaderconvertor)

file(GLOB_RECURSE SHADER_CONVERTOR_SRC CONFIGURE_DEPENDS *.cpp)

add_executable(${PROJECT_NAME} ${SHADER_CONVERTOR_SRC})
include("${CMAKE_SOURCE_DIR}/cmake/General.cmake")

file(COPY ${CMAKE_SOURCE_DIR}/tools/shaderconvertor/includes DESTINATION ${CMAKE_BINARY_DIR}/tools/shaderconvertor)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        ext
        glslc
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        includes/
        ${CMAKE_SOURCE_DIR}/engine/include
        ${CMAKE_SOURCE_DIR}/external/shaderc/glslc/src
)
