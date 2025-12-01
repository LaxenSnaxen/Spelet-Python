cmake_minimum_required(VERSION 3.16)
project(Spelet VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)

# BearLibTerminal
find_library(BEARLIBTERMINAL_LIBRARY BearLibTerminal)
find_path(BEARLIBTERMINAL_INCLUDE_DIR BearLibTerminal.h)

if(NOT BEARLIBTERMINAL_LIBRARY OR NOT BEARLIBTERMINAL_INCLUDE_DIR)
    message(STATUS "BearLibTerminal not found in system paths, will use pkg-config or manual paths")
endif()

# OpenAL
find_package(OpenAL REQUIRED)

# Source files
set(SOURCES
    src/main.cpp
    src/audio.cpp
)

set(HEADERS
    src/audio.hpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${BEARLIBTERMINAL_INCLUDE_DIR}
    ${OPENAL_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${BEARLIBTERMINAL_LIBRARY}
    ${OPENAL_LIBRARY}
)

# Platform-specific settings
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
endif()

# Copy assets if they exist
if(EXISTS ${CMAKE_SOURCE_DIR}/assets)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    )
endif()
