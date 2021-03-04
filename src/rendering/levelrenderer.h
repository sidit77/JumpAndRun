#pragma once

#include <glm.hpp>
#include <vector>
#include <glclasses/shader.h>
#include <glclasses/buffer.h>
#include <glclasses/texture.h>
#include <memory>
#include "camera.h"
#include "../util/noncopyable.h"

namespace jnr {
    class LevelRenderer : private NonCopyable{
    private:
#pragma pack(push, 1)
        struct Vertex {
            Vertex(glm::vec3 p, glm::vec2 t) : pos(p), tex(t){};
            glm::vec3 pos;
            glm::vec2 tex;
        };
#pragma pack(pop)
        glc::VertexArray vao;
        glc::Buffer vbo;
        glc::Texture texture;
        glc::Program program;
        std::vector<Vertex> sprites;
        int dx, dy;
    public:
        LevelRenderer(const std::string& path, int x, int y);

        void drawSprite(int id, glm::vec2 pos, glm::vec2 size, float z = -0.4f, glm::vec2 pivot = glm::vec2(0,0));

        void render(jnr::Camera& cam);
    };
}

