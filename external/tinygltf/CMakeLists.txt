project(tinygltf)

file(GLOB_RECURSE TINY_GLTF_SRC CONFIGURE_DEPENDS *.cpp)
add_library(${PROJECT_NAME} ${TINY_GLTF_SRC})
target_sources(${PROJECT_NAME}
        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
        FILES tiny_gltf.h
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${CMAKE_SOURCE_DIR}/external/stb/include
)
