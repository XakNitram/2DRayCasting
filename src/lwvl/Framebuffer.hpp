#pragma once

#include "pch.hpp"
#include "Texture.hpp"

namespace lwvl {
    enum class Attachment {
        Color = GL_COLOR_ATTACHMENT0,
        Depth = GL_DEPTH_ATTACHMENT,
        Stencil = GL_STENCIL_ATTACHMENT
    };

    class Texture;

    class Framebuffer {
        uint32_t m_id = 0;

    public:
        Framebuffer();

        ~Framebuffer();

        void attach(Attachment, Texture);

        void bind();
    };
}
