#include <glad/glad.h>
#include "game.h"
#include <imgui.h>
#include <iostream>
#include <memory>
#include <gtc/type_ptr.hpp>
#include <MeshBone.h>


using namespace jnr;
using namespace glm;

AABB getPlatform(float x, float y, float w, float h){
    return AABB{vec2(x,y), vec2(x+w,y+h)};
}

Game::Game() :
        cam(),
        player(50, 290),
        platforms(),
        program{std::make_shared<opengl::Shader>("res/shader/platform_vertex.glsl", GL_VERTEX_SHADER),std::make_shared<opengl::Shader>("res/shader/platform_fragment.glsl", GL_FRAGMENT_SHADER)},
        staticvao(),
        staticvbo(),
        creature_vao(),
        creature_pos(), creature_tex(), creature_ind(), creature_col(),
        creature_program{std::make_shared<opengl::Shader>("res/shader/character_vertex.glsl", GL_VERTEX_SHADER),std::make_shared<opengl::Shader>("res/shader/character_fragment.glsl", GL_FRAGMENT_SHADER)}
{
    platforms.push_back(getPlatform(0   , 0  , 1500, 10 ));
    platforms.push_back(getPlatform(1490, 10 , 10  , 700));
    platforms.push_back(getPlatform(0   , 10 , 10  , 700));
    platforms.push_back(getPlatform(60  , 500, 200 , 30 ));
    platforms.push_back(getPlatform(200 , 350, 200 , 30 ));
    platforms.push_back(getPlatform(300 , 200, 200 , 30 ));
    platforms.push_back(getPlatform(400 , 600, 200 , 30 ));
    platforms.push_back(getPlatform(600 , 100, 200 , 30 ));
    platforms.push_back(getPlatform(650 , 270, 200 , 30 ));
    platforms.push_back(getPlatform(900 , 400, 200 , 30 ));
    platforms.push_back(getPlatform(920 , 100, 200 , 30 ));
    platforms.push_back(getPlatform(1200   , 200  , 150, 30 ));
    platforms.push_back(getPlatform(1340   , 300  , 10, 400 ));

    std::vector<vec3> vertices = {
            vec3(-0.5f, -0.5f, 0.7f),
            vec3( 0.5f, -0.5f, 0.7f),
            vec3( 0.0f,  0.5f, 0.7f),
    };

    for(auto plat : platforms){
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

    CreatureModule::CreatureLoadDataPacket json_data;
    CreatureModule::LoadCreatureJSONData("assets/character/death_data.json", json_data);
    creature_texture = std::make_unique<opengl::Texture>("assets/character/death_atlas.png");

    auto cur_creature = std::make_shared<CreatureModule::Creature>(json_data);


    creature_manager = std::make_unique<CreatureModule::CreatureManager>(cur_creature);
    creature_manager->CreateAnimation(json_data, "Idle");
    //creature_manager->CreateAnimation(json_data, "second");
    creature_manager->SetActiveAnimationName("Idle");
    creature_manager->SetIsPlaying(true);

    creature_vao.bind();
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    creature_pos.bind(GL_ARRAY_BUFFER);
    glBufferData(GL_ARRAY_BUFFER, creature_manager->GetCreature()->GetTotalNumPoints() * 3 * sizeof(glm::float32), creature_manager->GetCreature()->GetGlobalPts(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::float32), (void*)0);

    creature_col.bind(GL_ARRAY_BUFFER);
    glBufferData(GL_ARRAY_BUFFER, creature_manager->GetCreature()->GetTotalNumPoints() * 5 * sizeof(uint8_t), creature_manager->GetCreature()->GetRenderColours(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 4 * sizeof(uint8_t), (void*)0);

    creature_tex.bind(GL_ARRAY_BUFFER);
    glBufferData(GL_ARRAY_BUFFER, creature_manager->GetCreature()->GetTotalNumPoints() * 2 * sizeof(glm::float32), creature_manager->GetCreature()->GetGlobalUvs(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::float32), (void*)0);

    creature_ind.bind(GL_ELEMENT_ARRAY_BUFFER);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, creature_manager->GetCreature()->GetTotalNumIndices() * sizeof(uint32), creature_manager->GetCreature()->GetGlobalIndices(), GL_STATIC_DRAW);
}

void Game::update(float timestep, GLFWwindow* window) {

    vec2 mv(0,0);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        mv.x -= 1.0f;
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        mv.x += 1.0f;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        mv.y += 1.0f;
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        mv.y -= 1.0f;
    player.move(mv.x);

    if(mv.x != 0)
        creature_manager->SetMirrorY(mv.x < 0);

    player.jump(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);

    static bool bp;
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        if (!bp) {
            player.vel += 1000.0f * mv;
            bp = true;
        }
    } else {
        bp = false;
    }
    player.update(timestep, platforms);
}


