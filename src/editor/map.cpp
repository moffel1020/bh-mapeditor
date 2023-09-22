#include "map.h"
#include "logger.h"
#include "raylib.h"
#include "rlgl.h"

std::string collisionTypeToString(CollisionType type) {
    std::string str;
    switch (type) {
    case CollisionType::HARD:
        str = "hard";
        break;
    case CollisionType::SOFT:
        str = "soft";
        break;
    case CollisionType::DYNAMIC:
        str = "dynamic";
        break;
    case CollisionType::NOSLIDE:
        str = "no slide";
        break;
    default:
        str = "unknown";
        break;
    }

    return str;
}

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

    rlSetLineWidth(10 * cam.zoom);
    for (auto col : collisions) {
        DrawLine(col.x1, col.y1, col.x2, col.y2, RED);
    }

    Rectangle cbounds = {camBounds.x, camBounds.y, camBounds.w, camBounds.h};
    DrawRectangleLinesEx(cbounds, 15, Color{0, 0, 0, 150});

    Rectangle kbounds = {camBounds.x - killBounds.left, camBounds.y - killBounds.top,
                         camBounds.w + killBounds.left + killBounds.right,
                         camBounds.h + killBounds.top + killBounds.bottom};
    DrawRectangleLinesEx(kbounds, 15, Color{255, 161, 0, 150});

    for (auto item : itemSpawns) {
        Color col{102, 191, 255, 150};
        if (item.init) {
            col = {0, 121, 241, 150};
        }
        DrawCircle(item.x, item.y, 50, col);
    }

    for (auto respawn : respawns) {
        Color col{253, 249, 9, 159};
        if (respawn.init) {
            col = {255, 161, 0, 150};
        }
        DrawCircle(respawn.x, respawn.y, 50, col);
    }

    DrawCircle(0, 0, 50, WHITE);

    EndMode2D();
}