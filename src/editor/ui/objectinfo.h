#pragma once
#include "map.h"
#include "mapobject.h"
#include <memory>

void showObjectInfoWindow(std::weak_ptr<MapObject> object, Map* map, bool* open);
