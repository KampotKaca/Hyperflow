if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -funroll-loops")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -funroll-loops")
endif()
project(shaderconvertor)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SHADER_CONVERTOR_SRC CONFIGURE_DEPENDS *.cpp)

add_executable(${PROJECT_NAME} ${SHADER_CONVERTOR_SRC})

file(COPY ${CMAKE_SOURCE_DIR}/shaderconvertor/includes DESTINATION ${CMAKE_BINARY_DIR}/shaderconvertor)
find_program(GLSLC_EXECUTABLE glslc REQUIRED)

if(NOT GLSLC_EXECUTABLE)
    message(FATAL_ERROR "glslc not found! Please install Vulkan SDK or add glslc to PATH.")
endif()

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        glm
        logc
        ext
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        include
#        ${CMAKE_SOURCE_DIR}/engine/include
#        ${CMAKE_SOURCE_DIR}/engine/external/glm
)
