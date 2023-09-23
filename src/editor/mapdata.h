#pragma once
#include "raylib.h"
#include <filesystem>
#include <string>

class ImageResource {
  public:
    std::filesystem::path path;
    Texture tex;
    ImageResource(const std::string& path);
    void setImage(const std::string& path);
    std::string getFilename() const { return path.filename().string(); }
    ImageResource() {}
};

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

struct Platform {
    float x;
    float y;
    float h;
    float w;
    ImageResource img;
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
    bool init;
};

struct WeaponColor {
    // rgb, 0 to 1
    float inner[3];
    float outer[3];
};