project(rpmalloc)

file(GLOB_RECURSE RPMALLOC_SRC CONFIGURE_DEPENDS *.c)
file(GLOB_RECURSE RPMALLOC_PSRC CONFIGURE_DEPENDS *.cpp)
add_library(${PROJECT_NAME} ${RPMALLOC_SRC} ${RPMALLOC_PSRC})
include("${CMAKE_SOURCE_DIR}/cmake/General.cmake")

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        ext
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_SOURCE_DIR}/engine/others/exception/include
        ${CMAKE_SOURCE_DIR}/engine/include
)
