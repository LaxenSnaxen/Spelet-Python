cmake_minimum_required(VERSION 3.10)

project(Spelet LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Put all output artifacts into known locations to avoid Makefile/dependency
# issues when building with MinGW/MSYS2. Static libs -> lib, shared -> lib,
# executables -> bin under the top-level binary directory.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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

# Ensure the BearLibTerminal DLL is available next to the executable so
# Visual Studio / Windows can find it at runtime. Copy the DLL (if any)
# into the `Spelet` runtime directory after building the `Spelet` target.
if(TARGET BearLibTerminal)
	add_custom_command(TARGET Spelet POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:BearLibTerminal>
			$<TARGET_FILE_DIR:Spelet>
	)
endif()
