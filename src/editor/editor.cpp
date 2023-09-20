#include "editor.h"
#include "imgui.h"
#include "map.h"
#include "raylib.h"
#include "raymath.h"
#include "rlImGui.h"
#include "rlgl.h"
#include <filesystem>
#include <iostream>
#include <memory>
#include <vector>

void Editor::start() {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "map editor");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    map = std::make_unique<Map>();

    rlImGuiSetup(true);

    cam.zoom = 1.0f;
}

void Editor::run() {
    ImGuiIO &io = ImGui::GetIO();
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

            const float zoomIncrement = 0.125f;
            cam.zoom += (wheel * zoomIncrement);
            if (cam.zoom < zoomIncrement) {
                cam.zoom = zoomIncrement;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);
        rlImGuiBegin();

        bool open = true;
        ImGui::ShowDemoWindow(&open);

        map->draw(cam);

        DrawFPS(0, 0);
        rlImGuiEnd();
        EndDrawing();
    }
}

Editor::~Editor() {
    rlImGuiShutdown();
    CloseWindow();
}
