#include "level.h"
#include <gtc/type_ptr.hpp>
#include <iostream>

using namespace jnr;
using namespace glm;

AABB getPlatform(float x, float y, float w, float h){
    return AABB{vec2(x,y), vec2(x+w,y+h)};
}

jnr::Level::Level() : hitboxes(), staticvbo(), staticvao(),
                      program{std::make_shared<Shader>("res/shader/platform_vertex.glsl", GL_VERTEX_SHADER),std::make_shared<Shader>("res/shader/platform_fragment.glsl", GL_FRAGMENT_SHADER)}
{
    hitboxes.push_back(getPlatform(0   , 0  , 1500, 10 ));
    hitboxes.push_back(getPlatform(1490, 10 , 10  , 700));
    hitboxes.push_back(getPlatform(0   , 10 , 10  , 700));
    hitboxes.push_back(getPlatform(60  , 500, 200 , 30 ));
    hitboxes.push_back(getPlatform(200 , 350, 200 , 30 ));
    hitboxes.push_back(getPlatform(300 , 200, 200 , 30 ));
    hitboxes.push_back(getPlatform(400 , 600, 200 , 30 ));
    hitboxes.push_back(getPlatform(600 , 100, 200 , 30 ));
    hitboxes.push_back(getPlatform(650 , 270, 200 , 30 ));
    hitboxes.push_back(getPlatform(900 , 400, 200 , 30 ));
    hitboxes.push_back(getPlatform(920 , 100, 200 , 30 ));
    hitboxes.push_back(getPlatform(1200, 200, 150 , 30 ));
    hitboxes.push_back(getPlatform(1340, 300, 10  , 400));

    std::vector<vec3> vertices = {
            vec3(-0.5f, -0.5f, 0.7f),
            vec3( 0.5f, -0.5f, 0.7f),
            vec3( 0.0f,  0.5f, 0.7f),
    };

    for(auto plat : hitboxes){
        vertices.emplace_back(plat.low .x, plat.low .y, 0.0f);
        vertices.emplace_back(plat.high.x, plat.low .y, 0.0f);
        vertices.emplace_back(plat.high.x, plat.high.y, 0.0f);
        vertices.emplace_back(plat.high.x, plat.high.y, 0.0f);
        vertices.emplace_back(plat.low .x, plat.high.y, 0.0f);
        vertices.emplace_back(plat.low .x, plat.low .y, 0.0f);
    }

    staticvao.bind();
    staticvbo.bind(GL_ARRAY_BUFFER);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

const std::vector<AABB> &jnr::Level::getHitboxes() {
    return hitboxes;
}

void Level::draw(float delta, float catchup, Camera &cam) {
    program.bind();
    staticvao.bind();
    glUniform3f(program.getUniformLocation("color"), 1.0f, 0.5f, 0.7f);
    glUniformMatrix4fv(program.getUniformLocation("cam"), 1, GL_FALSE, glm::value_ptr(mat4(1.0f)));
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glUniform3f(program.getUniformLocation("color"), 0.0f, 0.43f, 0.67f);
    glUniformMatrix4fv(program.getUniformLocation("cam"), 1, GL_FALSE, glm::value_ptr(cam.matrix));
    glDrawArrays(GL_TRIANGLES, 3, 6 * hitboxes.size());
    glBindVertexArray(0);
    glUseProgram(0);
}
