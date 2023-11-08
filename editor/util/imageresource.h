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