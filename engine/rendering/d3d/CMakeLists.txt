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
)

target_include_directories(d3d PUBLIC include)
target_compile_definitions(d3d PRIVATE RENDERER_DLL)