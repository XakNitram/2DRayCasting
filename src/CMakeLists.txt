# Add the project files to the executable.
add_executable(
        ray-casting Application.cpp

        # CASTERS
        Casters/Caster.hpp
        Casters/Caster.cpp
        Casters/AngleCaster.hpp
        Casters/AngleCaster.cpp
        Casters/EndPointCaster.hpp
        Casters/EndPointCaster.cpp

        # CORE
        Core/Utils.hpp
        Core/Utils.cpp
        Core/Simulation.hpp
        Core/Simulation.cpp

        # MATH
        Math/Geometrics.hpp
        Math/Geometrics.cpp

        # PRIMITIVES
        Primitives/Floor.hpp
        Primitives/Floor.cpp
        Primitives/Quad.hpp
        Primitives/Quad.cpp
        Primitives/FloorTexture.hpp
        Primitives/FloorTexture.cpp

        # RENDERER
        Renderer/Buffer.hpp
        Renderer/Framebuffer.hpp
        Renderer/Framebuffer.cpp
        Renderer/GLDebug.hpp
        Renderer/GLDebug.cpp
        Renderer/Shader.hpp
        Renderer/Shader.cpp
        Renderer/Texture.hpp
        Renderer/Texture.cpp
        Renderer/VertexArray.hpp
        Renderer/VertexArray.cpp
)

# Set src/ as an include directory so files in subdirectories can find each other.
target_include_directories(ray-casting PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

# Link in glad and glfw libraries.
target_link_libraries(ray-casting PRIVATE OpenGL32)
target_link_libraries(ray-casting PRIVATE glad)
target_link_libraries(ray-casting PRIVATE glfw)

# Use precompiled headers.
target_precompile_headers(ray-casting PRIVATE pch.hpp pch.cpp)

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

#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}" /SUBSYSTEM:WINDOWS /ENTRY:main)

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
