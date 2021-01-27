#include "pch.hpp"
#include "glad/glad.h"
#include "Renderer/VertexArray.hpp"
#include "Renderer/Buffer.hpp"

class Floor {
    lwvl::VertexArray vao;
    lwvl::ArrayBuffer vbo;
    lwvl::ElementBuffer ebo;
public:

    Floor(float left, float bottom, float width, float height);
    Floor(const Floor& other) = delete;
    Floor(Floor&& other) noexcept;

    Floor& operator=(const Floor& other) = delete;
    Floor& operator=(Floor&& other) noexcept;

    void draw();
};
