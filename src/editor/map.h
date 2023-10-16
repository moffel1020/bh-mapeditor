#pragma once
#include "imageresource.h"
#include "logger.h"
#include "mapobject.h"
#include "raylib.h"
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

struct CameraBounds {
    float x;
    float y;
    float w;
    float h;
};

// relative to camerabounds
struct KillBounds {
    int left;
    int right;
    int top;
    int bottom;
};

struct WeaponColor {
    // rgb, 0 to 1
    float inner[3];
    float outer[3];
};

class Map {
  public:
    void draw(const Camera2D& cam);
    void loadTestMap(const std::string& brawlDir);
    bool removeObject(MapObject* object);
    bool addObject(MapObjectType type, float x = 0, float y = 0);
    template <typename T, typename... Args> void addObject(Args&&... args) {
        mapObjects.emplace_back(std::make_shared<T>(std::forward<Args>(args)...));
    }

    std::string name = "CustomMap";
    WeaponColor weaponColor;
    std::unique_ptr<ImageResource> background;
    std::unique_ptr<ImageResource> thumbnail;
    CameraBounds camBounds;
    KillBounds killBounds;

    std::vector<std::shared_ptr<MapObject>> mapObjects;

  private:
    void drawBackground();
};
