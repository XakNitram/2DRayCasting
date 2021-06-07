# Add the project files to the executable.
target_sources(
    ray-casting
    PRIVATE
    Application.cpp

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
    Primitives/FloorTexture.hpp
    Primitives/FloorTexture.cpp
    Primitives/NodeRenderer.hpp
    Primitives/Quad.hpp
    Primitives/Quad.cpp
)

# Set src/ as an include directory so files in subdirectories can find each other.
target_include_directories(ray-casting PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

# Link in glad and glfw libraries.
target_link_libraries(ray-casting PRIVATE OpenGL32)
target_link_libraries(ray-casting PRIVATE glad)
target_link_libraries(ray-casting PRIVATE glfw)

target_link_libraries(ray-casting PRIVATE lwvl)

# Use precompiled headers.
target_precompile_headers(ray-casting PRIVATE pch.hpp pch.cpp)

#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}" /SUBSYSTEM:WINDOWS /ENTRY:main)
