#pragma once
#include "imageresource.h"
#include "mapobject.h"
#include "raylib.h"
#include <memory>
#include <vector>

class Respawn : public MapObject {
  public:
    Respawn(float x, float y, bool init = false);
    void draw() const override;
    MAPOBJECT_TYPE(Respawn)

    float x;
    float y;
    bool init;
};

class ItemSpawn : public MapObject {
  public:
    ItemSpawn(float x, float y, bool init = false);
    void draw() const override;
    MAPOBJECT_TYPE(ItemSpawn)

    float x;
    float y;
    bool init;
};

class Platform : public MapObject {
  public:
    Platform() = delete;
    Platform(float x, float y, float w = 300, float h = 100, const std::string& path = "");
    void draw() const override;
    MAPOBJECT_TYPE(Platform)

    float x;
    float y;
    float w;
    float h;
    std::unique_ptr<ImageResource> img;
};

class HardCollision : public MapObject {
  public:
    HardCollision(float x, float y);
    void addPoint();
    void removePoint();
    void draw() const override;
    MAPOBJECT_TYPE(HardCollision)

    std::vector<Vector2> points;
};

class SoftCollision : public MapObject {
  public:
    SoftCollision(float x1, float y1, float x2, float y2);
    SoftCollision(float x, float y);
    void draw() const override;
    MAPOBJECT_TYPE(SoftCollision)

    float x1;
    float y1;
    float x2;
    float y2;
};