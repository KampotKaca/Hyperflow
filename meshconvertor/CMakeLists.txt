project(meshconvertor)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE MESH_CONVERTOR_SRC CONFIGURE_DEPENDS src/*.cpp)
add_subdirectory(tinyobjloader)

add_executable(${PROJECT_NAME} ${MESH_CONVERTOR_SRC})

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        tiny_obj
        glm
        logc
        ext
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        include
        ${CMAKE_SOURCE_DIR}/engine/include
        ${CMAKE_SOURCE_DIR}/engine/external/glm
)
