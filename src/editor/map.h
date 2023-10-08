#pragma once
#include "logger.h"
#include "mapdata.h"
#include "mapobject.h"
#include "raylib.h"
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

class Map {
  public:
    void draw(const Camera2D& cam);
    void loadTestMap(const std::string& brawlDir);
    bool removeObject(MapObject* object);
    bool addObject(MapObjectType type, float x = 0, float y = 0);
    template <typename T, typename... Args> void addObject(Args&&... args) {
        mapObjects.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
    }

    std::string name = "CustomMap";
    WeaponColor weaponColor;
    std::unique_ptr<ImageResource> background;
    std::unique_ptr<ImageResource> thumbnail;
    CameraBounds camBounds;
    KillBounds killBounds;

    std::vector<std::unique_ptr<MapObject>> mapObjects;

  private:
    void drawBackground();
};
