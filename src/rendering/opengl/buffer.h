#pragma once

#include <glad/glad.h>
#include "../../util/noncopyable.h"

namespace jnr {
    class VertexBuffer : private NonCopyable{
    public:
        GLuint id{};
        VertexBuffer();
        ~VertexBuffer();
        void bind(GLenum target);
    };

    class VertexArray : private NonCopyable{
    public:
        GLuint id{};
        VertexArray();
        ~VertexArray();
        void bind();
    };

}


