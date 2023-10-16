#include "objectviewer.h"
#include "editor.h"
#include "imgui.h"
#include "mapobject.h"
#include <memory>

void showObjectViewWindow(Map* map, bool* open) {
    ImGui::Begin("Objects", open);

    for (size_t i = 0; i < map->mapObjects.size(); i++) {
        std::shared_ptr<MapObject> o = map->mapObjects[i];

        if (ImGui::Selectable((o->getTypeName() + std::to_string(i)).c_str(),
                              o == Editor::get().getSelectedobject().lock())) {
            Editor::get().setSelectedObject(o);
        }
    }

    ImGui::End();
}