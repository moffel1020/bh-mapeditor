#include "editor.h"
#include "imgui.h"
#include "logger.h"
#include "map.h"
#include "mapinfo.h"
#include "mapobject.h"
#include "nfd.hpp"
#include "objectlist.h"
#include "objectproperties.h"
#include "raylib.h"
#include "raymath.h"
#include "rlImGui.h"
#include "rlgl.h"
#include "serializer.h"
#include "spawnpopup.h"
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace MousePickShader {
const static char* vert = "#version 330\n"
                          "in vec3 vertexPosition;\n"
                          "in vec2 vertexTexCoord;\n"
                          "in vec4 vertexColor;\n"
                          "out vec2 fragTexCoord;\n"
                          "out vec4 fragColor;\n"
                          "uniform mat4 mvp;\n"
                          "void main()\n"
                          "{\n"
                          "    fragTexCoord = vertexTexCoord;\n"
                          "    fragColor = vertexColor;\n"
                          "    gl_Position = mvp*vec4(vertexPosition, 1.0);\n"
                          "}\n";

const static char* frag = "#version 330\n"
                          "in vec2 fragTexCoord;\n"
                          "in vec4 fragColor;\n"
                          "uniform int id;\n"
                          "uniform sampler2D texture0;\n"
                          "layout(location=0) out int finalColor;\n"
                          "void main()\n"
                          "{\n"
                          "    finalColor = id;\n"
                          "}\n";
}

Editor* Editor::instance = nullptr;
Editor::Editor() { Editor::instance = this; }

void Editor::start() {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "map editor");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    findBrawlDir();
    map = std::make_shared<Map>();
    map->loadTestMap(brawlDir);

    rlImGuiSetup(true);

    loadMousePickingFramebuffer();
    objectIdFBO.shader = LoadShaderFromMemory(MousePickShader::vert, MousePickShader::frag);

    cam.zoom = 1.0f;
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

        if (IsWindowResized()) {
            shouldResizeFramebuffer = true;
        }

        // select objects by clicking on them
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && !ImGui::GetIO().WantCaptureMouse) {
            if (shouldResizeFramebuffer) {
                loadMousePickingFramebuffer();
                shouldResizeFramebuffer = false;
            }
            setSelectedObject(getObjectAtCoords(GetMouseX(), GetMouseY()));
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
    static bool showObjectList = true;
    static bool showObjectProperties = true;
    static bool showDemo = false;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Export to game")) {
                MapSerializer(map).serialize(brawlDir, true);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Object List", nullptr, &showObjectList);
            ImGui::MenuItem("Object Properties", nullptr, &showObjectProperties);
            ImGui::MenuItem("Map Info", nullptr, &showMapInfo);
            ImGui::MenuItem("Gui Demo", nullptr, &showDemo);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if ((IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE) || IsKeyReleased(KEY_SPACE)) &&
        !ImGui::GetIO().WantCaptureMouse) {
        ImGui::OpenPopup("Item Select");
    }

    ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

    // clang-format off
    if (showDemo) ImGui::ShowDemoWindow(&showDemo);
    if (showMapInfo) showMapInfoWindow(map.get(), &showMapInfo);
    if (showObjectList) showObjectListWindow(map.get(), &showObjectList);
    if (showObjectProperties) showObjectPropertiesWindow(selectedObject, map.get(), &showObjectProperties);
    showObjectSpawnPopup(map.get());
    // clang-format on

    rlImGuiEnd();
}

Editor::~Editor() {
    UnloadRenderTexture(objectIdFBO.framebuffer);
    UnloadShader(objectIdFBO.shader);
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
    for (auto& file : requiredFiles) {
        if (!std::filesystem::exists(dir.string() + "/" + file)) {
            Logger::error("Could not find " + file + " in " + dir.string());
            return false;
        }
    }

    return true;
}

void Editor::loadMousePickingFramebuffer() {
    if (!IsShaderReady(objectIdFBO.shader)) {
        objectIdFBO.shader = LoadShaderFromMemory(MousePickShader::vert, MousePickShader::frag);
    }

    if (objectIdFBO.framebuffer.id) {
        UnloadRenderTexture(objectIdFBO.framebuffer);
    }

    int w = GetScreenWidth();
    int h = GetScreenHeight();
    objectIdFBO.framebuffer.id = rlLoadFramebuffer(w, h);

    if (objectIdFBO.framebuffer.id > 0) {
        rlEnableFramebuffer(objectIdFBO.framebuffer.id);
        objectIdFBO.framebuffer.texture.id =
            rlLoadTexture(NULL, w, h, PIXELFORMAT_UNCOMPRESSED_R32I, 1);
        objectIdFBO.framebuffer.texture.width = w;
        objectIdFBO.framebuffer.texture.height = h;
        objectIdFBO.framebuffer.texture.format = PIXELFORMAT_UNCOMPRESSED_R32I;
        objectIdFBO.framebuffer.texture.mipmaps = 1;

        rlFramebufferAttach(objectIdFBO.framebuffer.id, objectIdFBO.framebuffer.texture.id,
                            RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
        if (rlFramebufferComplete(objectIdFBO.framebuffer.id)) {
            Logger::info("Succesfully created mouse picking framebuffer");
        }
        rlDisableFramebuffer();
    } else {
        Logger::warn("Could not create mouse picking framebuffer");
    }
}

std::weak_ptr<MapObject> Editor::getObjectAtCoords(int screenX, int screenY) {
    Logger::disableRlLog = true;
    BeginTextureMode(objectIdFBO.framebuffer);
    ClearBackground(WHITE);
    BeginShaderMode(objectIdFBO.shader);

    BeginMode2D(cam);
    rlSetLineWidth(15 * cam.zoom);
    // this is terrible but actually setting shader attribs is annoying in raylib
    for (size_t i = 0; i < map->mapObjects.size(); i++) {
        map->mapObjects[i]->draw();
        SetShaderValue(objectIdFBO.shader, GetShaderLocation(objectIdFBO.shader, "id"), &i,
                       RL_SHADER_UNIFORM_INT);
        rlDrawRenderBatchActive();
    }

    EndMode2D();
    EndShaderMode();
    EndTextureMode();

    auto img = LoadImageFromTexture(objectIdFBO.framebuffer.texture);

    int32_t data = ((int32_t*)img.data)[screenX + (img.height - screenY) * img.width];
    UnloadImage(img);

    std::weak_ptr<MapObject> object;
    if (data >= 0 && data <= (int32_t)map->mapObjects.size()) {
        object = map->mapObjects[data];
    }

    Logger::disableRlLog = false;
    return object;
}