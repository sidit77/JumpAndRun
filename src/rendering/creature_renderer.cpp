#include "creature_renderer.h"

#include <utility>

using namespace jnr;
using namespace glm;

CreatureRenderer::CreatureRenderer(std::shared_ptr<CreatureModule::Creature> c_ptr, const std::string& texpath) :
        creature(std::move(c_ptr)),
        vao(),
        pos_buffer(), tex_buffer(), ind_buffer(), col_buffer(),
        texture(texpath),
        program{std::make_shared<Shader>("res/shader/character_vertex.glsl", GL_VERTEX_SHADER),std::make_shared<Shader>("res/shader/character_fragment.glsl", GL_FRAGMENT_SHADER)}
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
    vao.bind();
    program.bind();
    texture.bind(GL_TEXTURE_2D, GL_TEXTURE0);
    glUniformMatrix4fv(program.getUniformLocation("cam"), 1, false, glm::value_ptr(cam.matrix));
    glUniform2f(program.getUniformLocation("pos"), pos.x, pos.y);
    glUniform1f(program.getUniformLocation("scale"), scale);
    glNamedBufferData(pos_buffer.id, creature->GetTotalNumPoints() * 3 * sizeof(glm::float32), creature->GetRenderPts(), GL_STREAM_DRAW);
    glNamedBufferData(col_buffer.id, creature->GetTotalNumPoints() * 4 * sizeof(glm::uint8), creature->GetRenderColours(), GL_STREAM_DRAW);
    ind_buffer.bind(GL_ELEMENT_ARRAY_BUFFER);
    glDrawElements(GL_TRIANGLES, creature->GetTotalNumIndices(), GL_UNSIGNED_INT, 0);
}
