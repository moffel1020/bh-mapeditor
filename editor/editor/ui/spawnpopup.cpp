#include "spawnpopup.h"
#include "editor.h"
#include "imgui.h"
#include "objecttypes.h"
#include "raylib.h"

void showObjectSpawnPopup(Map* map) {
    auto cam = Editor::get().getCam();

    if (ImGui::BeginPopup("Item Select")) {
        ImGui::Text("Spawn object");
        Vector2 coords = GetScreenToWorld2D(GetMousePosition(), cam);

        if (ImGui::Button("Respawn")) {
            map->addObject<Respawn>(coords.x, coords.y);
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::Button("Item Spawn")) {
            map->addObject<ItemSpawn>(coords.x, coords.y);
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::Button("Platform")) {
            map->addObject<Platform>(coords.x, coords.y);
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::Button("Soft Collision")) {
            map->addObject<SoftCollision>(coords.x, coords.y);
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::Button("Hard Collision")) {
            map->addObject<HardCollision>(coords.x, coords.y);
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}