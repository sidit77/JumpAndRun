#include <glad/glad.h>
#include "game.h"
#include <imgui.h>
#include <iostream>
#include <memory>
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
        program{std::make_shared<Shader>("res/shader/platform_vertex.glsl", GL_VERTEX_SHADER),std::make_shared<Shader>("res/shader/platform_fragment.glsl", GL_FRAGMENT_SHADER)},
        staticvao(),
        staticvbo(),
        lastInput()
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
    CreatureModule::LoadCreatureJSONData("assets/character/character_data.json", json_data);
    auto cur_creature = std::make_shared<CreatureModule::Creature>(json_data);

    creature_renderer = std::make_unique<CreatureRenderer>(cur_creature, "assets/character/character_atlas.png");
    creature_manager = std::make_unique<CreatureModule::CreatureManager>(cur_creature);
    creature_manager->CreateAnimation(json_data, "Running");
    creature_manager->CreateAnimation(json_data, "Idle");
    creature_manager->CreateAnimation(json_data, "Still");
    creature_manager->CreateAnimation(json_data, "Jumping");
    creature_manager->CreateAnimation(json_data, "Falling");
    creature_manager->CreateAnimation(json_data, "Sliding");
    creature_manager->SetActiveAnimationName("Still");
    creature_manager->SetAutoBlending(true);
    creature_manager->SetIsPlaying(true);

}

void Game::update(float timestep, GLFWwindow* window) {
    Input input{};
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        input.move.x -= 1.0f;
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        input.move.x += 1.0f;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        input.move.y += 1.0f;
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        input.move.y -= 1.0f;

    input.jump = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    input.jumpDown = !lastInput.jump && input.jump;

    //static bool bp;
    //if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    //    if (!bp) {
    //        player.vel += 1000.0f * mv;
    //        bp = true;
    //    }
    //} else {
    //    bp = false;
    //}
    lastInput = input;
    player.update(timestep, input, platforms);
}

float f = 0;
void Game::render(float delta, float catchup, glm::ivec2 screensize) {
    cam.aspect = (float)screensize.x / screensize.y;
    cam.position = glm::mix(cam.position, player.pos + player.vel * catchup, glm::clamp(1-pow(0.1f, delta),0.0f, 1.0f));
    cam.update();

    program.bind();
    staticvao.bind();
    glUniform3f(program.getUniformLocation("color"), 1.0f, 0.5f, 0.7f);
    glUniformMatrix4fv(program.getUniformLocation("cam"), 1, GL_FALSE, glm::value_ptr(mat4(1.0f)));
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glUniform3f(program.getUniformLocation("color"), 0.0f, 0.43f, 0.67f);
    glUniformMatrix4fv(program.getUniformLocation("cam"), 1, GL_FALSE, glm::value_ptr(cam.matrix));
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

    creature_manager->AutoBlendTo(player.state->name, delta * 10);
    creature_manager->SetMirrorY(player.lookToLeft);
    creature_manager->Update(delta);
    creature_manager->GetCreature()->FillRenderColours(255,255,255,255);
    creature_renderer->draw(player.pos + player.vel * catchup, 17, cam);

}

Game::~Game() = default;

void Game::ongui() {
    if(ImGui::Button("Reset Player")){
        player.pos = vec2(40, 290);
        player.vel = vec2(0,0);
        player.force = vec2(0,0);
        cam.position = vec2(0,0);
    }
    ImGui::Checkbox("Show Hitbox", &showphitbox);
    ImGui::Text("vel: [%.2f,%.2f]", player.vel.x, player.vel.y);
    ImGui::Text("state: %s", player.state->name.c_str());
}

