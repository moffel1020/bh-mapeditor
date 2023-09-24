#include "editor.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "logger.h"
#include "map.h"
#include "mapdata.h"
#include "nfd.hpp"
#include "raylib.h"
#include "raymath.h"
#include "rlImGui.h"
#include "rlgl.h"
#include <climits>
#include <filesystem>
#include <iostream>
#include <math.h>
#include <memory>
#include <string>
#include <vector>

void Editor::start() {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "map editor");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    map = std::make_unique<Map>();

    rlImGuiSetup(true);

    cam.zoom = 1.0f;

    findBrawlDir();
}

void Editor::run() {
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    while (!WindowShouldClose()) {
        // drag move camera with right click
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f / cam.zoom);

            cam.target = Vector2Add(cam.target, delta);
        }

        // zoom with scroll wheel
        float wheel = GetMouseWheelMove();
        if (wheel != 0 && !io.WantCaptureMouse) {
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), cam);

            cam.offset = GetMousePosition();
            cam.target = mouseWorldPos;

            const float zoomIncrement = 0.075f;
            cam.zoom += (wheel * zoomIncrement);
            if (cam.zoom < zoomIncrement) {
                cam.zoom = zoomIncrement;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);
        map->draw(cam);
        gui();

        EndDrawing();
    }
}

void Editor::gui() {
    // this is kinda shit but whatever

    rlImGuiBegin();

    static bool showObjects = true;
    static bool showMapInfo = true;
    static bool showDemo = false;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Object viewer", nullptr, &showObjects);
            ImGui::MenuItem("Map Info", nullptr, &showMapInfo);
            ImGui::MenuItem("Gui Demo", nullptr, &showDemo);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

    if (showDemo) {
        ImGui::ShowDemoWindow(&showDemo);
    }

    if (showMapInfo) {
        ImGui::Begin("Map Info", &showMapInfo);
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

    if (showObjects) {
        ImGui::Begin("Objects", &showObjects);

        if (ImGui::CollapsingHeader("Platforms")) {
            for (size_t i = 0; i < map->platforms.size(); i++) {
                if (ImGui::TreeNode((void*)&map->platforms[i], "platform %d", (int)i)) {
                    if (ImGui::Button(map->platforms[i].img.getFilename().c_str())) {
                        map->platforms[i].img.setImage(selectImageFile());
                    }
                    ImGui::SameLine();
                    ImGui::Text("Image");
                    ImGui::Image(&map->platforms[i].img.tex.id,
                                 ImVec2(60 * map->platforms[i].img.ratio, 60));
                    ImGui::DragFloat("x", &map->platforms[i].x, 5.0f);
                    ImGui::DragFloat("y", &map->platforms[i].y, 5.0f);
                    ImGui::DragFloat("w", &map->platforms[i].w, 5.0f, 1.0f, FLT_MAX, "%f",
                                     ImGuiSliderFlags_AlwaysClamp);
                    ImGui::DragFloat("h", &map->platforms[i].h, 5.0f, 1.0f, FLT_MAX, "%f",
                                     ImGuiSliderFlags_AlwaysClamp);
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
                    ImGui::TreePop();
                }
            }
        }
        ImGui::End();
    }

    rlImGuiEnd();
}

Editor::~Editor() {
    rlImGuiShutdown();
    CloseWindow();
}

void Editor::findBrawlDir() {
    // TODO: automatically detect this folder with vdf parser

    NFD::Init();
    nfdchar_t* out;

    bool foundDir = false;
    while (!foundDir) {
        auto result = NFD::PickFolder(out);
        if (result == NFD_OKAY && isValidBrawlDir(out)) {
            brawlDir = out;
            foundDir = true;
            NFD::FreePath(out);
        }
    }

    Logger::info("Found brawlhalla path: " + brawlDir);
    NFD::Quit();
}

bool Editor::isValidBrawlDir(const std::filesystem::path& dir) const {
    const std::string requiredFiles[] = {"Dynamic.swz", "BrawlhallaAir.swf"};
    for (auto file : requiredFiles) {
        if (!std::filesystem::exists(dir.string() + "/" + file)) {
            Logger::error("Could not find " + file + " in " + dir.string());
            return false;
        }
    }

    return true;
}

std::string Editor::selectImageFile() const {
    NFD::Init();
    nfdchar_t* out;
    nfdfilteritem_t filter = {"Image", "png,jpg,jpeg"};
    auto result = NFD::OpenDialog(out, &filter, 1);
    if (result == NFD_OKAY) {
        NFD::FreePath(out);
        return std::string(out);
    }

    Logger::error("failed selecting image");
    return std::string("");
}