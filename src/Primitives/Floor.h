#include "rcpch.h"
#include "glad/glad.h"
#include "Renderer/VertexArray.h"

class Floor {
    VertexArray vao;
public:

    Floor(float left, float bottom, float width, float height);
    Floor(const Floor& other) = delete;
    Floor(Floor&& other) noexcept;

    Floor& operator=(const Floor& other) = delete;
    Floor& operator=(Floor&& other) noexcept;

    void draw() const;
};
