#pragma once
#include "map.h"
#include <memory>

class MapSerializer {
  public:
    static std::string createLevelXml(std::shared_ptr<Map> map);
};