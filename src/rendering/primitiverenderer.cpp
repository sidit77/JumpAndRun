#include "primitiverenderer.h"

#include <gtc/type_ptr.hpp>
#include <iostream>
#include "util/filesystem.h"

jnr::PrimitiveRenderer::PrimitiveRenderer() :
    program(*services::filesystem->readResource<glc::Program>("assets/shader/primitive.json")){

    vao.bind();
    vbo.bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(0, 3, GL_FLOAT        , GL_FALSE, sizeof(Vertex), (void*)0);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE , sizeof(Vertex), (void*)sizeof(glm::vec3));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

}

void jnr::PrimitiveRenderer::render(jnr::Camera &cam) {
    program.bind();
    vao.bind();
    glUniformMatrix4fv(program.getUniformLocation("cam"), 1, GL_FALSE, glm::value_ptr(cam.matrix));
    if(!triangles.empty()) {
        glNamedBufferData(vbo.id, triangles.size() * sizeof(Vertex), triangles.data(), GL_STREAM_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, triangles.size());
        triangles.clear();
    }
    if(!lines.empty()) {
        glNamedBufferData(vbo.id, lines.size() * sizeof(Vertex), lines.data(), GL_STREAM_DRAW);
        glDrawArrays(GL_LINES, 0, lines.size());
        lines.clear();
    }
}

void jnr::PrimitiveRenderer::drawLine(glm::vec2 p1, glm::vec2 p2, colors::color c, float z) {
    lines.emplace_back(glm::vec3(p1, z), c);
    lines.emplace_back(glm::vec3(p2, z), c);
}

void jnr::PrimitiveRenderer::drawAABB(glm::vec2 low, glm::vec2 high, colors::color c, float z) {
    triangles.emplace_back(glm::vec3(low .x, low .y, z), c);
    triangles.emplace_back(glm::vec3(high.x, low .y, z), c);
    triangles.emplace_back(glm::vec3(high.x, high.y, z), c);
    triangles.emplace_back(glm::vec3(high.x, high.y, z), c);
    triangles.emplace_back(glm::vec3(low .x, high.y, z), c);
    triangles.emplace_back(glm::vec3(low .x, low .y, z), c);
}

void jnr::PrimitiveRenderer::drawQuad(glm::vec2 pos, glm::vec2 size, colors::color c, float z, glm::vec2 pivot) {
    glm::vec2 spos = pos - size * pivot;
    drawAABB(spos, spos + size, c, z);
}

void jnr::PrimitiveRenderer::drawAABBOutline(glm::vec2 p1, glm::vec2 p2, colors::color c, float z) {
    drawLine(glm::mix(p1, p2, glm::vec2(0,0)),glm::mix(p1, p2, glm::vec2(1,0)), c, z);
    drawLine(glm::mix(p1, p2, glm::vec2(1,0)),glm::mix(p1, p2, glm::vec2(1,1)), c, z);
    drawLine(glm::mix(p1, p2, glm::vec2(1,1)),glm::mix(p1, p2, glm::vec2(0,1)), c, z);
    drawLine(glm::mix(p1, p2, glm::vec2(0,1)),glm::mix(p1, p2, glm::vec2(0,0)), c, z);
}

void jnr::PrimitiveRenderer::drawAABBOutlineP(glm::vec2 p1, glm::vec2 p2, colors::color c, float z, float w) {
    drawQuad(glm::mix(p1, p2, glm::vec2(0.5f,0.0f)), glm::vec2(p2.x - p1.x + w, w), c, z, glm::vec2(0.5f));
    drawQuad(glm::mix(p1, p2, glm::vec2(0.5f,1.0f)), glm::vec2(p2.x - p1.x + w, w), c, z, glm::vec2(0.5f));
    drawQuad(glm::mix(p1, p2, glm::vec2(1.0f,0.5f)), glm::vec2(w, p2.y - p1.y + w), c, z, glm::vec2(0.5f));
    drawQuad(glm::mix(p1, p2, glm::vec2(0.0f,0.5f)), glm::vec2(w, p2.y - p1.y + w), c, z, glm::vec2(0.5f));
}
