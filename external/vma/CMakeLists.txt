project(vma)

file(GLOB_RECURSE VMA_SRC CONFIGURE_DEPENDS src/*.cpp)
add_library(${PROJECT_NAME} ${VMA_SRC})
target_compile_options(${PROJECT_NAME} PRIVATE -fPIC)

find_package(Vulkan REQUIRED)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)
    else ()
        message(FATAL_ERROR "Vulkan libraries on windows can only be compiled with gcc")
    endif()
else ()
    target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)
endif ()

target_include_directories(${PROJECT_NAME} PUBLIC include)