#pragma once

#include <glad/glad.h>

namespace jnr::opengl {
    class VertexBuffer {
    public:
        GLuint id;
        VertexBuffer();
        ~VertexBuffer();
        void bind(GLenum target);
    };

    class VertexArray {
    public:
        GLuint id;
        VertexArray();
        ~VertexArray();
        void bind();
    };

}


