#include <imgui.h>
#include "guihelper.h"

void jnr::guihelper::beginInfoOverlay(int *corner) {
    const float DISTANCE = 10.0f;
    ImGuiIO &io = ImGui::GetIO();
    if (*corner != -1) {
        ImVec2 window_pos = ImVec2((*corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE,
                                   (*corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
        ImVec2 window_pos_pivot = ImVec2((*corner & 1) ? 1.0f : 0.0f, (*corner & 2) ? 1.0f : 0.0f);
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    }
    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
    ImGuiWindowFlags window_flags =
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (*corner != -1)
        window_flags |= ImGuiWindowFlags_NoMove;
    ImGui::Begin("Info-Overlay", NULL, window_flags);
    if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::MenuItem("Custom", NULL, *corner == -1)) *corner = -1;
        if (ImGui::MenuItem("Top-left", NULL, *corner == 0)) *corner = 0;
        if (ImGui::MenuItem("Top-right", NULL, *corner == 1)) *corner = 1;
        if (ImGui::MenuItem("Bottom-left", NULL, *corner == 2)) *corner = 2;
        if (ImGui::MenuItem("Bottom-right", NULL, *corner == 3)) *corner = 3;
        ImGui::EndPopup();
    }
}
