#include "buffer.h"

using namespace gl;

jnr::opengl::VertexBuffer::VertexBuffer(){
    glCreateBuffers(1, &id);
}

jnr::opengl::VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &id);
}

void jnr::opengl::VertexBuffer::bind(GLenum target) {
    glBindBuffer(target, id);
}

jnr::opengl::VertexArray::VertexArray() {
    glCreateVertexArrays(1, &id);
}

jnr::opengl::VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &id);
}

void jnr::opengl::VertexArray::bind() {
    glBindVertexArray(id);
}
