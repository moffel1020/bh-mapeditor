#include "map.h"
#include "logger.h"
#include "raylib.h"
#include "rlgl.h"

Map::Map() {
    // placeholder test map
    setBackground("D:"
                  "\\games\\steamlibrary\\steamapps\\common\\Brawlhalla\\mapArt\\Backgrou"
                  "nds\\BG_Steam."
                  "jpg");

    camBounds = CameraBounds{-1364, 278.15f, 4928, 2772};
    killBounds = KillBounds{700, 700, 800, 850};

    platforms.emplace_back(LoadTexture("D:"
                                       "\\games\\steamlibrary\\steamapps\\common\\Brawlhall"
                                       "a\\mapArt\\Enigma\\Platform_"
                                       "Steam1A."

                                       "png"),
                           187, 1747, 930.77f, 938.45f);
    platforms.emplace_back(
        LoadTexture("D:"
                    "\\games\\steamlibrary\\steamapps\\common\\Brawlhalla\\mapArt\\Eni"
                    "gma\\Platform_"
                    "Steam1B."
                    "png"),
        1091.5f, 1747, 930.77f, 938.45f);
    collisions.emplace_back(CollisionType::HARD, 200, 1850, 2000, 1850);
    collisions.emplace_back(CollisionType::HARD, 200, 2450, 200, 1850);
    itemSpawns.emplace_back(1086.45f, 1353.95f, true);
    itemSpawns.emplace_back(698.65f, 1570, false);
    itemSpawns.emplace_back(1117.6f, 1570, false);
    itemSpawns.emplace_back(346.35f, 1200, false);
    respawns.emplace_back(480, 1200, false);
    respawns.emplace_back(350, 1550, true);
    respawns.emplace_back(1850, 1550, true);
    respawns.emplace_back(1740, 1200, true);
    weaponColor.inner[0] = 185.0f / 255;
    weaponColor.inner[1] = 70.0f / 255;
    weaponColor.inner[2] = 1.0f;
    weaponColor.outer[0] = 246.0f / 255;
    weaponColor.outer[1] = 175.0f / 255;
    weaponColor.outer[2] = 1;
}

void Map::setBackground(std::string path) {
    Logger::info("Loading texture " + path);
    background = LoadTexture(path.c_str());
}

void Map::draw(const Camera2D& cam) {
    DrawTexturePro(background, Rectangle{0, 0, (float)background.width, (float)background.height},
                   Rectangle{0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
                   Vector2(0, 0), 0, WHITE);

    BeginMode2D(cam);

    for (auto pf : platforms) {
        DrawTexturePro(pf.tex, Rectangle{0, 0, (float)pf.tex.width, (float)pf.tex.height},
                       Rectangle{pf.x, pf.y, pf.w, pf.h}, Vector2(0, 0), 0, WHITE);
    }

    int opacity = 150;
    rlSetLineWidth(15 * cam.zoom);
    for (auto collision : collisions) {
        auto col = ORANGE;
        col.a = opacity;
        if (collision.type == CollisionType::HARD) {
            col = RED;
            col.a = opacity;
        }
        DrawLine(collision.x1, collision.y1, collision.x2, collision.y2, col);
    }

    Rectangle cbounds = {camBounds.x, camBounds.y, camBounds.w, camBounds.h};
    DrawRectangleLinesEx(cbounds, 15, DARKGRAY);

    Rectangle kbounds = {camBounds.x - killBounds.left, camBounds.y - killBounds.top,
                         camBounds.w + killBounds.left + killBounds.right,
                         camBounds.h + killBounds.top + killBounds.bottom};
    DrawRectangleLinesEx(kbounds, 15, LIGHTGRAY);

    for (auto item : itemSpawns) {
        auto col = BLUE;
        col.a = opacity;
        if (item.init) {
            col = DARKBLUE;
            col.a = opacity;
        }
        DrawCircle(item.x, item.y, 50, col);
    }

    for (auto respawn : respawns) {
        auto col = GREEN;
        col.a = opacity;
        if (respawn.init) {
            col = LIME;
            col.a = opacity;
        }
        DrawCircle(respawn.x, respawn.y, 70, col);
    }

    EndMode2D();
}