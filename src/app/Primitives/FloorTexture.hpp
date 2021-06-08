#pragma once

#include "pch.hpp"
#include "Core/Utils.hpp"
#include "Quad.hpp"
#include "Framebuffer.hpp"
#include "Texture.hpp"
#include "Shader.hpp"


class FloorTexture : public lwvl::Texture2D {
protected:
    // Disallow the user constructing over the font atlas.
    using lwvl::Texture2D::construct;
public:
    void render(uint32_t width, uint32_t height);
};
