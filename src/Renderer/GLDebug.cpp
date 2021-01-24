#include "../pch.h"
#include "GLDebug.h"

void lwvl::debug::simpleDebugCallback(GLDEBUGPROC callback, void* userPtr) {
    glDebugMessageCallback(callback, userPtr);
}

void lwvl::debug::clearErrors() {
    while (glGetError() != GL_NO_ERROR);
}

void lwvl::debug::GLEventListener::assign(LWVLDebugProc callback) {
    glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* state) {
        ((GLEventListener*)state)->invoke(source, type, id, severity, length, message);
    }, this);
}

lwvl::debug::GLEventListener::GLEventListener(void* userPtr, LWVLDebugProc callback, bool enabled): m_callback(callback), m_userPtr(userPtr) {
    assign(std::forward<LWVLDebugProc>(callback));
    if (enabled) {
        control(true);
    }
}

lwvl::debug::GLEventListener::GLEventListener(LWVLDebugProc callback, bool enabled): m_callback(callback), m_userPtr(nullptr) {
    assign(std::forward<LWVLDebugProc>(callback));
    if (enabled) {
        control(true);
    }
}

lwvl::debug::GLEventListener::~GLEventListener() {
    glDebugMessageCallback(nullptr, nullptr);
}

void lwvl::debug::GLEventListener::control(Source sourceFilter, Type typeFilter, Severity severityFilter, GLsizei count, bool enabled) {
    if (enabled) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    }

    else {
        glDisable(GL_DEBUG_OUTPUT);
        glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    }

    glDebugMessageControl(
        static_cast<GLenum>(sourceFilter), 
        static_cast<GLenum>(typeFilter), 
        static_cast<GLenum>(severityFilter), 
        count, &m_unusedIDs, enabled
    );
}

void lwvl::debug::GLEventListener::control(bool enabled) {
    control(
        lwvl::debug::Source::DONT_CARE,
        lwvl::debug::Type::DONT_CARE,
        lwvl::debug::Severity::DONT_CARE,
        0, std::forward<bool>(enabled)
    );
}

void lwvl::debug::GLEventListener::invoke(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message) {
    m_callback(static_cast<Source>(source), static_cast<Type>(type), id, static_cast<Severity>(severity), length, message, m_userPtr);
}
