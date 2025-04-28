if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
endif()

file(GLOB_RECURSE D3D_SRC CONFIGURE_DEPENDS src/*.cpp)

add_library(d3d SHARED ${D3D_SRC})

target_link_libraries(d3d
        PRIVATE
        d3d11
        dxgi
        glm
        logc
        ext
)

target_include_directories(d3d PUBLIC include)
target_include_directories(d3d PUBLIC ../include)
target_include_directories(d3d PUBLIC ../../include)
target_include_directories(d3d PUBLIC ../../platforms/windows)
target_compile_definitions(d3d PRIVATE RENDERER_DLL)

add_custom_command(TARGET d3d POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:d3d>
        $<TARGET_FILE_DIR:Hyperflow>
)