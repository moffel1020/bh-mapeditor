#include "editor.h"
#include "imgui.h"
#include "logger.h"
#include "map.h"
#include "mapinfo.h"
#include "nfd.hpp"
#include "objectviewer.h"
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
    rlImGuiBegin();

    static bool showMapInfo = true;
    static bool showObjectView = true;
    static bool showDemo = false;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Object viewer", nullptr, &showObjectView);
            ImGui::MenuItem("Map Info", nullptr, &showMapInfo);
            ImGui::MenuItem("Gui Demo", nullptr, &showDemo);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

    // clang-format off
    if (showDemo) ImGui::ShowDemoWindow(&showDemo);
    if (showMapInfo) showMapInfoWindow(map.get(), &showMapInfo);
    if (showObjectView) showObjectViewWindow(map.get(), &showMapInfo);
    // clang-format on

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
