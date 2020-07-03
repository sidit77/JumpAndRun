#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_glfw.h>
#include <ttvfs.h>
#include "leveleditor.h"
#include "service.h"

int main() {

    jnr::services::config = std::make_unique<jnr::Config>("config.toml");

    if (!glfwInit())
        return -1;

    glfwSetErrorCallback([](int error, const char* description) {
        std::cout << "Error" << description << std::endl;
    });

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

    {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        bool f = jnr::getOrDefault((*jnr::services::config)["display"]["fullscreen"], false);
        int w = jnr::getOrDefault((*jnr::services::config)["display"]["width"], 1280);
        int h = jnr::getOrDefault((*jnr::services::config)["display"]["height"], 720);

        jnr::services::window = deleted_unique_ptr<GLFWwindow>(glfwCreateWindow(f ? mode->width : w, f ? mode->height : h, "Jump And Run", f ? monitor : NULL, NULL),
                                                               glfwDestroyWindow);
        if (!jnr::services::window) {
            glfwTerminate();
            return -1;
        }
    }
    glfwSetWindowSizeCallback(jnr::services::window.get(), [](GLFWwindow* window,int w,int h){
        if(glfwGetWindowMonitor(window) == NULL){
            auto* config = jnr::services::config.get();
            (*config)["display"]["width"] = w;
            (*config)["display"]["height"] = h;
        }
    });

    glfwMakeContextCurrent(jnr::services::window.get());
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(jnr::getOrDefault((*jnr::services::config)["display"]["vsync"], 1));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = NULL;
    jnr::guihelper::setupStyle();

    ImGui_ImplGlfw_InitForOpenGL(jnr::services::window.get(), true);
    ImGui_ImplOpenGL3_Init("#version 450");

    {
        auto game = std::unique_ptr<jnr::Game>(new jnr::LevelEditor());
        game->run();
    }

    jnr::services::window.reset();

    glfwTerminate();
    return 0;
}

