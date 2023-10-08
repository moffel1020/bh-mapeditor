#pragma once

enum class MapObjectType {
    Unknown = 0,
    Platform,
    Respawn,
    ItemSpawn,
    Collision,
};

#define MAPOBJECT_TYPE(type)                                                                       \
    static MapObjectType getStaticType() { return MapObjectType::type; }                           \
    virtual MapObjectType getType() const override { return getStaticType(); }                     \
    virtual const char* getTypeName() const override { return #type; }

class MapObject {
  public:
    virtual ~MapObject() = default;
    virtual MapObjectType getType() const = 0;
    virtual const char* getTypeName() const = 0;

    virtual void draw() const = 0;
};