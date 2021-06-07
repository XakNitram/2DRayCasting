#pragma once
#include "pch.hpp"


namespace lwvl {
    class VertexArray {
        // There are only < 256 attribute bind sites, so this could be a uint16_t
        //   if another 16 or 2x8 bytes can be used for something else.
        uint32_t m_attributes = 0;
        uint32_t m_instances = 1;
        uint32_t m_id = 0;
    public:

        VertexArray();
        ~VertexArray();

        void bind();
        static void clear();

        [[nodiscard]] uint32_t instances() const;
        void instances(uint32_t count);

        void attribute(uint8_t dimensions, GLenum type, int64_t stride, int64_t offset, uint32_t divisor = 0);

        void drawArrays(GLenum mode, int count) const;
        void drawElements(GLenum mode, int count, GLenum type) const;
    };
}
