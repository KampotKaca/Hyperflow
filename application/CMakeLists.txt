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
target_precompile_headers(${PROJECT_NAME} PRIVATE "../engine/include/hyperflow.h")
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ON)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        include/
        PRIVATE
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
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

# === Shader Setup ===
set(SHADER_SOURCE_DIRS
        "${CMAKE_SOURCE_DIR}/engine/res/shaders/vulkan"
        "${CMAKE_SOURCE_DIR}/application/assets/shaders/vulkan"
        # Add more shader source directories here
)

set(SHADER_BINARY_DIR "${CMAKE_BINARY_DIR}/res/shaders/vulkan")
file(MAKE_DIRECTORY ${SHADER_BINARY_DIR})
set(COMPILED_SHADERS)

foreach(SHADER_SOURCE_DIR ${SHADER_SOURCE_DIRS})
    file(GLOB_RECURSE SHADER_SOURCES_TMP
            "${SHADER_SOURCE_DIR}/*.vert"
            "${SHADER_SOURCE_DIR}/*.frag"
    )

    foreach(SHADER ${SHADER_SOURCES_TMP})
        get_filename_component(FILE_NAME ${SHADER} NAME)
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
endforeach()

# === Mesh Setup ===
set(MESH_SOURCE_DIRS
        "${CMAKE_SOURCE_DIR}/engine/res/meshes"
        "${CMAKE_SOURCE_DIR}/application/assets/meshes"
        # Add more mesh source directories here
)

set(MESH_BINARY_DIR "${CMAKE_BINARY_DIR}/res/meshes")
file(MAKE_DIRECTORY ${MESH_BINARY_DIR})
set(CONVERTED_MESHES)

foreach(MESH_SOURCE_DIR ${MESH_SOURCE_DIRS})
    file(GLOB_RECURSE MESH_SOURCES_TMP
            "${MESH_SOURCE_DIR}/*.obj"
    )

    foreach(MESH ${MESH_SOURCES_TMP})
        get_filename_component(FILE_NAME ${MESH} NAME)
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
endforeach()

# === Prebuild Target ===
add_custom_target(Prebuild
        ALL DEPENDS ${CONVERTED_MESHES} ${COMPILED_SHADERS}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/application/res ${CMAKE_BINARY_DIR}/res
)

#endregion
add_dependencies(${CMAKE_PROJECT_NAME} Prebuild)
