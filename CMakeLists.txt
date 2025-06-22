cmake_minimum_required(VERSION 3.30)
project(Hyperflow)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O0")

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -s -static -static-libgcc -static-libstdc++ -pthread")
    endif()

elseif(CMAKE_BUILD_TYPE STREQUAL "Release")

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -funroll-loops")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native -flto -funroll-loops")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -funroll-loops -s -static -static-libgcc -static-libstdc++")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native -funroll-loops")
    endif()
endif()

set(CMAKE_CXX_STANDARD 23)

set(PRODUCTION_BUILD_DIR "" CACHE STRING "Path to export stripped build")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DRELEASE)
endif()

add_subdirectory(meshconvertor)
add_subdirectory(shaderconvertor)
add_subdirectory(engine)
add_subdirectory(application)