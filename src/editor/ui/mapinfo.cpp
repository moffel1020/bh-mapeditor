#include "mapinfo.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "selectimage.h"

void showMapInfoWindow(Map* map, bool* open) {
    ImGui::Begin("Map Info", open);
    ImGui::InputTextWithHint("Map name", "Enter name", &map->name,
                             ImGuiInputTextFlags_CharsNoBlank);
    if (ImGui::Button(map->background.getFilename().c_str())) {
        map->background.setImage(selectImageFile());
    }
    ImGui::SameLine();
    ImGui::Text("Background");
    ImGui::Image(&map->background.tex.id, ImVec2(90 * map->background.ratio, 90));
    ImGui::SeparatorText("Weapon spawn color");
    ImGui::ColorEdit3("inner", map->weaponColor.inner, ImGuiColorEditFlags_NoInputs);
    ImGui::ColorEdit3("outer", map->weaponColor.outer, ImGuiColorEditFlags_NoInputs);
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Camera Bounds")) {
        ImGui::DragFloat("x", &map->camBounds.x, 10.0f);
        ImGui::DragFloat("y", &map->camBounds.y, 10.0f);
        ImGui::DragFloat("w", &map->camBounds.w, 10.0f, 1.0f, FLT_MAX, "%f",
                         ImGuiSliderFlags_AlwaysClamp);
        ImGui::DragFloat("h", &map->camBounds.h, 10.0f, 1.0f, FLT_MAX, "%f",
                         ImGuiSliderFlags_AlwaysClamp);
    }

    if (ImGui::CollapsingHeader("Kill Bounds")) {
        ImGui::DragInt("left", &map->killBounds.left, 10.0f, 1, INT_MAX, "%d",
                       ImGuiSliderFlags_AlwaysClamp);
        ImGui::DragInt("right", &map->killBounds.right, 10.0f, 1, INT_MAX, "%d",
                       ImGuiSliderFlags_AlwaysClamp);
        ImGui::DragInt("top", &map->killBounds.top, 10.0f, 1, INT_MAX, "%d",
                       ImGuiSliderFlags_AlwaysClamp);
        ImGui::DragInt("bottom", &map->killBounds.bottom, 10.0f, 1, INT_MAX, "%d",
                       ImGuiSliderFlags_AlwaysClamp);
    }

    if (ImGui::Button(map->thumbnail.getFilename().c_str())) {
        map->background.setImage(selectImageFile());
    }
    ImGui::SameLine();
    ImGui::Text("Thumbnail");
    ImGui::Image(&map->thumbnail.tex.id, ImVec2(90 * map->thumbnail.ratio, 90));
    ImGui::End();
}