#include <game.h>
#include <glad/glad.h>

void Game::update(float timestep) {

}

void Game::render() {
    glColor3f(0.5f,0.5f,1.0f);
    glBegin(GL_QUADS);
    glVertex2f(100,100);
    glVertex2f(100+200,100);
    glVertex2f(100+200,100+200);
    glVertex2f(100,100+200);
    glEnd();
}

Game::Game() {

}

Game::~Game() {

}
