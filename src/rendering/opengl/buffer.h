#pragma once

#include <glad/glad.h>

namespace jnr {
    class VertexBuffer {
    public:
        GLuint id;
        VertexBuffer();
        VertexBuffer(const VertexBuffer &) = delete;
        VertexBuffer &operator=(const VertexBuffer &) = delete;
        ~VertexBuffer();
        void bind(GLenum target);
    };

    class VertexArray {
    public:
        GLuint id;
        VertexArray();
        ~VertexArray();
        VertexArray(const VertexArray &) = delete;
        VertexArray &operator=(const VertexArray &) = delete;
        void bind();
    };

}


