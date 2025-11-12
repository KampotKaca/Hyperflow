project(modelconvertor)

file(GLOB_RECURSE MODEL_CONVERTOR_SRC CONFIGURE_DEPENDS src/*.cpp)

add_executable(${PROJECT_NAME} ${MODEL_CONVERTOR_SRC})
include("${CMAKE_SOURCE_DIR}/cmake/General.cmake")

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        tinyobj
        tinygltf
        ufbx
        lz4_static
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_SOURCE_DIR}/external/tinyobj/include
        ${CMAKE_SOURCE_DIR}/external/tinygltf/include
        ${CMAKE_SOURCE_DIR}/external/ufbx/include
        ${CMAKE_SOURCE_DIR}/engine/include
)
