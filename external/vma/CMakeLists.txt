project(vma)

file(GLOB_RECURSE VMA_SRC CONFIGURE_DEPENDS *.cpp)
add_library(${PROJECT_NAME} ${VMA_SRC})
target_sources(${PROJECT_NAME}
        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
        FILES vk_mem_alloc.h
)
target_compile_options(${PROJECT_NAME} PRIVATE -fPIC)