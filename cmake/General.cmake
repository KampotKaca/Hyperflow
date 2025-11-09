
target_link_libraries(${PROJECT_NAME}
        PUBLIC
        glm
        logc
        svector
        rpmalloc
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