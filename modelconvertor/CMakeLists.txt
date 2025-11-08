project(modelconvertor)

file(GLOB_RECURSE MODEL_CONVERTOR_SRC CONFIGURE_DEPENDS src/*.cpp)
add_subdirectory(tinyobjloader)

add_executable(${PROJECT_NAME} ${MODEL_CONVERTOR_SRC})
include("${CMAKE_SOURCE_DIR}/cmake/General.cmake")

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        tiny_obj
        lz4_static
        rpmalloc
        ext
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_SOURCE_DIR}/engine/include
        ${CMAKE_SOURCE_DIR}/engine/external/glm
)
