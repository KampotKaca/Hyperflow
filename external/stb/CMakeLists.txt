project(stb)

file(GLOB_RECURSE STB_SRC CONFIGURE_DEPENDS src/*.c)
add_library(${PROJECT_NAME} ${STB_SRC})
target_sources(${PROJECT_NAME}
        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
        include/stb_image.h
        include/stb_image_write.h
        include/stb_image_resize2.h
)

target_include_directories(${PROJECT_NAME} PUBLIC include)