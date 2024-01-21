# Minimum required CMake version
cmake_minimum_required(VERSION 3.12)

# C_version
set(CMAKE_C_STANDARD 99)

# Project name
project(RSP-client)

# Build standalone executable
set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
set(CMAKE_EXE_LINKER_FLAGS "-static")

# Add your source files here
AUX_SOURCE_DIRECTORY(src/ SOURCES)
# Add your header filessource directory here
AUX_SOURCE_DIRECTORY(include/ HEADERS)
# Add your library files here
# AUX_SOURCE_DIRECTORY(lib/ LIBS)

# Add executable target
add_executable(${PROJECT_NAME} client.c ${SOURCES} ${HEADERS})

# Link Libraries
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ws2_32.lib)

# install target
install(TARGETS ${PROJECT_NAME} DESTINATION bin)