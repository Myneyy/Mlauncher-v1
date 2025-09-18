#include "settings.h"
#include "../imgui/imgui.h"

void ToggleSwitch1(const char* name, bool& state, ImVec2 size) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();

    ImGui::InvisibleButton("##toggle1", size);
    bool held = ImGui::IsItemActive();

    
    if (ImGui::IsItemClicked()) {
        state = !state;
    }

    
    float t = state ? 1.0f : 0.0f;

    
    if (held) {
        float mouse_x = ImGui::GetIO().MousePos.x;
        t = (mouse_x - p.x - size.y / 2) / (size.x - size.y);
        if (t < 0) t = 0;
        if (t > 1) t = 1;
    }

    // Background
    ImU32 col_bg = IM_COL32(100, 100, 100, 255);
    draw_list->AddRectFilled(p, ImVec2(p.x + size.x, p.y + size.y), col_bg, size.y / 2);

    
    ImVec2 circle_pos = ImVec2(p.x + size.y / 2 + t * (size.x - size.y), p.y + size.y / 2);
    ImU32 col_circle = IM_COL32(255, 255, 255, 255);
    draw_list->AddCircleFilled(circle_pos, size.y / 2 - 2, col_circle);

    // Zustand beim Loslassen
    if (!held)
        state = t > 0.5f;

    ImGui::SameLine();
    ImGui::Text("%s", name);
}

void ToggleSwitch2(const char* name, bool& state, ImVec2 size) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();

    ImGui::InvisibleButton("##toggle2", size);
    bool held = ImGui::IsItemActive();

    
    if (ImGui::IsItemClicked()) {
        state = !state;
    }

    
    float t = state ? 1.0f : 0.0f;

    
    if (held) {
        float mouse_x = ImGui::GetIO().MousePos.x;
        t = (mouse_x - p.x - size.y / 2) / (size.x - size.y);
        if (t < 0) t = 0;
        if (t > 1) t = 1;
    }

    // Background
    ImU32 col_bg = IM_COL32(100, 100, 100, 255);
    draw_list->AddRectFilled(p, ImVec2(p.x + size.x, p.y + size.y), col_bg, size.y / 2);

    
    ImVec2 circle_pos = ImVec2(p.x + size.y / 2 + t * (size.x - size.y), p.y + size.y / 2);
    ImU32 col_circle = IM_COL32(255, 255, 255, 255);
    draw_list->AddCircleFilled(circle_pos, size.y / 2 - 2, col_circle);

    
    if (!held)
        state = t > 0.5f;

    ImGui::SameLine();
    ImGui::Text("%s", name);
}

void ToggleSwitch3(const char* name, bool& state, ImVec2 size) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();

    ImGui::InvisibleButton("##toggle3", size);
    bool held = ImGui::IsItemActive();

    
    if (ImGui::IsItemClicked()) {
        state = !state;
    }

    
    float t = state ? 1.0f : 0.0f;

    
    if (held) {
        float mouse_x = ImGui::GetIO().MousePos.x;
        t = (mouse_x - p.x - size.y / 2) / (size.x - size.y);
        if (t < 0) t = 0;
        if (t > 1) t = 1;
    }

    // Background
    ImU32 col_bg = IM_COL32(100, 100, 100, 255);
    draw_list->AddRectFilled(p, ImVec2(p.x + size.x, p.y + size.y), col_bg, size.y / 2);

   
    ImVec2 circle_pos = ImVec2(p.x + size.y / 2 + t * (size.x - size.y), p.y + size.y / 2);
    ImU32 col_circle = IM_COL32(255, 255, 255, 255);
    draw_list->AddCircleFilled(circle_pos, size.y / 2 - 2, col_circle);

    
    if (!held)
        state = t > 0.5f;

    ImGui::SameLine();
    ImGui::Text("%s", name);
}


