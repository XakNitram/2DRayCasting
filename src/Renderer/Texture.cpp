#include "pch.hpp"
#include "Texture.hpp"

void lwvl::Texture::bind() {
    if (m_id != 0) {
        glActiveTexture(GL_TEXTURE0 + m_slot);
        glBindTexture(GL_TEXTURE_2D, m_id);
    }
}

uint32_t lwvl::Texture::slot() const {
    return m_slot;
}

void lwvl::Texture::slot(uint32_t value) {
    int32_t maxTextureUnits;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &maxTextureUnits);

    if (maxTextureUnits < value) {
        m_slot = value;
    } else {
        throw std::exception("Max Texture Units Exceeded.");
    }
}

void lwvl::Texture::construct(uint32_t width, uint32_t height, const void *data) {
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, data
    );
}

void lwvl::Texture::filter(lwvl::Filter value) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(value));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(value));
}
