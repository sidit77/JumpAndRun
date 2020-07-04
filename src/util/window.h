#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <functional>
#include <memory>

template<typename T>
using deleted_unique_ptr = std::unique_ptr<T,std::function<void(T*)>>;

namespace jnr::services {
    inline deleted_unique_ptr<GLFWwindow> window;
}
