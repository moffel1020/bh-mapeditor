#include "imgui.h"
#include "objectviewer.h"
#include "selectimage.h"

void showObjectViewWindow(Map* map, bool* open) {
    ImGui::Begin("Objects", open);

    if (ImGui::CollapsingHeader("Platforms")) {
        for (size_t i = 0; i < map->platforms.size(); i++) {
            if (ImGui::TreeNode((void*)&map->platforms[i], "platform %d", (int)i)) {
                if (ImGui::Button(("file: " + map->platforms[i].img->getFilename()).c_str())) {
                    map->platforms[i].img->setImage(selectImageFile());
                }
                ImGui::SameLine();
                ImGui::Text("Image");
                ImGui::Image(&map->platforms[i].img->tex.id,
                             ImVec2(60 * map->platforms[i].img->ratio, 60));
                ImGui::DragFloat("x", &map->platforms[i].x, 5.0f);
                ImGui::DragFloat("y", &map->platforms[i].y, 5.0f);
                ImGui::DragFloat("w", &map->platforms[i].w, 5.0f, 1.0f, FLT_MAX, "%f",
                                 ImGuiSliderFlags_AlwaysClamp);
                ImGui::DragFloat("h", &map->platforms[i].h, 5.0f, 1.0f, FLT_MAX, "%f",
                                 ImGuiSliderFlags_AlwaysClamp);
                if (ImGui::Button("Delete")) {
                    map->platforms.erase(map->platforms.begin() + i);
                }
                ImGui::TreePop();
            }
        }
    }

    if (ImGui::CollapsingHeader("Collisions")) {
        for (size_t i = 0; i < map->collisions.size(); i++) {
            if (ImGui::TreeNode((void*)&map->collisions[i], "collision %d", (int)i)) {
                const char* colTypes[] = {"Hard", "Soft"}; // bad but it works for now
                ImGui::Combo("type", (int*)(&map->collisions[i].type), colTypes, 2);
                ImGui::DragFloat("x1", &map->collisions[i].x1, 5.0f);
                ImGui::DragFloat("y1", &map->collisions[i].y1, 5.0f);
                ImGui::DragFloat("x2", &map->collisions[i].x2, 5.0f);
                ImGui::DragFloat("y2", &map->collisions[i].y2, 5.0f);
                if (ImGui::Button("Delete")) {
                    map->collisions.erase(map->collisions.begin() + i);
                }
                ImGui::TreePop();
            }
        }
    }

    if (ImGui::CollapsingHeader("Item Spawns")) {
        for (size_t i = 0; i < map->itemSpawns.size(); i++) {
            if (ImGui::TreeNode((void*)&map->itemSpawns[i], "item %d", (int)i)) {
                ImGui::Checkbox("Initial", &map->itemSpawns[i].init);
                ImGui::DragFloat("x", &map->itemSpawns[i].x, 5.0f);
                ImGui::DragFloat("y", &map->itemSpawns[i].y, 5.0f);
                if (ImGui::Button("Delete")) {
                    map->itemSpawns.erase(map->itemSpawns.begin() + i);
                }
                ImGui::TreePop();
            }
        }
    }

    if (ImGui::CollapsingHeader("Respawns")) {
        for (size_t i = 0; i < map->respawns.size(); i++) {
            if (ImGui::TreeNode((void*)&map->respawns[i], "respawn %d", (int)i)) {
                ImGui::Checkbox("Initial", &map->respawns[i].init);
                ImGui::DragFloat("x", &map->respawns[i].x, 5.0f);
                ImGui::DragFloat("y", &map->respawns[i].y, 5.0f);
                if (ImGui::Button("Delete")) {
                    map->respawns.erase(map->respawns.begin() + i);
                }
                ImGui::TreePop();
            }
        }
    }
    ImGui::End();
}