#pragma once

#include <cpp-colors/color.h>
#include <vector>
#include <glclasses/buffer.h>
#include <glclasses/shader.h>
#include <memory>
#include "camera.h"
#include "../util/noncopyable.h"

namespace jnr {
    class PrimitiveRenderer : private NonCopyable{
    private:
#pragma pack(push, 1)
        struct Vertex {
            Vertex(glm::vec3 p, colors::color c) : pos(p), color(c.value<colors::pixel_format::rgba32>()){};
            glm::vec3 pos;
            glm::uint32 color;
        };
#pragma pack(pop)
        glc::VertexArray vao;
        glc::VertexBuffer vbo;
        std::unique_ptr<glc::Program> program;
        std::vector<Vertex> lines;
        std::vector<Vertex> triangles;

    public:
        PrimitiveRenderer();

        void drawLine(glm::vec2 p1, glm::vec2 p2, colors::color c, float z = -0.4f);

        void drawAABB(glm::vec2 p1, glm::vec2 p2, colors::color c, float z = -0.4f);
        void drawAABBOutline(glm::vec2 p1, glm::vec2 p2, colors::color c, float z = -0.4f);
        void drawAABBOutlineP(glm::vec2 p1, glm::vec2 p2, colors::color c, float z = -0.4f, float width = 4);

        void drawQuad(glm::vec2 pos, glm::vec2 size, colors::color c, float z = -0.4f, glm::vec2 pivot = glm::vec2(0,0));

        void render(jnr::Camera& cam);
    };
}

