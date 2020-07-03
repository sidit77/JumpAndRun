#include "game.h"
#include <MeshBone.h>
#include <GLFW/glfw3.h>
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_glfw.h>
#include "service.h"

using namespace jnr;
using namespace glm;

PlayMode::PlayMode():
        debugOptions(std::make_shared<DebugOptions>()),
        cam(std::make_shared<Camera>()),
        player(std::make_shared<Player>("assets/character/character_data.json", "assets/character/character_atlas.png")),
        primitiveRenderer(std::make_shared<PrimitiveRenderer>()),
        level(std::make_shared<LevelWrapper>(getOrDefault<std::string>((*jnr::services::config)["level"]["name"], "assets/levels/level1.dat")))
{
}

void PlayMode::update(float timestep) {
    Input input{};
    if(glfwGetKey(services::window.get(), GLFW_KEY_A) == GLFW_PRESS)
        input.move.x -= 1.0f;
    if(glfwGetKey(services::window.get(), GLFW_KEY_D) == GLFW_PRESS)
        input.move.x += 1.0f;
    if(glfwGetKey(services::window.get(), GLFW_KEY_W) == GLFW_PRESS)
        input.move.y += 1.0f;
    if(glfwGetKey(services::window.get(), GLFW_KEY_S) == GLFW_PRESS)
        input.move.y -= 1.0f;

    input.jump = glfwGetKey(services::window.get(), GLFW_KEY_SPACE) == GLFW_PRESS;

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
    ivec2 screensize;
    glfwGetFramebufferSize(services::window.get(), &screensize.x, &screensize.y);
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

void PlayMode::run() {

    glClearColor(0.043f, 0.31f, 0.424f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);

    double lastupdate = glfwGetTime();
    double lastframe = glfwGetTime();

    while (!(glfwWindowShouldClose(jnr::services::window.get()) && canClose())){
        if(services::config->dirty){
            services::config->dirty = false;
            GLFWmonitor *monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);
            bool f = getOrDefault((*services::config)["display"]["fullscreen"], false);
            int w = getOrDefault((*services::config)["display"]["width"], 1280);
            int h = getOrDefault((*services::config)["display"]["height"], 720);
            glfwSetWindowMonitor(services::window.get(),
                    f ? monitor : NULL,
                    (mode->width  - w) / 2,(mode->height - h) / 2,
                    f ? mode->width : w, f ? mode->height : h, GLFW_DONT_CARE);
            glfwSwapInterval(getOrDefault((*services::config)["display"]["vsync"], 1));
        }
        while (glfwGetTime() - lastupdate > 1.0 / debugOptions->timestep) {
            lastupdate += 1.0 / debugOptions->timestep;
            update(debugOptions->speed * (1.0 / debugOptions->timestep));
        }
        glm::ivec2 screensize;
        glfwGetFramebufferSize(jnr::services::window.get(), &screensize.x, &screensize.y);
        glViewport(0, 0, screensize.x, screensize.y);

        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT); //

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        double delta = glfwGetTime() - lastframe;
        render(debugOptions->speed * delta,
               debugOptions->speed * (glfwGetTime() - lastupdate) * (debugOptions->movement_smoothing ? 1 : 0));
        lastframe += delta;


        onGui();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(jnr::services::window.get());
        glfwPollEvents();
    }
}


