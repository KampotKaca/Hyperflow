cmake_minimum_required(VERSION 3.30)
project(Hyperflow VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")

file(GLOB_RECURSE PROJECT_SRC CONFIGURE_DEPENDS src/*.cpp)

add_executable(${PROJECT_NAME} ${PROJECT_SRC})

target_include_directories(${PROJECT_NAME}
        PUBLIC
        src/
        PRIVATE
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        PRIVATE
        Hyperflow_Engine
)

#region resources
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/application/res/") #DEBUG
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    file(COPY ${CMAKE_SOURCE_DIR}/application/res DESTINATION ${CMAKE_BINARY_DIR})
    target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RES_PATH="${CMAKE_BINARY_DIR}./res/") #BUILD
endif()

add_custom_target(copy_res ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/application/res ${CMAKE_BINARY_DIR}/res
        COMMENT "Copying resource files..."
)

# Make the custom target a dependency of the main project
add_dependencies(${CMAKE_PROJECT_NAME} copy_res)
#endregion