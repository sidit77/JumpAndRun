#pragma once;

#include <string>
#include <CreatureModule.h>
#include "opengl/shader.h"
#include "opengl/buffer.h"
#include "opengl/texture.h"
#include "camera.h"

namespace jnr {
    class CreatureRenderer : private NonCopyable{
    private:
        std::shared_ptr<CreatureModule::Creature> creature;
        Program program;
        VertexArray vao;
        VertexBuffer pos_buffer, col_buffer, tex_buffer, ind_buffer;
        Texture texture;
    public:
        CreatureRenderer(std::shared_ptr<CreatureModule::Creature> c_ptr, const std::string& texpath);
        ~CreatureRenderer() = default;
        void draw(glm::vec2 pos, float scale, const Camera& camera);
    };
}