#include "mainmenu.h"
#include "../imgui/imgui.h"
#include "login.h"
#include "settings.h"
#include "gui.h"


namespace mainmenu {

    
    static int currentTab = 0;
    bool mySwitch1 = false;
    bool mySwitch2 = false;
    bool mySwitch3 = false;

    void Render() {

        ImGui::SetCursorPos(ImVec2(10, 20)); // Position der Sidebar
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.12f, 0.12f, 1.0f)); // Grauer Hintergrund
        ImGui::BeginChild("Sidebar", ImVec2(140, 467), false, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.0f);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.35f, 0.20f, 0.55f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.30f, 0.70f, 1.0f)); // Hover
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.25f, 0.15f, 0.40f, 1.0f));
        
       
        ImGui::SetCursorPos(ImVec2(20, 375)); //Position
        if (ImGui::Button("Logout", ImVec2(100, 30))) {
            login::loggedIn = false;
        }

        ImGui::SetCursorPos(ImVec2(20, 135));//Position
        if (ImGui::Button("Dashboard", ImVec2(100, 30))) {
            currentTab = 0;
        }

        ImGui::SetCursorPos(ImVec2(20, 215));//Position
        if (ImGui::Button("Info", ImVec2(100, 30))) {
            currentTab = 1;
        }

        ImGui::SetCursorPos(ImVec2(20, 295));//Position
        if (ImGui::Button("Settings", ImVec2(100, 30))) {
            currentTab = 2;
        }

        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::PopStyleColor();

        
        ImGui::SetCursorPos(ImVec2(160, 20)); // Start-Position
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.12f, 0.12f, 1.0f)); // Hintergrundfarbe (dunkelgrau)
        ImGui::BeginChild("Content", ImVec2(677, 467), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

        // Tabs-Inhalt
        ImGui::SetCursorPos(ImVec2(250,15));
        if (currentTab == 0) {
            ImGui::Text("Willkommen im Dashboard!");
            ImGui::Separator();
           
            
        }
        else if (currentTab == 1) {
            ImGui::Text("Info");
            ImGui::Separator();
        }
        else if (currentTab == 2) {
            ImGui::Text("Settings");
            ImGui::Separator();

            ImGui::SetCursorPos(ImVec2(20, 60));
            ToggleSwitch1("Dark/Light Mode", mySwitch1);
            ImGui::SetCursorPos(ImVec2(20, 90));
            ToggleSwitch2("Brightness High/Low", mySwitch2);
            ImGui::SetCursorPos(ImVec2(20, 120));
            ToggleSwitch3("Window Glow effect on/off", mySwitch3);

            
            if (mySwitch2) {
                ImGuiStyle& style = ImGui::GetStyle();
                style.Alpha = 0.5f;
                gui::showText = false;
            }
            else {
                ImGuiStyle& style = ImGui::GetStyle();
                style.Alpha = 1.0f;  
            }
        }

        
        if (mySwitch1 == true) {
            ImGui::Text("Coming soon");
        }

        if (mySwitch2 == true) {
            
            
        }

        if (mySwitch3 == true) {
            ImGui::Text("Coming Soon");
        }

        ImGui::EndChild();
        ImGui::PopStyleColor();

        
        

        
        
    }

}
