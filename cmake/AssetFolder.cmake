
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_BINARY_DIR})

# === Shader Setup ===
set(SHADER_SOURCE_DIRS
        "assets/shaders/vulkan"
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
        "assets/models"
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
add_custom_target("${PROJECT_NAME}_Prebuild" ALL
        #        ALL DEPENDS ${PROCESSED_MODELS} ${PROCESSED_SHADERS}
        COMMAND ${CMAKE_COMMAND} -E echo "Running modelconvertor..."
        COMMAND modelconvertor ${MODELCONVERTOR_ARGS}
        COMMAND ${CMAKE_COMMAND} -E echo "Running shaderconvertor..."
        COMMAND shaderconvertor ${SHADERCONVERTOR_ARGS}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/res ${CMAKE_BINARY_DIR}/res
)
