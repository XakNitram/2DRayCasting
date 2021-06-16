cmake_minimum_required(VERSION 3.17)
project(RayCasting VERSION 2.2.0)

# Define the target here so other top-level targets can attach to it.
add_executable(ray-casting "")

# Mark the project as C++ 17 and set the output directory.
set_target_properties(
    ray-casting PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO

    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/
)

add_subdirectory(vendor)
add_subdirectory(src)
add_subdirectory(res)
