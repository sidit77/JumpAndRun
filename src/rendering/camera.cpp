#include "camera.h"

#include <gtc/matrix_transform.hpp>

void jnr::Camera::update() {
    matrix = glm::ortho(position.x - (scale * aspect), position.x + (scale * aspect), position.y - scale, position.y + scale, -10.0f, 10.0f);
}
