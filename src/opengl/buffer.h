#pragma once

#include <glbinding/gl/gl.h>

namespace jnr::opengl {
    class VertexBuffer {
    public:
        gl::GLuint id;
        VertexBuffer();
        ~VertexBuffer();
        void bind(gl::GLenum target);
    };

    class VertexArray {
    public:
        gl::GLuint id;
        VertexArray();
        ~VertexArray();
        void bind();
    };

}


