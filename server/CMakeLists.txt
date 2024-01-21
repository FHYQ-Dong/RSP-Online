# Minimum required CMake version
cmake_minimum_required(VERSION 3.12)

# C_version
set(CMAKE_C_STANDARD 99)
# CXX_version
set(CMAKE_CXX_STANDARD 11)

# Project name
project(RSP-server)

# Add your source files here
AUX_SOURCE_DIRECTORY(src/ SOURCES)
# Add your header filessource directory here
AUX_SOURCE_DIRECTORY(include/ HEADERS)

# Add executable target
add_executable(${PROJECT_NAME} server.cpp ${SOURCES} ${HEADERS})

# Link Libraries
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ws2_32.lib)

# install target
install(TARGETS ${PROJECT_NAME} DESTINATION bin)