file(GLOB_RECURSE RPMALLOC_SRC CONFIGURE_DEPENDS *.c)
file(GLOB_RECURSE RPMALLOC_PSRC CONFIGURE_DEPENDS *.cpp)
add_library(rpmalloc ${RPMALLOC_SRC} ${RPMALLOC_PSRC})

target_link_libraries(vk
        PRIVATE
        ext
)

target_include_directories(rpmalloc
        PRIVATE
        ../../include
        ../../others/exception/include
        ../../external/logc/include
        ../../external/glm
)
