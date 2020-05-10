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

    window = glfwCreateWindow(1280, 720, "Jump And Run", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    glClearColor(0.043f, 0.31f, 0.424f, 1.0f);
    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");


    {
        jnr::Game game;

        double lastupdate = glfwGetTime();
        int timestep = 60;
        float speed = 1;

        while (!glfwWindowShouldClose(window)){
            while (glfwGetTime() - lastupdate > 1.0 / timestep) {
                lastupdate += 1.0 / timestep;
                game.update(speed * (1.0f / (float)timestep), window);
            }

            {
                int display_w, display_h;
                glfwGetFramebufferSize(window, &display_w, &display_h);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(0, display_w, 0, display_h, 1, -1);
                glMatrixMode(GL_MODELVIEW);
                glViewport(0, 0, display_w, display_h);
            }

            glClear(GL_COLOR_BUFFER_BIT); // | GL_DEPTH_BUFFER_BIT


            game.render((glfwGetTime() - lastupdate));

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            {
                ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
                ImGui::SliderInt("timestep", &timestep, 1, 300);
                ImGui::SliderFloat("speed", &speed, 0, 3);
                //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

                game.ongui();
            }
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}