#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_glfw.h>
#include <ttvfs.h>
#include "leveleditor.h"
#include "service.h"

void error_callback(int error, const char* description) {
    std::cout << "Error" << description << std::endl;
}

void configureWindow();

int main() {

    jnr::services::config = std::make_unique<jnr::Config>("config.toml");

    //ttvfs::Root vfs;
    //vfs.AddLoader(new ttvfs::DiskLoader);
//
    //ttvfs::File *vf = vfs.GetFile("config.toml");
    //if(!vf)
    //{
    //    printf("ERROR: myfile.txt does not exist\n");
    //    return 1;
    //}
    //if(!vf->open("r"))
    //{
    //    printf("ERROR: Failed to open myfile.txt for reading\n");
    //    return 2;
    //}
//
    //char buf[513];
    //size_t bytes = vf->read(buf, 512);
    //buf[bytes] = 0;
//
    //puts(buf);
//
    //vf->close();

    if (!glfwInit())
        return -1;

    glfwSetErrorCallback(error_callback);

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
        std::unique_ptr<jnr::PlayMode> game = std::unique_ptr<jnr::PlayMode>(new jnr::LevelEditorMode());
        game->run();
    }

    jnr::services::window.reset();

    glfwTerminate();
    return 0;
}

void configureWindow(){
    auto& conf = *jnr::services::config;
    if(conf["display"]["fullscreen"].as<bool>()) {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(jnr::services::window.get(), monitor, 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
    } else {
        glfwSetWindowMonitor(jnr::services::window.get(), NULL,
                             conf["display"]["x"].as<int>(),
                             conf["display"]["y"].as<int>(),
                             conf["display"]["w"].as<int>(),
                             conf["display"]["h"].as<int>(),
                             GLFW_DONT_CARE);
    }
}
