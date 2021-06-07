add_library(
    lwvl

    Buffer.hpp
    Framebuffer.hpp
    Framebuffer.cpp
    GLDebug.hpp
    GLDebug.cpp
    Shader.hpp
    Shader.cpp
    Texture.hpp
    Texture.cpp
    VertexArray.hpp
    VertexArray.cpp
)

set_target_properties(
    lwvl PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_include_directories(lwvl PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# Use precompiled headers.
target_precompile_headers(lwvl PRIVATE pch.hpp pch.cpp)

target_link_libraries(lwvl PRIVATE OpenGL32)
target_link_libraries(lwvl PRIVATE glad)
