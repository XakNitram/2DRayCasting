# Add the project files to the executable.
add_executable(
        ray-casting Application.cpp

        # CASTERS
        Casters/Caster.h
        Casters/Caster.cpp
        Casters/AngleCaster.h
        Casters/AngleCaster.cpp
        Casters/EndPointCaster.h
        Casters/EndPointCaster.cpp

        # CORE
        Core/Utils.h
        Core/Utils.cpp
        Core/Simulation.h
        Core/Simulation.cpp

        # MATH
        Math/Geometrics.h
        Math/Geometrics.cpp
        Math/Boundary.h
        Math/Boundary.cpp

        # PRIMITIVES
        Primitives/Floor.h
        Primitives/Floor.cpp
        Primitives/Quad.h
        Primitives/Quad.cpp

        # RENDERER
        Renderer/GLUtils.h
        Renderer/GLDebug.h
        Renderer/GLDebug.cpp
        Renderer/Shader.h
        Renderer/Shader.cpp
        Renderer/VertexArray.h
        Renderer/VertexArray.cpp
)

# Set src/ as an include directory so files in subdirectories can find each other.
target_include_directories(ray-casting PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

# Link in glad and glfw libraries.
target_link_libraries(ray-casting PRIVATE glad)
target_link_libraries(ray-casting PRIVATE glfw)

# Use precompiled headers.
target_precompile_headers(ray-casting PRIVATE pch.h pch.cpp)

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

add_custom_target(
        shaders ALL
        DEPENDS
            ${CMAKE_BINARY_DIR}/bin/Shaders/default.frag
            ${CMAKE_BINARY_DIR}/bin/Shaders/default.vert
            ${CMAKE_BINARY_DIR}/bin/Shaders/floor.frag
            ${CMAKE_BINARY_DIR}/bin/Shaders/floor.vert
            ${CMAKE_BINARY_DIR}/bin/Shaders/light.frag
            ${CMAKE_BINARY_DIR}/bin/Shaders/light.vert
#            ${CMAKE_BINARY_DIR}/bin/Shaders/mazing.frag
#            ${CMAKE_BINARY_DIR}/bin/Shaders/spacetime.frag
)
add_dependencies(ray-casting shaders)

# Copy shaders to output directory.
add_custom_command(
        DEPENDS
            ${CMAKE_SOURCE_DIR}/res/Shaders/default.frag
            ${CMAKE_SOURCE_DIR}/res/Shaders/default.vert
            ${CMAKE_SOURCE_DIR}/res/Shaders/floor.frag
            ${CMAKE_SOURCE_DIR}/res/Shaders/floor.vert
            ${CMAKE_SOURCE_DIR}/res/Shaders/light.frag
            ${CMAKE_SOURCE_DIR}/res/Shaders/light.vert
#            ${CMAKE_SOURCE_DIR}/res/Shaders/mazing.frag
#            ${CMAKE_SOURCE_DIR}/res/Shaders/spacetime.frag
        OUTPUT
            ${CMAKE_BINARY_DIR}/bin/Shaders/default.frag
            ${CMAKE_BINARY_DIR}/bin/Shaders/default.vert
            ${CMAKE_BINARY_DIR}/bin/Shaders/floor.frag
            ${CMAKE_BINARY_DIR}/bin/Shaders/floor.vert
            ${CMAKE_BINARY_DIR}/bin/Shaders/light.frag
            ${CMAKE_BINARY_DIR}/bin/Shaders/light.vert
#            ${CMAKE_BINARY_DIR}/bin/Shaders/mazing.frag
#            ${CMAKE_BINARY_DIR}/bin/Shaders/spacetime.frag
        COMMENT "Copying shaders to output directory"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/res/Shaders/
        ${CMAKE_BINARY_DIR}/bin/Shaders/
)
