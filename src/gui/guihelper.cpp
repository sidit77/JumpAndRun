#include <imgui.h>
#include "guihelper.h"

void jnr::guihelper::beginSaved(toml::Value &loc, const std::string& title, bool *open, ImGuiWindowFlags flags) {
    if(loc.has("x") && loc.has("x")){
        ImGui::SetNextWindowPos(ImVec2(
                loc["x"].as<double>(),
                loc["y"].as<double>()), ImGuiCond_Once);
    }
    if(loc.has("w") && loc.has("h")){
        ImGui::SetNextWindowSize(ImVec2(
                loc["w"].as<double>(),
                loc["h"].as<double>()), ImGuiCond_Once);
    }
    ImGui::Begin(title.c_str(), open, flags);
    ImVec2 pos = ImGui::GetWindowPos();
    loc["x"] = pos.x;
    loc["y"] = pos.y;
    ImVec2 size = ImGui::GetWindowSize();
    loc["w"] = size.x;
    loc["h"] = size.y;
}

void jnr::guihelper::beginInfoOverlay(toml::Value& loc) {
    int corner = getOrDefault(loc["corner"], 0);
    const float DISTANCE = 10.0f;
    ImGuiIO &io = ImGui::GetIO();
    if (corner != 4) {
        ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE,
                                   (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
        ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    }
    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
    ImGuiWindowFlags window_flags =
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (corner != 4)
        window_flags |= ImGuiWindowFlags_NoMove;

    if(corner == 4)
        beginSaved(loc, "Info-Overlay", NULL, window_flags);
    else
        ImGui::Begin("Info-Overlay", NULL, window_flags);

    if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::MenuItem("Custom", NULL, corner == 4)) loc["corner"] = 4;
        if (ImGui::MenuItem("Top-left", NULL, corner == 0)) loc["corner"] = 0;
        if (ImGui::MenuItem("Top-right", NULL, corner == 1)) loc["corner"] = 1;
        if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) loc["corner"] = 2;
        if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) loc["corner"] = 3;
        ImGui::EndPopup();
    }
}


void jnr::guihelper::setupStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(10, 8);
    style.FramePadding = ImVec2(4,4);
    style.ItemSpacing = ImVec2(8,4);
    style.ItemInnerSpacing = ImVec2(4,4);
    style.TouchExtraPadding = ImVec2(0,0);
    style.IndentSpacing = 18;
    style.ScrollbarSize = 10;
    style.GrabMinSize = 10;

    style.WindowBorderSize = 0;
    style.ChildBorderSize = 0;
    style.PopupBorderSize = 0;
    style.FrameBorderSize = 0;
    style.TabBorderSize = 0;

    style.WindowRounding = 10;
    style.ChildRounding = 10;
    style.FrameRounding = 5;
    style.PopupRounding = 8;
    style.ScrollbarRounding = 12;
    style.GrabRounding = 5;
    style.TabRounding = 4;

    style.WindowTitleAlign = ImVec2(0.02f, 0.50f);
    style.WindowMenuButtonPosition = ImGuiDir_None;
    style.ColorButtonPosition = ImGuiDir_Right;
    style.ButtonTextAlign = ImVec2(0.5f,0.5f);
    style.SelectableTextAlign = ImVec2(0,0);

    style.DisplaySafeAreaPadding = ImVec2(3,3);

    style.Alpha = 0.9f;
}

