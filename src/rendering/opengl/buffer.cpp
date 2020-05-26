#include "buffer.h"

using namespace jnr;

VertexBuffer::VertexBuffer(){
    glCreateBuffers(1, &id);
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &id);
}

void VertexBuffer::bind(GLenum target) {
    glBindBuffer(target, id);
}

VertexArray::VertexArray() {
    glCreateVertexArrays(1, &id);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &id);
}

void VertexArray::bind() {
    glBindVertexArray(id);
}
