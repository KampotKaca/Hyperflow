
if(PRODUCTION_BUILD_DIR)
    set(EXPORT_DIR "../../${PRODUCTION_BUILD_DIR}")

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${EXPORT_DIR}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${EXPORT_DIR}/res"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${EXPORT_DIR}/${EXECUTABLE_FOLDER}"

            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_BINARY_DIR}/res" "${EXPORT_DIR}/res"
            COMMAND ${CMAKE_STRIP} $<TARGET_FILE:Hyperflow> -o "${EXPORT_DIR}/${EXECUTABLE_FOLDER}/$<TARGET_FILE_NAME:Hyperflow>"

            COMMENT "Stripping and exporting production build to ${EXPORT_DIR}"
    )

    if (WINDOWS)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND cmd /c "for %f in (\"$<TARGET_FILE_DIR:Hyperflow>\\*.dll\") do copy /Y \"%f\" \"${EXPORT_DIR}/${EXECUTABLE_FOLDER}/\""
        )
    elseif (LINUX)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE_DIR:Hyperflow>/*.so
                "${EXPORT_DIR}/${EXECUTABLE_FOLDER}"
        )
    endif ()

endif()