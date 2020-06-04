#pragma once

#include "../config.h"
#include <imgui.h>

namespace jnr{
    namespace guihelper {
        void beginInfoOverlay(toml::Value& loc);
        void beginSaved(toml::Value& loc, const std::string& title, bool* open = NULL, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
        void setupStyle();
        bool DisabledButton(const char *label, bool disabled, const ImVec2 &size = ImVec2(0,0));
    };
}

