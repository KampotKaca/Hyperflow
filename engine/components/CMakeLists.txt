project(components)

file(GLOB_RECURSE COMPONENTS_SRC CONFIGURE_DEPENDS src/*.cpp)
add_library(${PROJECT_NAME} ${COMPONENTS_SRC})
include("${CMAKE_SOURCE_DIR}/cmake/General.cmake")
target_precompile_headers(${PROJECT_NAME} PRIVATE "../include/hshared.h")
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ON)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        stb
        ext
)

target_include_directories(${PROJECT_NAME} PUBLIC ../include)
