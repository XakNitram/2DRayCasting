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

# Mark the project as C++ 17.
set_target_properties(
        ray-casting PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

# Copy shaders to output directory.
add_custom_command(
        TARGET ray-casting POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/res/Shaders/
        $<TARGET_FILE_DIR:ray-casting>/Shaders/
)
message(STATUS "Copied shaders to output")
