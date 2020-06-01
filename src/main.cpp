#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_glfw.h>
#include <toml++/toml.h>
#include "game.h"
#include "config.h"

void error_callback(int error, const char* description) {
    std::cout << "Error" << description << std::endl;
}

int main() {
    GLFWwindow* window;

    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

    glfwSetErrorCallback(error_callback);

    //auto tomlconfig = toml::parse(R"(
    //    [graphics]
    //    fullscreen = false
    //    vsync = 1
//
    //    [dependencies]
    //    cpp = 17
    //)");

    window = glfwCreateWindow(1280, 720, "Jump And Run", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);

    glClearColor(0.043f, 0.31f, 0.424f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");


    {
        jnr::Game game(window);

        glfwSetWindowMonitor(window, NULL,
                game.getConfig().get()["display"]["x"].ref<int64_t>(),
                game.getConfig().get()["display"]["y"].ref<int64_t>(),
                game.getConfig().get()["display"]["w"].ref<int64_t>(),
                game.getConfig().get()["display"]["h"].ref<int64_t>(),
                GLFW_DONT_CARE);

        double lastupdate = glfwGetTime();
        double lastframe = glfwGetTime();

        while (!glfwWindowShouldClose(window)){
            int64_t timestep = game.getConfig().get()["graphics"]["timestep"].ref<int64_t>();
            float speed = (float)game.getConfig().get()["graphics"]["speed"].ref<double>();
            while (glfwGetTime() - lastupdate > 1.0 / timestep) {
                lastupdate += 1.0 / timestep;
                game.update(speed * (1.0f / timestep));

            }

            glm::ivec2 screensize;
            glfwGetFramebufferSize(window, &screensize.x, &screensize.y);
            glViewport(0, 0, screensize.x, screensize.y);

            glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT); //

            double delta = glfwGetTime() - lastframe;
            game.render(
                    speed * delta,
                    speed * (glfwGetTime() - lastupdate) * (game.getConfig().get()["graphics"]["movement_smoothing"].ref<bool>() ? 1 : 0),
                    screensize);
            lastframe += delta;

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            game.ongui();
            //ImGui::Checkbox("Fullsceen")
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapInterval(game.getConfig().get()["graphics"]["vsync"].ref<int64_t>());
            glfwSwapBuffers(window);
            glfwPollEvents();
            {
                bool fullscreen = game.getConfig().get()["graphics"]["fullscreen"].ref<bool>();
                if((glfwGetWindowMonitor(window) != NULL) != fullscreen){
                    GLFWmonitor* monitor = glfwGetPrimaryMonitor();//;
                    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                    if(fullscreen) {
                        int spbackup[4];
                        glfwGetWindowPos(window, &spbackup[0], &spbackup[1]);
                        glfwGetWindowSize(window, &spbackup[2], &spbackup[3]);
                        game.getConfig().get()["display"]["x"].ref<int64_t>() = spbackup[0];
                        game.getConfig().get()["display"]["y"].ref<int64_t>() = spbackup[1];
                        game.getConfig().get()["display"]["w"].ref<int64_t>() = spbackup[2];
                        game.getConfig().get()["display"]["h"].ref<int64_t>() = spbackup[3];
                        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
                    } else {
                        glfwSetWindowMonitor(window, NULL,
                                             game.getConfig().get()["display"]["x"].ref<int64_t>(),
                                             game.getConfig().get()["display"]["y"].ref<int64_t>(),
                                             game.getConfig().get()["display"]["w"].ref<int64_t>(),
                                             game.getConfig().get()["display"]["h"].ref<int64_t>(),
                                             GLFW_DONT_CARE);
                    }
                }
            }

        }

        if(!game.getConfig().get()["graphics"]["fullscreen"].ref<bool>()){
            int spbackup[4];
            glfwGetWindowPos(window, &spbackup[0], &spbackup[1]);
            glfwGetWindowSize(window, &spbackup[2], &spbackup[3]);
            game.getConfig().get()["display"]["x"].ref<int64_t>() = spbackup[0];
            game.getConfig().get()["display"]["y"].ref<int64_t>() = spbackup[1];
            game.getConfig().get()["display"]["w"].ref<int64_t>() = spbackup[2];
            game.getConfig().get()["display"]["h"].ref<int64_t>() = spbackup[3];
        }
    }
    //std::cout << config << std::endl;

    glfwTerminate();
    return 0;
}