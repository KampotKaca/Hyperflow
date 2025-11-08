project(d3d)

file(GLOB_RECURSE D3D_SRC CONFIGURE_DEPENDS src/*.cpp)

add_library(${PROJECT_NAME} SHARED ${D3D_SRC})
include("${CMAKE_SOURCE_DIR}/cmake/General.cmake")
target_precompile_headers(${PROJECT_NAME} PRIVATE "include/hd3d_shared.h")
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ON)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        d3d11
        dxgi
        ext
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ../include
        ${CMAKE_SOURCE_DIR}/engine/include
        ${CMAKE_SOURCE_DIR}/engine/platform/windows/include
)

target_compile_definitions(${PROJECT_NAME} PRIVATE RENDERER_DLL)
include("${CMAKE_SOURCE_DIR}/cmake/ExportAsDll.cmake")