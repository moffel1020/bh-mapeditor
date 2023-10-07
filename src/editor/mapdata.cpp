#include "mapdata.h"
#include "logger.h"
#include "raylib.h"
#include <filesystem>
#include <string>

ImageResource::ImageResource(const std::string& path) { setImage(path); }

void ImageResource::setImage(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        Logger::error("could not load " + path + " it doesnt exist");
        return;
    }

    Logger::info("Loading image resource " + path);
    if (tex.id && std::filesystem::exists(this->path)) {
        Logger::info("Unloading image resource " + this->path.string());
        UnloadTexture(tex);
    }
    this->path = path;
    this->tex = LoadTexture(path.c_str());
    this->ratio = (float)tex.width / (float)tex.height;
}

ImageResource::~ImageResource() {
    if (tex.id) {
        UnloadTexture(tex);
    }
}