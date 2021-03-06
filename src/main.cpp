#include <iostream>

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include "util/window.h"
#include "util/config.h"
#include "util/filesystem.h"
#include "leveleditor.h"

void KHRONOS_APIENTRY message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
{
    auto const src_str = [source]() {
        switch (source)
        {
            case GL_DEBUG_SOURCE_API: return "API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
            case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
            case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
            case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
            case GL_DEBUG_SOURCE_OTHER: return "OTHER";
            default: return "-";
        }
    }();

    auto const type_str = [type]() {
        switch (type)
        {
            case GL_DEBUG_TYPE_ERROR: return "ERROR";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
            case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
            case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
            case GL_DEBUG_TYPE_MARKER: return "MARKER";
            case GL_DEBUG_TYPE_OTHER: return "OTHER";
            default: return "-";
        }
    }();

    auto const severity_str = [severity]() {
        switch (severity) {
            case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
            case GL_DEBUG_SEVERITY_LOW: return "LOW";
            case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
            case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
            default: return "-";
        }
    }();

    std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << '\n';
}

int main() {

    jnr::services::config = std::make_unique<jnr::Config>("config.toml");
    jnr::services::filesystem = std::make_unique<jnr::FileSystem>();

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

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
    glDebugMessageCallback(&message_callback, nullptr);

    {
        auto game = std::unique_ptr<jnr::Game>(new jnr::LevelEditor());
        game->run();
    }

    ImGui_ImplGlfw_Shutdown();

    jnr::services::window.reset();

    glfwTerminate();
    return 0;
}

