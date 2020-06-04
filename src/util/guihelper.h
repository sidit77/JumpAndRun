#pragma once

#include "config.h"
#include <imgui.h>

namespace jnr{
    enum GuiSection{
        INFO, GRAPHICS, DEBUG
    };
    namespace guihelper {
        void beginInfoOverlay(toml::Value& loc);
        void beginSaved(toml::Value& loc, const std::string& title, bool* open = NULL, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
        void setupStyle();
        bool Button(const char *label, bool enabled = true, const ImVec2 &size = ImVec2(0,0));
    };
}

