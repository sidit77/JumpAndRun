#pragma once

#include <memory>
#include "util/config.h"

template<typename T>
using deleted_unique_ptr = std::unique_ptr<T,std::function<void(T*)>>;

namespace jnr::services {

    inline std::unique_ptr<Config> config;
#ifdef _glfw3_h_
    inline deleted_unique_ptr<GLFWwindow> window;
#endif
}


