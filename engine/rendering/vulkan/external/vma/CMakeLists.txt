file(GLOB_RECURSE VMA_SRC CONFIGURE_DEPENDS src/*.cpp)
add_library(vma ${VMA_SRC})
set_target_properties(vma PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_options(vma PRIVATE -fPIC)

find_package(Vulkan REQUIRED)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_libraries(vma PRIVATE Vulkan::Vulkan)
    else ()
        message(FATAL_ERROR "Vulkan libraries on windows can only be compiled with gcc")
    endif()
else ()
    target_link_libraries(vma PRIVATE Vulkan::Vulkan)
endif ()

target_include_directories(vma PUBLIC include)