#pragma once
#include "mapdata.h"
#include "raylib.h"
#include <filesystem>
#include <string>
#include <vector>

class Map {
  public:
    Map();
    void draw(const Camera2D& cam);

    std::string name = "CustomMap";
    WeaponColor weaponColor;
    ImageResource background;
    CameraBounds camBounds;
    KillBounds killBounds;

    std::vector<ItemSpawn> itemSpawns;
    std::vector<Respawn> respawns;
    std::vector<Platform> platforms;
    std::vector<Collision> collisions;
};