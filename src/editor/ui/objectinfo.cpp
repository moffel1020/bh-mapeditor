#include "objectinfo.h"
#include "editor.h"
#include "imgui.h"
#include "map.h"
#include "mapobject.h"
#include "objecttypes.h"
#include "selectimage.h"

void showObjectInfoWindow(std::weak_ptr<MapObject> mapObject, Map* map, bool* open) {
    if (mapObject.expired()) {
        return;
    }

    std::shared_ptr<MapObject> object = mapObject.lock();

    ImGui::Begin("Object Info", open);
    ImGui::Text("type: %s", object->getTypeName());

    switch (object->getType()) {
    case MapObjectType::Platform: {
        Platform* platform = static_cast<Platform*>(object.get());
        if (ImGui::Button(("file: " + platform->img->getFilename()).c_str())) {
            auto image = selectImageFile();
            if (image.has_value()) {
                platform->img->setImage(image.value());
            }
        }
        ImGui::SameLine();
        ImGui::Text("Image");
        ImGui::Image(&platform->img->tex.id, ImVec2(60 * platform->img->ratio, 60));
        ImGui::DragFloat("x", &platform->x, 5.0f);
        ImGui::DragFloat("y", &platform->y, 5.0f);
        ImGui::DragFloat("w", &platform->w, 5.0f, 1.0f, FLT_MAX, "%f",
                         ImGuiSliderFlags_AlwaysClamp);
        ImGui::DragFloat("h", &platform->h, 5.0f, 1.0f, FLT_MAX, "%f",
                         ImGuiSliderFlags_AlwaysClamp);
        if (ImGui::Button("Delete")) {
            map->removeObject(platform);
        }
        ImGui::Text("x: %f", platform->x);
        ImGui::Text("y: %f", platform->y);
        break;
    }

    case MapObjectType::Respawn: {
        Respawn* respawn = static_cast<Respawn*>(object.get());
        ImGui::Checkbox("Initial", &respawn->init);
        ImGui::DragFloat("x", &respawn->x, 5.0f);
        ImGui::DragFloat("y", &respawn->y, 5.0f);
        if (ImGui::Button("Delete")) {
            map->removeObject(respawn);
        }
        break;
    }

    case MapObjectType::ItemSpawn: {
        ItemSpawn* itemSpawn = static_cast<ItemSpawn*>(object.get());
        ImGui::Checkbox("Initial", &itemSpawn->init);
        ImGui::DragFloat("x", &itemSpawn->x, 5.0f);
        ImGui::DragFloat("y", &itemSpawn->y, 5.0f);
        if (ImGui::Button("Delete")) {
            map->removeObject(itemSpawn);
        }
        break;
    }
    case MapObjectType::Collision: {
        Collision* collision = static_cast<Collision*>(object.get());
        const char* colTypes[] = {"Hard", "Soft"}; // bad but it works for now
        ImGui::Combo("type", (int*)(&collision->collisionType), colTypes, 2);
        ImGui::DragFloat("x1", &collision->x1, 5.0f);
        ImGui::DragFloat("y1", &collision->y1, 5.0f);
        ImGui::DragFloat("x2", &collision->x2, 5.0f);
        ImGui::DragFloat("y2", &collision->y2, 5.0f);
        if (ImGui::Button("Delete")) {
            map->removeObject(collision);
        }
        break;
    }
    default:
        break;
    }

    ImGui::End();
}