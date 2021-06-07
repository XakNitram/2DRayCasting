#pragma once
#include "pch.hpp"
#include "Framebuffer.hpp"

namespace lwvl {
    class Framebuffer;

    enum class Filter {
        Linear = GL_LINEAR,
        Nearest = GL_NEAREST
    };

    // In this project I don't see myself using anything other than
    //  a 2D texture, even if we use textures from file.
    class Texture {
        class ID {
            static unsigned int reserve() {
                unsigned int tempID;
                glGenTextures(1, &tempID);
                return tempID;
            }

        public:
            ~ID() {
                glDeleteTextures(1, &textureID);
            }

            explicit operator uint32_t () const {
                return textureID;
            }

            const uint32_t textureID = reserve();
        };

        // Offsite Data - to avoid copying buffers on the GPU for simple copies of this class.
        std::shared_ptr<Texture::ID> m_offsite_id = std::make_shared<Texture::ID>();

        // Local Data
        uint32_t m_id = static_cast<uint32_t>(*m_offsite_id);
        uint32_t m_slot = 0;

        friend Framebuffer;
    public:
        [[nodiscard]] uint32_t slot() const;
        void slot(uint32_t);

        void construct(uint32_t width, uint32_t height, const void* data);
        void filter(Filter);

        void bind();
    };
}
