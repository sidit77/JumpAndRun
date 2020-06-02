#include <imgui.h>
#include "guihelper.h"

void jnr::guihelper::beginInfoOverlay(Config& config) {
    int corner = config["ui"]["overlay"]["corner"].as<int>();
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
    else
        ImGui::SetNextWindowPos(ImVec2(
                config["ui"]["overlay"]["x"].as<double>(),
                config["ui"]["overlay"]["y"].as<double>()), ImGuiCond_Once);
    ImGui::Begin("Info-Overlay", NULL, window_flags);
    if(corner == 4){
        ImVec2 pos = ImGui::GetWindowPos();
        config["ui"]["overlay"]["x"] = pos.x;
        config["ui"]["overlay"]["y"] = pos.y;
    }
    if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::MenuItem("Custom", NULL, corner == 4)) config["ui"]["overlay"]["corner"] = 4;
        if (ImGui::MenuItem("Top-left", NULL, corner == 0)) config["ui"]["overlay"]["corner"] = 0;
        if (ImGui::MenuItem("Top-right", NULL, corner == 1)) config["ui"]["overlay"]["corner"] = 1;
        if (ImGui::MenuItem("Bottom-left", NULL, corner == 2)) config["ui"]["overlay"]["corner"] = 2;
        if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) config["ui"]["overlay"]["corner"] = 3;
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
