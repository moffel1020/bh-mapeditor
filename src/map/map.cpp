#include "map.h"
#include "imageresource.h"
#include "logger.h"
#include "mapobject.h"
#include "objecttypes.h"
#include "raylib.h"
#include "rlgl.h"
#include <algorithm>
#include <memory>
#include <string>

void Map::loadTestMap(const std::string& brawlDir) {
    // placeholder test map
    background = std::make_unique<ImageResource>(brawlDir + "/mapArt/Backgrounds/BG_Steam.jpg");

    thumbnail = std::make_unique<ImageResource>(brawlDir + "/images/thumbnails/testmap.jpg");

    camBounds = CameraBounds{-1364, 278.15f, 4928, 2772};
    killBounds = KillBounds{700, 700, 800, 850};

    addObject<Platform>(187, 1747, 930.77f, 938.45f,
                        brawlDir + "/mapArt/Enigma/Platform_Steam1A.png");

    addObject<Platform>(1091.5f, 1747, 930.77f, 938.45f,
                        brawlDir + "/mapArt/Enigma/Platform_Steam1B.png");

    addObject<ItemSpawn>(1086.45f, 1353.95f, true);
    addObject<ItemSpawn>(698.65f, 1570);
    addObject<ItemSpawn>(1117.6f, 1570);
    addObject<ItemSpawn>(346.35f, 1200);
    addObject<Respawn>(480, 1200, true);
    addObject<Respawn>(350, 1550, true);
    addObject<Respawn>(1850, 1550, true);
    addObject<Respawn>(1740, 1200, true);

    weaponColor.inner[0] = 185.0f / 255;
    weaponColor.inner[1] = 70.0f / 255;
    weaponColor.inner[2] = 1.0f;
    weaponColor.outer[0] = 246.0f / 255;
    weaponColor.outer[1] = 175.0f / 255;
    weaponColor.outer[2] = 1;

    addObject<HardCollision>(1000, 1000);
    addObject<SoftCollision>(500, 100);
}

void Map::draw(const Camera2D& cam) {
    drawBackground();
    BeginMode2D(cam);

    Rectangle cbounds = {camBounds.x, camBounds.y, camBounds.w, camBounds.h};
    DrawRectangleLinesEx(cbounds, 15, DARKGRAY);

    Rectangle kbounds = {camBounds.x - killBounds.left, camBounds.y - killBounds.top,
                         camBounds.w + killBounds.left + killBounds.right,
                         camBounds.h + killBounds.top + killBounds.bottom};
    DrawRectangleLinesEx(kbounds, 15, LIGHTGRAY);

    rlSetLineWidth(15 * cam.zoom);
    for (auto& mo : mapObjects) {
        mo->draw();
    }

    EndMode2D();
    DrawFPS(50, 50);
}

bool Map::addObject(MapObjectType type, float x, float y) {
    switch (type) {
    case MapObjectType::Platform:
        mapObjects.emplace_back(std::make_shared<Platform>(x, y));
        break;
    case MapObjectType::Respawn:
        mapObjects.emplace_back(std::make_shared<Respawn>(x, y));
        break;
    case MapObjectType::HardCollision:
        mapObjects.emplace_back(std::make_shared<HardCollision>(x, y));
        break;
    case MapObjectType::SoftCollision:
        mapObjects.emplace_back(std::make_shared<SoftCollision>(x, y));
        break;
    case MapObjectType::ItemSpawn:
        mapObjects.emplace_back(std::make_shared<ItemSpawn>(x, y));
        break;
    default:
        Logger::error("tried to add unknown object type");
        return false;
    }

    return true;
}

bool Map::removeObject(MapObject* object) {
    const auto it =
        std::find_if(mapObjects.begin(), mapObjects.end(),
                     [&](std::shared_ptr<MapObject>& mo) { return mo.get() == object; });

    if (it == mapObjects.end()) {
        return false;
    }

    Logger::info("Removing object " + std::string(object->getTypeName()));
    std::erase(mapObjects, *it);

    return true;
}

void Map::drawBackground() {
    float sWidth = GetScreenWidth();
    float sHeight = GetScreenHeight();

    if (sHeight * background->ratio < sWidth) {
        // if the image is smaller than the width of the screen, scale with the width and move
        // it up to center it
        float hwRatio = 1 / background->ratio;
        float bY = -(sWidth - sHeight * background->ratio) / 2;
        DrawTexturePro(background->tex,
                       Rectangle{0, 0, (float)background->tex.width, (float)background->tex.height},
                       Rectangle{0, bY, sWidth, sWidth * hwRatio}, Vector2{0, 0}, 0, WHITE);
        return;
    }

    // if image width is bigger than the screen, center it by moving it to the left
    float bX = -(sHeight * background->ratio - sWidth) / 2;
    DrawTexturePro(background->tex,
                   Rectangle{0, 0, (float)background->tex.width, (float)background->tex.height},
                   Rectangle{bX, 0, sHeight * background->ratio, sHeight}, Vector2{0, 0}, 0, WHITE);
    return;
}
