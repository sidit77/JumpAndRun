#include "creature_renderer.h"

#include <utility>
#include "util/filesystem.h"

using namespace jnr;
using namespace glm;

CreatureRenderer::CreatureRenderer(std::shared_ptr<CreatureModule::Creature> c_ptr, std::shared_ptr<glc::Texture> tex) :
        creature(std::move(c_ptr)),
        texture(std::move(tex)),
        vao(),
        pos_buffer(), tex_buffer(), ind_buffer(), col_buffer(),
        program(*services::filesystem->readResource<glc::Program>("assets/shader/character.json"))
{

    vao.bind();
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    pos_buffer.bind(GL_ARRAY_BUFFER);
    glBufferData(GL_ARRAY_BUFFER, creature->GetTotalNumPoints() * 3 * sizeof(glm::float32), creature->GetGlobalPts(), GL_STREAM_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::float32), (void*)0);

    col_buffer.bind(GL_ARRAY_BUFFER);
    glBufferData(GL_ARRAY_BUFFER, creature->GetTotalNumPoints() * 5 * sizeof(glm::uint8), creature->GetRenderColours(), GL_STREAM_DRAW);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 4 * sizeof(uint8_t), (void*)0);

    tex_buffer.bind(GL_ARRAY_BUFFER);
    glBufferData(GL_ARRAY_BUFFER, creature->GetTotalNumPoints() * 2 * sizeof(glm::float32), creature->GetGlobalUvs(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::float32), (void*)0);

    ind_buffer.bind(GL_ELEMENT_ARRAY_BUFFER);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, creature->GetTotalNumIndices() * sizeof(glm::uint32), creature->GetGlobalIndices(), GL_STATIC_DRAW);

}

void CreatureRenderer::draw(glm::vec2 pos, float scale, const Camera &cam) {
    glDepthFunc(GL_LEQUAL);
    vao.bind();
    program.bind();
    texture->bind(GL_TEXTURE0);
    program.setUniformMatrix("cam", false, cam.matrix);
    program.setUniform("pos", pos);
    program.setUniform("scale", scale);
    glNamedBufferData(pos_buffer.id, creature->GetTotalNumPoints() * 3 * sizeof(glm::float32), creature->GetRenderPts(), GL_STREAM_DRAW);
    glNamedBufferData(col_buffer.id, creature->GetTotalNumPoints() * 4 * sizeof(glm::uint8), creature->GetRenderColours(), GL_STREAM_DRAW);
    ind_buffer.bind(GL_ELEMENT_ARRAY_BUFFER);
    glDrawElements(GL_TRIANGLES, creature->GetTotalNumIndices(), GL_UNSIGNED_INT, 0);
    glDepthFunc(GL_LESS);
}
