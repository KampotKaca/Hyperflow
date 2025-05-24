cmake_minimum_required(VERSION 3.30)

file(READ "appconfig.h" CONFIG_HEADER_CONTENTS)

string(REGEX MATCH "#define VERSION \"([0-9]+\\.[0-9]+\\.[0-9]+)\"" VERSION_MATCH "${CONFIG_H_CONTENTS}")

# Set CMake project version
if(VERSION_MATCH)
    project(Hyperflow VERSION ${CMAKE_MATCH_1})
else()
    project(Hyperflow VERSION 0.0.0)
endif()

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE PROJECT_SRC CONFIGURE_DEPENDS src/*.cpp)

add_executable(${PROJECT_NAME} ${PROJECT_SRC})

target_include_directories(${PROJECT_NAME}
        PUBLIC
        include/
        PRIVATE
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        ryml
        PRIVATE
        Hyperflow_Engine
)

#region Prebuild
find_program(GLSLC_EXECUTABLE glslc REQUIRED)

if(NOT GLSLC_EXECUTABLE)
    message(FATAL_ERROR "glslc not found! Please install Vulkan SDK or add glslc to PATH.")
endif()

file(COPY ${CMAKE_SOURCE_DIR}/application/res DESTINATION ${CMAKE_BINARY_DIR})
target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RES_PATH="${CMAKE_BINARY_DIR}/res/")

set(SHADER_SOURCE_DIR "${CMAKE_SOURCE_DIR}/application/assets/shaders/vulkan")
set(SHADER_BINARY_DIR "${CMAKE_BINARY_DIR}/res/shaders/vulkan")

# Find all shader source files
file(GLOB_RECURSE SHADER_SOURCES
        "${SHADER_SOURCE_DIR}/*.vert"
        "${SHADER_SOURCE_DIR}/*.frag"
)

# Create the output directory
file(MAKE_DIRECTORY ${SHADER_BINARY_DIR})

# List to hold all compiled shader files
set(COMPILED_SHADERS)

foreach(SHADER ${SHADER_SOURCES})
    # Get the filename without path
    get_filename_component(FILE_NAME ${SHADER} NAME)
    # Output file path
    set(OUTPUT_FILE "${SHADER_BINARY_DIR}/${FILE_NAME}.spv")

    add_custom_command(
            OUTPUT ${OUTPUT_FILE}
            COMMAND ${GLSLC_EXECUTABLE} ${SHADER} -o ${OUTPUT_FILE}
            DEPENDS ${SHADER}
            COMMENT "Compiling shader ${FILE_NAME}"
            VERBATIM
    )

    list(APPEND COMPILED_SHADERS ${OUTPUT_FILE})
endforeach()

set(MESH_SOURCE_DIR "${CMAKE_SOURCE_DIR}/application/assets/meshes")
set(MESH_BINARY_DIR "${CMAKE_BINARY_DIR}/res/meshes")

file(MAKE_DIRECTORY ${MESH_BINARY_DIR})

file(GLOB_RECURSE MESH_SOURCES
        "${MESH_SOURCE_DIR}/*.obj"
)

set(CONVERTED_MESHES)

foreach(MESH ${MESH_SOURCES})
    # Get the filename without path
    get_filename_component(FILE_NAME ${MESH} NAME)
    # Output file path
    set(OUTPUT_FILE "${MESH_BINARY_DIR}/${FILE_NAME}.mesh")

    add_custom_command(
            OUTPUT ${OUTPUT_FILE}
            COMMAND meshconvertor
            ARGS
            "${MESH}"
            "${OUTPUT_FILE}"
            DEPENDS
            meshconvertor
            ${MESH}
            COMMENT "Converting mesh: ${FILE_NAME}"
            VERBATIM
    )

    list(APPEND CONVERTED_MESHES ${OUTPUT_FILE})
endforeach()

add_custom_target(Prebuild
        ALL DEPENDS ${CONVERTED_MESHES} ${COMPILED_SHADERS}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/application/res ${CMAKE_BINARY_DIR}/res
)

#endregion
add_dependencies(${CMAKE_PROJECT_NAME} Prebuild)
