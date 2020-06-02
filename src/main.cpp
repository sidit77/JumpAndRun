#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_glfw.h>
#include "game.h"

void error_callback(int error, const char* description) {
    std::cout << "Error" << description << std::endl;
}

void configureWindow(GLFWwindow* window, jnr::Config& conf);
void backupWindow(GLFWwindow* window, jnr::Config& conf);

int main() {
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwSetErrorCallback(error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);


    jnr::Config config("config.toml");
    window = glfwCreateWindow(1280, 720, "Jump And Run", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwSetWindowUserPointer(window, &config);
    glfwSetWindowSizeCallback(window, [](GLFWwindow* window,int w,int h){
        if(glfwGetWindowMonitor(window) == NULL){
            auto* config = static_cast<jnr::Config *>(glfwGetWindowUserPointer(window));
            (*config)["display"]["w"] = w;
            (*config)["display"]["h"] = h;
        }
    });

    glfwSetWindowPosCallback(window, [](GLFWwindow* window,int x,int y){
        if(glfwGetWindowMonitor(window) == NULL){
            auto* config = static_cast<jnr::Config *>(glfwGetWindowUserPointer(window));
            (*config)["display"]["x"] = x;
            (*config)["display"]["y"] = y;
        }
    });

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    configureWindow(window, config);


    glClearColor(0.043f, 0.31f, 0.424f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = NULL;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");


    {
        jnr::Game game(config, window);

        double lastupdate = glfwGetTime();
        double lastframe = glfwGetTime();

        while (!glfwWindowShouldClose(window)){
            if(game.getConfig().dirty){
                game.getConfig().dirty = false;
                configureWindow(window, config);
            }
            while (glfwGetTime() - lastupdate > 1.0 / game.getDebugOptions().timestep) {
                lastupdate += 1.0 / game.getDebugOptions().timestep;
                game.update(game.getDebugOptions().speed * (1.0 / game.getDebugOptions().timestep));

            }

            glm::ivec2 screensize;
            glfwGetFramebufferSize(window, &screensize.x, &screensize.y);
            glViewport(0, 0, screensize.x, screensize.y);

            glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT); //

            double delta = glfwGetTime() - lastframe;
            game.render(
                    game.getDebugOptions().speed * delta,
                    game.getDebugOptions().speed * (glfwGetTime() - lastupdate) * (game.getDebugOptions().movement_smoothing ? 1 : 0),
                    screensize);
            lastframe += delta;

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            game.ongui();
            //ImGui::Checkbox("Fullsceen")
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }
    //std::cout << config << std::endl;

    glfwTerminate();
    return 0;
}

void configureWindow(GLFWwindow* window, jnr::Config& conf){
    glfwSwapInterval(conf["display"]["vsync"].as<int>());

    if(conf["display"]["fullscreen"].as<bool>()) {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
    } else {
        glfwSetWindowMonitor(window, NULL,
                             conf["display"]["x"].as<int>(),
                             conf["display"]["y"].as<int>(),
                             conf["display"]["w"].as<int>(),
                             conf["display"]["h"].as<int>(),
                             GLFW_DONT_CARE);
    }
}
