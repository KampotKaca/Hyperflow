file(GLOB_RECURSE D3D_SRC CONFIGURE_DEPENDS src/*.cpp)

add_library(d3d SHARED ${D3D_SRC})
target_precompile_headers(d3d PRIVATE "include/hd3d_shared.h")
set_target_properties(d3d PROPERTIES UNITY_BUILD ON)

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