#pragma once

#include <string>
#include <CreatureModule.h>
#include <glclasses/shader.h>
#include <glclasses/buffer.h>
#include <glclasses/texture.h>
#include "camera.h"
#include "util/noncopyable.h"

namespace jnr {
    class CreatureRenderer : private NonCopyable{
    private:
        std::shared_ptr<CreatureModule::Creature> creature;
        glc::Program program;
        glc::VertexArray vao;
        glc::Buffer pos_buffer, col_buffer, tex_buffer, ind_buffer;
        std::shared_ptr<glc::Texture> texture;
    public:
        CreatureRenderer(std::shared_ptr<CreatureModule::Creature> c_ptr, std::shared_ptr<glc::Texture> tex);
        ~CreatureRenderer() = default;
        void draw(glm::vec2 pos, float scale, const Camera& camera);
    };
}