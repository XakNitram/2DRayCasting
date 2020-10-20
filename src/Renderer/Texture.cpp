#include "rcpch.h"
#include "Texture.h"

GLuint lwvl::Texture::reserveTexture() {
    GLuint id;
    glGenTextures(1, &id);
    // assert(id != 0);
#ifdef _DEBUG
    std::cout << "Reserved texture " << id << '.' << std::endl;
#endif
    return id;
}

lwvl::Texture::Texture(Target mode): m_target(mode), m_id(reserveTexture()) {}

lwvl::Texture::Texture(Texture&& other) noexcept : m_target(other.m_target), m_id(other.m_id) {
    other.m_id = 0;
}

lwvl::Texture& lwvl::Texture::operator=(Texture&& other) noexcept {
    m_target = other.m_target;
    m_id = other.m_id;
    other.m_id = 0;
    return *this;
}

void lwvl::Texture::ActiveTexture(unsigned int slot) {
#ifndef IGNORE_BOUNDS
    GLint maxTextureSlots;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureSlots);
    if (static_cast<int>(slot) + 1 > maxTextureSlots) {
        std::stringstream msg;
        msg << "Texture image unit index " << slot << " out of bounds [max " << maxTextureSlots - 1 << "].";
        throw std::invalid_argument(msg.str().c_str());
    }
#endif // IGNORE_BOUNDS
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
}

unsigned int lwvl::Texture::ActiveTexture() {
    GLint activeTexture;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);
    return activeTexture;
}

void lwvl::Texture::enable() {
    glEnable(static_cast<GLenum>(m_target));
}

void lwvl::Texture::disable() {
    glDisable(static_cast<GLenum>(m_target));
}

void lwvl::Texture::bind() {
    glBindTexture(static_cast<GLenum>(m_target), m_id);
}

void lwvl::Texture::clear() {
    glBindTexture(static_cast<GLenum>(m_target), 0);
}
