
if(PRODUCTION_BUILD_DIR)
    set(EXECUTABLE_FOLDER app)
    if(ENABLE_EDITOR)
        set(EXECUTABLE_NAME Hyperflow_Editor)
    else ()
        set(EXECUTABLE_NAME Hyperflow_Application)
    endif ()

    set(EXPORT_DIR "../../${PRODUCTION_BUILD_DIR}")

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${EXPORT_DIR}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${EXPORT_DIR}/res"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${EXPORT_DIR}/${EXECUTABLE_FOLDER}"

            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_BINARY_DIR}/res" "${EXPORT_DIR}/res"
            COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${EXECUTABLE_NAME}> -o "${EXPORT_DIR}/${EXECUTABLE_FOLDER}/$<TARGET_FILE_NAME:${EXECUTABLE_NAME}>"

            COMMENT "Stripping and exporting production build to ${EXPORT_DIR}"
    )

    if (WINDOWS)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND cmd /c "for %f in (\"$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>\\*.dll\") do copy /Y \"%f\" \"${EXPORT_DIR}/${EXECUTABLE_FOLDER}/\""
        )
    elseif (LINUX)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/*.so
                "${EXPORT_DIR}/${EXECUTABLE_FOLDER}"
        )
    endif ()

endif()