project(rpmalloc)

file(GLOB_RECURSE RPMALLOC_SRC CONFIGURE_DEPENDS *.c)
file(GLOB_RECURSE RPMALLOC_PSRC CONFIGURE_DEPENDS *.cpp)
add_library(${PROJECT_NAME} ${RPMALLOC_SRC} ${RPMALLOC_PSRC})

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        logc
        glm
        logc
        svector
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        "${CMAKE_SOURCE_DIR}/engine/include"
        "${CMAKE_SOURCE_DIR}/external/logc/include"
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        include/
        "${CMAKE_SOURCE_DIR}/external/glm"
        "${CMAKE_SOURCE_DIR}/external/logc/include"
        "${CMAKE_SOURCE_DIR}/external/svector/include"
        "${CMAKE_SOURCE_DIR}/external/phmap"
        "${CMAKE_SOURCE_DIR}/engine/platform/include"
        "${CMAKE_SOURCE_DIR}/engine/rendering/include"
        "${CMAKE_SOURCE_DIR}/engine/components/include"
        "${CMAKE_SOURCE_DIR}/external/rpmalloc"
        ${CMAKE_SOURCE_DIR}
)

target_sources(${PROJECT_NAME}
        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
        "${CMAKE_SOURCE_DIR}/external/phmap"
        "${CMAKE_SOURCE_DIR}/external/svector/include"
        FILES
        "${CMAKE_SOURCE_DIR}/external/phmap/btree.h"
        "${CMAKE_SOURCE_DIR}/external/phmap/phmap.h"
        "${CMAKE_SOURCE_DIR}/external/svector/include/ankerl/svector.h"
)