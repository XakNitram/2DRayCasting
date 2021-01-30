#pragma once
#include "pch.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/Buffer.hpp"


class Quad {
    lwvl::VertexArray vao;
    lwvl::ArrayBuffer vbo;
    lwvl::ElementBuffer ebo;
public:

    Quad(float left, float bottom, float width, float height);
    Quad(const Quad& other) = delete;
    Quad(Quad&& other) noexcept;

    Quad& operator=(const Quad& other) = delete;
    Quad& operator=(Quad&& other) noexcept;

    void draw();
};
