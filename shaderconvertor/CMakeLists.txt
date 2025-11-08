project(shaderconvertor)

file(GLOB_RECURSE SHADER_CONVERTOR_SRC CONFIGURE_DEPENDS *.cpp)

add_executable(${PROJECT_NAME} ${SHADER_CONVERTOR_SRC})
include("${CMAKE_SOURCE_DIR}/cmake/General.cmake")

file(COPY ${CMAKE_SOURCE_DIR}/shaderconvertor/includes DESTINATION ${CMAKE_BINARY_DIR}/shaderconvertor)
find_program(GLSLC_EXECUTABLE glslc REQUIRED)

if(NOT GLSLC_EXECUTABLE)
    message(FATAL_ERROR "glslc not found! Please install Vulkan SDK or add glslc to PATH.")
endif()

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        ext
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_SOURCE_DIR}/engine/include
)