void Game::render(float delta, float catchup, glm::ivec2 screensize) {
    cam.aspect = (float)screensize.x / screensize.y;
    cam.position = glm::mix(cam.position, player.pos + player.vel * catchup, glm::clamp(1-pow(0.1f, delta),0.0f, 1.0f));
    cam.update();

    program.bind();
    staticvao.bind();
    glUniform3f(program.getUniformLocation("color"), 1.0f, 0.5f, 0.7f);
    glUniformMatrix4fv(program.getUniformLocation("cam"), 1, false, glm::value_ptr(mat4(1.0f)));
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glUniform3f(program.getUniformLocation("color"), 0.0f, 0.43f, 0.67f);
    glUniformMatrix4fv(program.getUniformLocation("cam"), 1, false, glm::value_ptr(cam.matrix));
    glDrawArrays(GL_TRIANGLES, 3, 6 * platforms.size());
    glBindVertexArray(0);
    glUseProgram(0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(cam.position.x - (cam.scale * cam.aspect), cam.position.x + (cam.scale * cam.aspect), cam.position.y - cam.scale, cam.position.y + cam.scale, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);

    if(showphitbox) {
        glColor3f(1.0f, 0.42f, 0.42f);
        glBegin(GL_QUADS);
        glVertex3f(player.pos.x + player.vel.x * catchup + player.hitbox.low.x,
                   player.pos.y + player.vel.y * catchup + player.hitbox.low.y, 0.3f);
        glVertex3f(player.pos.x + player.vel.x * catchup + player.hitbox.high.x,
                   player.pos.y + player.vel.y * catchup + player.hitbox.low.y, 0.3f);
        glVertex3f(player.pos.x + player.vel.x * catchup + player.hitbox.high.x,
                   player.pos.y + player.vel.y * catchup + player.hitbox.high.y, 0.3f);
        glVertex3f(player.pos.x + player.vel.x * catchup + player.hitbox.low.x,
                   player.pos.y + player.vel.y * catchup + player.hitbox.high.y, 0.3f);
        glEnd();
    }

    creature_vao.bind();
    creature_manager->Update(delta);
    creature_manager->GetCreature()->FillRenderColours(255,255,255,255);
    creature_program.bind();
    creature_texture->bind(GL_TEXTURE_2D, GL_TEXTURE0);
    glUniformMatrix4fv(creature_program.getUniformLocation("cam"), 1, false, glm::value_ptr(cam.matrix));
    //glUniform2f(creature_program.getUniformLocation("pos"), player.pos.x + player.vel.x * catchup, player.pos.y + player.vel.y * catchup+ 25);
    //glUniform1f(creature_program.getUniformLocation("scale"), 4);
    glUniform2f(creature_program.getUniformLocation("pos"), player.pos.x + player.vel.x * catchup, player.pos.y + player.vel.y * catchup - 12);
    glUniform1f(creature_program.getUniformLocation("scale"), 14);
    glNamedBufferData(creature_pos.id, creature_manager->GetCreature()->GetTotalNumPoints() * 3 * sizeof(glm::float32), creature_manager->GetCreature()->GetRenderPts(), GL_STREAM_DRAW);
    glNamedBufferData(creature_col.id, creature_manager->GetCreature()->GetTotalNumPoints() * 4 * sizeof(uint8_t), creature_manager->GetCreature()->GetRenderColours(), GL_STREAM_DRAW);
    creature_ind.bind(GL_ELEMENT_ARRAY_BUFFER);
    glDrawElements(GL_TRIANGLES, creature_manager->GetCreature()->GetTotalNumIndices(), GL_UNSIGNED_INT, 0);

}

Game::~Game() {
}

void Game::ongui() {
    if(ImGui::Button("Reset Player")){
        player.pos = vec2(40, 290);
        player.vel = vec2(0,0);
        player.force = vec2(0,0);
        cam.position = vec2(0,0);
    }
    ImGui::Checkbox("Show Hitbox", &showphitbox);
    ImGui::Text("vel: [%.2f,%.2f]", player.vel.x, player.vel.y);
}

