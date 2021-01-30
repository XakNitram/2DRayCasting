#pragma once
#include "pch.hpp"
#include "Renderer/Framebuffer.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/Shader.hpp"
#include "Core/Utils.hpp"
#include "Quad.hpp"


class FloorTexture {
    lwvl::Texture texture;

public:
    void render(uint32_t width, uint32_t height);

    [[nodiscard]] uint32_t slot();
    void slot(uint32_t);

    void bind();
};
