#include <glad/glad.h>
#include "game.h"
#include <MeshBone.h>

using namespace jnr;
using namespace glm;

void PlayMode::update(float timestep) {
    Input input{};
    if(glfwGetKey(jnr::services::window.get(), GLFW_KEY_A) == GLFW_PRESS)
        input.move.x -= 1.0f;
    if(glfwGetKey(jnr::services::window.get(), GLFW_KEY_D) == GLFW_PRESS)
        input.move.x += 1.0f;
    if(glfwGetKey(jnr::services::window.get(), GLFW_KEY_W) == GLFW_PRESS)
        input.move.y += 1.0f;
    if(glfwGetKey(jnr::services::window.get(), GLFW_KEY_S) == GLFW_PRESS)
        input.move.y -= 1.0f;

    input.jump = glfwGetKey(jnr::services::window.get(), GLFW_KEY_SPACE) == GLFW_PRESS;

    if(glfwJoystickIsGamepad(GLFW_JOYSTICK_1)){
        GLFWgamepadstate state;
        if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)){
            input.jump |= state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS;

            vec2 mv(state.axes[GLFW_GAMEPAD_AXIS_LEFT_X],state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]);
            if(length(mv) > 0.3)
                input.move = mv;
        }
    }

    input.jumpDown = !lastInput.jump && input.jump;
    lastInput = input;
    player->update(timestep, input, level->get(false));
}

void PlayMode::render(float delta, float catchup) {
    glm::ivec2 screensize;
    glfwGetFramebufferSize(jnr::services::window.get(), &screensize.x, &screensize.y);
    cam->aspect = (float)screensize.x / screensize.y;
    cam->position = glm::mix(cam->position, player->pos + player->vel * catchup, glm::clamp(1-pow(0.1f, delta),0.0f, 1.0f));
    cam->scale = glm::mix(cam->scale, camera::default_scale, glm::clamp(1-pow(0.1f, delta),0.0f, 1.0f));
    cam->update();

    {
        colors::color fill_color(0xF0006EAB);
        colors::color line_color(0xFF00293F);
        for (const AABB &box : level->get().hitboxes) {
            primitiveRenderer->drawAABB(box.low, box.high, fill_color, 0.0f);
            primitiveRenderer->drawAABBOutlineP(box.low, box.high, line_color, 0.1f, 2);
        }
        primitiveRenderer->drawQuad(vec2(600, 300) + cam->position / 4.0f, vec2(800,300), line_color, -1.0f, vec2(0.5));
    }

    if(debugOptions->showPlayerHitbox)
        player->drawDebug(delta, catchup, *primitiveRenderer);

    primitiveRenderer->render(*cam);
    player->draw(delta, catchup, *cam);
}

void PlayMode::onGui() {

}
