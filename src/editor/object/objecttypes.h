#pragma once
#include "imageresource.h"
#include "mapobject.h"
#include <memory>

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

enum class CollisionType {
    Hard,
    Soft,
    Dynamic,
    NoSlide
};

class Collision : public MapObject {
  public:
    Collision(float x1, float y1, float x2, float y2, CollisionType type = CollisionType::Hard);
    Collision(float x, float y, CollisionType type = CollisionType::Hard);
    void draw() const override;
    MAPOBJECT_TYPE(Collision)

    float x1;
    float y1;
    float x2;
    float y2;
    CollisionType collisionType;
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
