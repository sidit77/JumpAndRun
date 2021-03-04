#include "levelrenderer.h"

#include <gtc/type_ptr.hpp>
#include "util/filesystem.h"

jnr::LevelRenderer::LevelRenderer(const std::string &path, int x, int y) :
program(*services::filesystem->readResource<glc::Program>("assets/shader/sprite.json")),
texture(*services::filesystem->readResource<glc::Texture>(path)),
dx(x),
dy(y)
{
    vao.bind();
    vbo.bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE , sizeof(Vertex), (void*)sizeof(glm::vec3));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

void jnr::LevelRenderer::drawSprite(int id, glm::vec2 pos, glm::vec2 size, float z, glm::vec2 pivot) {
    glm::vec2 low = pos - size * pivot;
    glm::vec2 high = pos + size;

    int tx = id % dx;
    int ty = id / dx;

    sprites.emplace_back(glm::vec3(low .x, low .y, z), glm::vec2(tx + 0, ty + 1) * (1.0f / dx));
    sprites.emplace_back(glm::vec3(high.x, low .y, z), glm::vec2(tx + 1, ty + 1) * (1.0f / dx));
    sprites.emplace_back(glm::vec3(high.x, high.y, z), glm::vec2(tx + 1, ty + 0) * (1.0f / dx));

    sprites.emplace_back(glm::vec3(high.x, high.y, z), glm::vec2(tx + 1, ty + 0) * (1.0f / dx));
    sprites.emplace_back(glm::vec3(low .x, high.y, z), glm::vec2(tx + 0, ty + 0) * (1.0f / dx));
    sprites.emplace_back(glm::vec3(low .x, low .y, z), glm::vec2(tx + 0, ty + 1) * (1.0f / dx));
}

void jnr::LevelRenderer::render(jnr::Camera &cam) {
    program.bind();
    texture.bind(GL_TEXTURE0);
    vao.bind();
    glUniformMatrix4fv(program.getUniformLocation("cam"), 1, GL_FALSE, glm::value_ptr(cam.matrix));
    if(!sprites.empty()) {
        glNamedBufferData(vbo.id, sprites.size() * sizeof(Vertex), sprites.data(), GL_STREAM_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, sprites.size());
        sprites.clear();
    }
}
