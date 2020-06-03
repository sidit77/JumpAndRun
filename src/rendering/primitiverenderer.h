#pragma once

#include <cpp-colors/color.h>
#include "opengl/buffer.h"
#include "opengl/shader.h"
#include "camera.h"

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
        jnr::VertexArray vao;
        jnr::VertexBuffer vbo;
        jnr::Program program;
        std::vector<Vertex> lines;
        std::vector<Vertex> triangles;

    public:
        PrimitiveRenderer();

        void drawLine(glm::vec2 p1, glm::vec2 p2, colors::color c, float z = -0.4f);

        void drawAABB(glm::vec2 p1, glm::vec2 p2, colors::color c, float z = -0.4f);

        void drawQuad(glm::vec2 pos, glm::vec2 size, colors::color c, float z = -0.4f, glm::vec2 pivot = glm::vec2(0,0));

        void render(jnr::Camera& cam);
    };
}

