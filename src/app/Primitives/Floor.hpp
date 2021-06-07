#pragma once
#include "pch.hpp"
#include "glad/glad.h"
#include "Math/Geometrics.hpp"
#include "VertexArray.hpp"
#include "Buffer.hpp"

class Floor {
    lwvl::VertexArray vao;
    lwvl::ArrayBuffer vbo;
    lwvl::ElementBuffer ebo;
    Point start, end;

public:

    Floor();
    Floor(float left, float bottom, float width, float height);
    Floor(const Floor& other) = delete;
    Floor(Floor&& other) noexcept;

    Floor& operator=(const Floor& other) = delete;
    Floor& operator=(Floor&& other) noexcept;

    void update(float left, float bottom, float width, float height);

    void draw();
};
