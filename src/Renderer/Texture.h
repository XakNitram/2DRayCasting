#pragma once
#include <rcpch.h>
#include <glad/glad.h>
#include "GLUtils.h"

namespace lwvl {
    class Texture {
    public:
        enum class Target : GLenum {
            Texture1D = GL_TEXTURE_1D,
            Texture2D = GL_TEXTURE_2D,
            Texture3D = GL_TEXTURE_3D,
            Texture2DMultisample = GL_TEXTURE_2D_MULTISAMPLE,

            TextureRectangle = GL_TEXTURE_RECTANGLE,
            TextureBuffer = GL_TEXTURE_BUFFER,
            TextureCubeMap = GL_TEXTURE_CUBE_MAP,

            Texture1DArray = GL_TEXTURE_1D_ARRAY,
            Texture2DArray = GL_TEXTURE_2D_ARRAY,
            Texture2DMultisampleArray = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
            TextureCubeMapArray = GL_TEXTURE_CUBE_MAP_ARRAY
        };

    private:
        Target m_target;
        GLuint m_id;

        static GLuint reserveTexture();

    public:
        Texture(Target target);
        Texture(const Texture& other) = delete;
        Texture(Texture&& other) noexcept;

        Texture& operator=(const Texture& other) = delete;
        Texture& operator=(Texture&& other) noexcept;

        // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ftwi, fthi, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        void attachImage();

        static void ActiveTexture(unsigned int slot);
        static unsigned int ActiveTexture();

        // Only useful when no fragment shader is bound.
        void enable();
        void disable();

        void bind();
        void clear();

        // To match the rest of the API's static void clear()
        template<Target target>
        static void clear() {
            glBindTexture(static_cast<GLenum>(target), 0);
        }
    };
}
