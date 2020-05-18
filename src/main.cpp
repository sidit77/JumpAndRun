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

    int vsync = 1;
    bool fullscreen = false;
    int spbackup[4];
    window = glfwCreateWindow(1280, 720, "Jump And Run", fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
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
        jnr::Game game;

        bool movint = true;
        double lastupdate = glfwGetTime();
        double lastframe = glfwGetTime();
        int timestep = 60;
        float speed = 1;

        while (!glfwWindowShouldClose(window)){
            while (glfwGetTime() - lastupdate > 1.0 / timestep) {
                lastupdate += 1.0 / timestep;
                game.update(speed * (1.0f / (float)timestep), window);
            }

            glm::ivec2 screensize;
            glfwGetFramebufferSize(window, &screensize.x, &screensize.y);
            glViewport(0, 0, screensize.x, screensize.y);

            glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT); //

            double delta = glfwGetTime() - lastframe;
            game.render(speed * delta,speed * (glfwGetTime() - lastupdate) * (movint ? 1 : 0), screensize);
            lastframe += delta;

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            {
                ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
                ImGui::SliderInt("timestep", &timestep, 1, 300);
                ImGui::SliderFloat("speed", &speed, 0, 3);
                ImGui::Checkbox("Fullscreen", &fullscreen);
                ImGui::SliderInt("Vsync", &vsync, 0, 8);
                ImGui::Checkbox("mov. pred.", &movint);
                //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

                game.ongui();


            }
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapInterval(vsync);
            glfwSwapBuffers(window);
            glfwPollEvents();

            if((glfwGetWindowMonitor(window) != NULL) != fullscreen){
                GLFWmonitor* monitor = glfwGetPrimaryMonitor();//;
                const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                if(fullscreen) {
                    glfwGetWindowPos(window, &spbackup[0], &spbackup[1]);
                    glfwGetWindowSize(window, &spbackup[2], &spbackup[3]);
                    glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
                } else {
                    glfwSetWindowMonitor(window, NULL, spbackup[0], spbackup[1], spbackup[2], spbackup[3], GLFW_DONT_CARE);
                }
            }
        }
    }

    glfwTerminate();
    return 0;
}