#include "levelrenderer.h"

#include <gtc/type_ptr.hpp>
#include <iostream>
#include "util/filesystem.h"

jnr::LevelRenderer::LevelRenderer(const std::string &path, int x, int y) :
program(*services::filesystem->readResource<glc::Program>("assets/shader/sprite.json"))
{
    vao.bind();
    vbo.bind(GL_ARRAY_BUFFER);
    vbo.setLabel("LevelRenderer");
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE , sizeof(Vertex), (void*)sizeof(glm::vec3));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

void jnr::LevelRenderer::drawSprite(const std::string& name, glm::vec2 pos, glm::vec2 size, float z, glm::vec2 pivot) {
    assert(currentSpriteSheet.has_value());

    glm::vec2 low = pos - size * pivot;
    glm::vec2 high = pos + size;

    auto sprite = (*currentSpriteSheet).get().sprites.at(name);

    sprites.emplace_back(glm::vec3(low .x, low .y, z), glm::vec2(sprite.low .x, sprite.low .y));
    sprites.emplace_back(glm::vec3(high.x, low .y, z), glm::vec2(sprite.high.x, sprite.low .y));
    sprites.emplace_back(glm::vec3(high.x, high.y, z), glm::vec2(sprite.high.x, sprite.high.y));
    sprites.emplace_back(glm::vec3(high.x, high.y, z), glm::vec2(sprite.high.x, sprite.high.y));
    sprites.emplace_back(glm::vec3(low .x, high.y, z), glm::vec2(sprite.low .x, sprite.high.y));
    sprites.emplace_back(glm::vec3(low .x, low .y, z), glm::vec2(sprite.low .x, sprite.low .y));
}

void jnr::LevelRenderer::beginRender(const jnr::SpriteSheet &spriteSheet) {
    assert(sprites.empty());
    currentSpriteSheet.emplace(spriteSheet);
}

void jnr::LevelRenderer::finishRender(jnr::Camera &cam) {
    assert(currentSpriteSheet.has_value());
    program.bind();
    currentSpriteSheet->get().texture.bind(0);
    vao.bind();
    program.setUniformMatrix("cam", false, cam.matrix);
    if(!sprites.empty()) {
        glNamedBufferData(vbo.id, sprites.size() * sizeof(Vertex), sprites.data(), GL_STREAM_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, sprites.size());
        sprites.clear();
    }
    currentSpriteSheet.reset();
}