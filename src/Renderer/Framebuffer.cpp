#include "rcpch.h"
#include "Framebuffer.h"

GLuint lwvl::Framebuffer::reserveFramebuffer() {
    GLuint id;
    glGenFramebuffers(1, &id);
    return id;
}

lwvl::Framebuffer::Framebuffer(): m_id(reserveFramebuffer()) {}

lwvl::Framebuffer::Framebuffer(Framebuffer&& other) noexcept : m_id(other.m_id) {
    other.m_id = 0;
}

lwvl::Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &m_id);
}

lwvl::Framebuffer& lwvl::Framebuffer::operator=(Framebuffer&& other) noexcept {
    m_id = other.m_id;
    other.m_id = 0;
    return *this;
}

void lwvl::Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

void lwvl::Framebuffer::clear() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
