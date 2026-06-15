cmake_minimum_required(VERSION 3.28.3)
project(RSA)

# Enforce C++20 and disable compiler-specific extensions
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(lib)
add_subdirectory(src)

target_link_libraries(RSA PRIVATE Base256)
target_include_directories(RSA PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})