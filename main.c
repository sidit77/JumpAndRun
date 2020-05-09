#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "game.h"

void error_callback(int error, const char* description) {
    printf("Error %s\n", description);
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

    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);

    testfunc();

    while (!glfwWindowShouldClose(window)){

        {
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, display_w, 0, display_h, 1, -1);
            glMatrixMode(GL_MODELVIEW);
            glViewport(0, 0, display_w, display_h);
        }

        glClear(GL_COLOR_BUFFER_BIT);

        glColor3f(0.5f,0.5f,1.0f);
        glBegin(GL_QUADS);
            glVertex2f(100,100);
            glVertex2f(100+200,100);
            glVertex2f(100+200,100+200);
            glVertex2f(100,100+200);
        glEnd();

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}