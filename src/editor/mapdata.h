#pragma once
#include "raylib.h"
#include <filesystem>
#include <string>

class ImageResource {
  public:
    ImageResource(const std::string& path);
    void setImage(const std::string& path);
    std::string getFilename() const { return path.filename().string(); }
    ~ImageResource();

    std::filesystem::path path;
    Texture tex = {};
    float ratio = 1; // width / height
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

struct WeaponColor {
    // rgb, 0 to 1
    float inner[3];
    float outer[3];
};
