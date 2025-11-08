project(Hyperflow_Application)
file(GLOB_RECURSE PROJECT_SRC CONFIGURE_DEPENDS src/*.cpp)

if(ENABLE_EDITOR)
    add_library(${PROJECT_NAME} ${PROJECT_SRC})
    message(STATUS "Application is Library!")
else ()
    add_executable(${PROJECT_NAME} ${PROJECT_SRC} main.cpp)
    message(STATUS "Application is Executable!")
endif ()

target_precompile_headers(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/engine/include/hyperflow.h")

include("${CMAKE_SOURCE_DIR}/cmake/AssetFolder.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/General.cmake")
add_dependencies(${PROJECT_NAME} "${PROJECT_NAME}_Prebuild" "Hyperflow_Engine_Prebuild")

set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ON)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        PRIVATE
        Hyperflow_Engine
)

if(ENABLE_EDITOR)

else ()
    include("${CMAKE_SOURCE_DIR}/cmake/Export.cmake")
endif ()