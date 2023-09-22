#pragma once
#include "raylib.h"
#include <filesystem>
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
    float left;
    float right;
    float top;
    float bottom;
};

struct Platform {
    Texture tex;
    float x;
    float y;
    float h;
    float w;
};

enum class CollisionType {
    HARD,
    SOFT,
    DYNAMIC,
    NOSLIDE
};

struct Collision {
    CollisionType type;
    float x1;
    float y1;
    float x2;
    float y2;
};

struct Respawn {
    float x;
    float y;
    bool init;
};

struct ItemSpawn {
    float x;
    float y;
    float w; // idk what w and h are for
    float h;
};

struct InitItemSpawn {
    float x;
    float y;
};

class Map {
  public:
    Map();
    void setBackground(std::string path);
    void draw(const Camera2D& cam);

  private:
    std::string name = "Custom map";
    Texture background;
    CameraBounds camBounds;
    KillBounds killBounds;
    InitItemSpawn initItemSpawn;
    std::vector<ItemSpawn> itemSpawns;
    std::vector<Respawn> respawns;
    std::vector<Platform> platforms;
    std::vector<Collision> collisions;
};