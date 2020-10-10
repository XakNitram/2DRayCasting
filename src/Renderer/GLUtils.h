#pragma once
#include "rcpch.h"
#include <glad/glad.h>
#include "Core/Utils.h"

#ifndef NDEBUG
#define GLCall(x) GLClearErrors();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
#define GLCall(x) x
#endif


static void GLClearErrors() {
    while (glGetError() != GL_NO_ERROR);
}


static bool GLLogCall(const char* function, const char* file, int line) {
    bool exitOK = true;
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << ')' << function <<
            " " << file << ':' << line << std::endl;
        exitOK = false;
    }
    
    return exitOK;
}

static bool GLLogErrors() {
    bool exitOK = true;
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << std::hex << error << ')' << std::endl;
        exitOK = false;
    }

    return exitOK;
}