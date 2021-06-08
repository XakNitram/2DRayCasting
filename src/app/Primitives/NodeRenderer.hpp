#pragma once

#include "pch.hpp"
#include "Math/Geometrics.hpp"
#include "VertexArray.hpp"
#include "Buffer.hpp"


// We can remove this template variable in the future
//  if it makes the class easier to use.
template<size_t capacity>
class NodeRenderer {
    // Attributes
    lwvl::VertexArray vao;
    lwvl::ArrayBuffer vbo;

    std::vector<LineSegment> m_segments;

    // Methods
    std::array<float, capacity * 4> collectData() {
        std::array<float, capacity * 4> temp{};

        size_t currentElements = size();
        for (size_t i = 0; i < currentElements; i++) {
            LineSegment &segment = m_segments[i];
            temp[i * 4 + 0] = segment.a.x;
            temp[i * 4 + 1] = segment.a.y;
            temp[i * 4 + 2] = segment.b.x;
            temp[i * 4 + 3] = segment.b.y;
        }

        return temp;
    }

public:
    NodeRenderer() {
        m_segments.reserve(capacity);

        vao.bind();
        vbo.bind();
        vbo.usage(lwvl::Usage::Dynamic);

        std::array<float, capacity * 4> data = collectData();
        vbo.construct(data.begin(), data.end());
        vao.attribute(2, GL_FLOAT, 2 * sizeof(float), 0);

        lwvl::VertexArray::clear();
        lwvl::ArrayBuffer::clear();
    }

    const std::vector<LineSegment> &segments() { return m_segments; }

    size_t size() { return m_segments.size(); }

    size_t max() { return capacity; }

    void add(LineSegment &&segment) {
        if (size() != capacity) {
            m_segments.push_back(segment);
        } else {
            throw std::exception("NodeRenderer capacity exceeded.");
        }
    }

    // Maybe a remove method but I won't use it here.

    void update() {
        vbo.bind();
        std::array<float, capacity * 4> data = collectData();
        vbo.update(data.begin(), data.end());
    }

    void draw() {
        vao.bind();
        vao.drawArrays(GL_LINES, capacity * 2);
    }
};
