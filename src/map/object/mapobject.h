#pragma once

enum class MapObjectType {
    Unknown = 0,
    Platform,
    Respawn,
    ItemSpawn,
    HardCollision,
    SoftCollision,
};

#define MAPOBJECT_TYPE(type)                                                                       \
    static MapObjectType getStaticType() { return MapObjectType::type; }                           \
    MapObjectType getType() const override { return getStaticType(); }                             \
    const char* getTypeName() const override { return #type; }

class MapObject {
  public:
    virtual ~MapObject() = default;
    virtual MapObjectType getType() const = 0;
    virtual const char* getTypeName() const = 0;

    virtual void draw() const = 0;
};