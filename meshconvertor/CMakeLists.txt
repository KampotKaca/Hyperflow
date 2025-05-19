if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -funroll-loops")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -funroll-loops")
endif()
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
