#pragma once
#include <glad/glad.h>

namespace lwvl {
    class Framebuffer {
        GLuint m_id;

        static GLuint reserveFramebuffer();

    public:
        Framebuffer();
        Framebuffer(const Framebuffer&) = delete;
        Framebuffer(Framebuffer&& other) noexcept;
        ~Framebuffer();

        Framebuffer& operator=(const Framebuffer&) = delete;
        Framebuffer& operator=(Framebuffer&& other) noexcept;

        void bind();
        static void clear();
    };
}
