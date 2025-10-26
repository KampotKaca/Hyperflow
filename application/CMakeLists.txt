cmake_minimum_required(VERSION 3.30)

file(READ "appconfig.h" CONFIG_HEADER_CONTENTS)

string(REGEX MATCH "#define VERSION \"([0-9]+\\.[0-9]+\\.[0-9]+)\"" VERSION_MATCH "${CONFIG_H_CONTENTS}")

# Set CMake project version
if(VERSION_MATCH)
    project(Hyperflow VERSION ${CMAKE_MATCH_1})
else()
    project(Hyperflow VERSION 0.0.0)
endif()

file(GLOB_RECURSE PROJECT_SRC CONFIGURE_DEPENDS src/*.cpp)

add_executable(${PROJECT_NAME} ${PROJECT_SRC})
target_precompile_headers(${PROJECT_NAME} PRIVATE "../engine/include/hyperflow.h")
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ON)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        include/
        ../editor/include
        PRIVATE
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        PRIVATE
        Hyperflow_Engine
)

if(EDITOR)
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        Hyperflow_Editor
)
endif ()

#region Prebuild
file(COPY ${CMAKE_SOURCE_DIR}/application/res DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/engine/res DESTINATION ${CMAKE_BINARY_DIR})

# === Shader Setup ===
set(SHADER_SOURCE_DIRS
        "${CMAKE_SOURCE_DIR}/engine/assets/shaders/vulkan"
        "${CMAKE_SOURCE_DIR}/application/assets/shaders/vulkan"
        # Add more shader source directories here
)

set(SHADER_BINARY_DIR "${CMAKE_BINARY_DIR}/res/shaders/vulkan")
file(MAKE_DIRECTORY ${SHADER_BINARY_DIR})

set(SHADERCONVERTOR_ARGS)
set(PROCESSED_SHADERS)

foreach(SHADER_SOURCE_DIR ${SHADER_SOURCE_DIRS})
    file(GLOB_RECURSE SHADER_SOURCES_TMP
            "${SHADER_SOURCE_DIR}/*.vert"
            "${SHADER_SOURCE_DIR}/*.frag"
    )

    foreach(SHADER ${SHADER_SOURCES_TMP})
        get_filename_component(FILE_NAME ${SHADER} NAME)
        set(OUTPUT_FILE "${SHADER_BINARY_DIR}/${FILE_NAME}.spv")

        list(APPEND SHADERCONVERTOR_ARGS "${SHADER}" "${OUTPUT_FILE}")
        list(APPEND PROCESSED_SHADERS "${OUTPUT_FILE}")
    endforeach()
endforeach()

# === Model Setup ===
set(MODEL_SOURCE_DIRS
        "${CMAKE_SOURCE_DIR}/engine/assets/models"
        "${CMAKE_SOURCE_DIR}/application/assets/models"
        # Add more model source directories here
)

set(MODEL_BINARY_DIR "${CMAKE_BINARY_DIR}/res/models")
file(MAKE_DIRECTORY ${MODEL_BINARY_DIR})
set(MODELCONVERTOR_ARGS)
set(PROCESSED_MODELS)

foreach(MODEL_SOURCE_DIR ${MODEL_SOURCE_DIRS})
    file(GLOB_RECURSE MODEL_SOURCES_TMP
            "${MODEL_SOURCE_DIR}/*.obj"
            "${MODEL_SOURCE_DIR}/*.gltf"
            "${MODEL_SOURCE_DIR}/*.glb"
            "${MODEL_SOURCE_DIR}/*.fbx"
    )

    foreach(MODEL ${MODEL_SOURCES_TMP})
        get_filename_component(FILE_NAME ${MODEL} NAME)
        set(OUTPUT_FILE "${MODEL_BINARY_DIR}/${FILE_NAME}.model")

        list(APPEND MODELCONVERTOR_ARGS "${MODEL}" "${OUTPUT_FILE}")
        list(APPEND PROCESSED_MODELS "${OUTPUT_FILE}")
    endforeach()
endforeach()

# === Prebuild Target ===
add_custom_target(Prebuild ALL
#        ALL DEPENDS ${PROCESSED_MODELS} ${PROCESSED_SHADERS}
        COMMAND ${CMAKE_COMMAND} -E echo "Running modelconvertor..."
        COMMAND modelconvertor ${MODELCONVERTOR_ARGS}
        COMMAND ${CMAKE_COMMAND} -E echo "Running shaderconvertor..."
        COMMAND shaderconvertor ${SHADERCONVERTOR_ARGS}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/application/res ${CMAKE_BINARY_DIR}/res
)

#endregion
add_dependencies(${CMAKE_PROJECT_NAME} Prebuild)

if(PRODUCTION_BUILD_DIR)
    set(EXPORT_DIR "../../${PRODUCTION_BUILD_DIR}")

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${EXPORT_DIR}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${EXPORT_DIR}/res"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${EXPORT_DIR}/application"

            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_BINARY_DIR}/res" "${EXPORT_DIR}/res"
            COMMAND ${CMAKE_STRIP} $<TARGET_FILE:Hyperflow> -o "${EXPORT_DIR}/application/$<TARGET_FILE_NAME:Hyperflow>"

            COMMENT "Stripping and exporting production build to ${EXPORT_DIR}"
    )

    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND cmd /c "for %f in (\"$<TARGET_FILE_DIR:Hyperflow>\\*.dll\") do copy /Y \"%f\" \"${EXPORT_DIR}/application/\""
        )
    elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE_DIR:Hyperflow>/*.so
                "${EXPORT_DIR}/application"
        )
    endif ()

endif()