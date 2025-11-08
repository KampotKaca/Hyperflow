cmake_minimum_required(VERSION 3.30)

file(READ "appconfig.h" CONFIG_HEADER_CONTENTS)

string(REGEX MATCH "#define VERSION \"([0-9]+\\.[0-9]+\\.[0-9]+)\"" VERSION_MATCH "${CONFIG_H_CONTENTS}")

# Set CMake project version
if(VERSION_MATCH)
    project(Hyperflow VERSION ${CMAKE_MATCH_1})
else()
    project(Hyperflow VERSION 0.0.0)
endif()

file(GLOB_RECURSE PROJECT_SRC CONFIGURE_DEPENDS src/*.cpp)

add_executable(${PROJECT_NAME} ${PROJECT_SRC})
target_precompile_headers(${PROJECT_NAME} PRIVATE "../engine/include/hyperflow.h")
set_target_properties(${PROJECT_NAME} PROPERTIES UNITY_BUILD ON)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        include/
        ../editor/include
        PRIVATE
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        PRIVATE
        Hyperflow_Engine
)

if(EDITOR)
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        Hyperflow_Editor
)
endif ()

include(${CMAKE_SOURCE_DIR}/cmake/AssetFolder.cmake)
add_dependencies(${CMAKE_PROJECT_NAME} "${PROJECT_NAME}_Prebuild" "Hyperflow_Engine_Prebuild")

if(PRODUCTION_BUILD_DIR)
    set(EXPORT_DIR "../../${PRODUCTION_BUILD_DIR}")

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${EXPORT_DIR}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${EXPORT_DIR}/res"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${EXPORT_DIR}/application"

            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_BINARY_DIR}/res" "${EXPORT_DIR}/res"
            COMMAND ${CMAKE_STRIP} $<TARGET_FILE:Hyperflow> -o "${EXPORT_DIR}/application/$<TARGET_FILE_NAME:Hyperflow>"

            COMMENT "Stripping and exporting production build to ${EXPORT_DIR}"
    )

    if (WINDOWS)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND cmd /c "for %f in (\"$<TARGET_FILE_DIR:Hyperflow>\\*.dll\") do copy /Y \"%f\" \"${EXPORT_DIR}/application/\""
        )
    elseif (LINUX)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE_DIR:Hyperflow>/*.so
                "${EXPORT_DIR}/application"
        )
    endif ()

endif()