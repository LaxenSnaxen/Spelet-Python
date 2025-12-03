cmake_minimum_required(VERSION 3.10)

project(Spelet LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build BearLibTerminal from the cloned repo under build/bearlibterminal
add_subdirectory(${CMAKE_SOURCE_DIR}/build/bearlibterminal/Terminal ${CMAKE_BINARY_DIR}/bearlibterminal_build)

# Build miniaudio as a simple static library from the single-file implementation
add_library(miniaudio STATIC ${CMAKE_SOURCE_DIR}/build/miniaudio/miniaudio.c)
target_include_directories(miniaudio PUBLIC ${CMAKE_SOURCE_DIR}/build/miniaudio)

# Explicitly list application sources (avoid automatically including audio.cpp)
set(APP_SOURCES
	${CMAKE_SOURCE_DIR}/src/main.cpp
	${CMAKE_SOURCE_DIR}/src/inventory.cpp
)
add_executable(Spelet ${APP_SOURCES})
target_include_directories(Spelet PRIVATE ${CMAKE_SOURCE_DIR}/build/bearlibterminal/Terminal/Include/C ${CMAKE_SOURCE_DIR}/src)

# Link against BearLibTerminal and miniaudio
target_link_libraries(Spelet PRIVATE BearLibTerminal miniaudio)

# Place the built exe in a predictable location
set_target_properties(Spelet PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
