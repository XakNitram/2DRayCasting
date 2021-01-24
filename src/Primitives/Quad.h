#include "pch.h"
#include "glad/glad.h"
#include "Renderer/VertexArray.h"


class Quad {
    VertexArray vao;
public:

    Quad(float left, float bottom, float width, float height);
    Quad(const Quad& other) = delete;
    Quad(Quad&& other) noexcept;

    Quad& operator=(const Quad& other) = delete;
    Quad& operator=(Quad&& other) noexcept;

    void draw() const;
};
