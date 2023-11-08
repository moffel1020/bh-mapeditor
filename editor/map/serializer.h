#pragma once
#include "map.h"
#include <map>
#include <memory>
#include <string>

class MapSerializer {
  public:
    MapSerializer(std::shared_ptr<Map> map);
    void serialize(const std::string& brawlDir, bool moveToBrawlDir = false);

  private:
    // for file in dynamic.swz
    std::string createLevelXml();
    void addLevelXml(std::map<std::string, std::string>& files);
    // add to LevelSetTypes.xml
    void addToPlaylists(std::map<std::string, std::string>& files);
    // add to LevelTypes.xml
    void addLevelEntry(std::map<std::string, std::string>& files);

    void moveImages(const std::string& brawlDir);

    std::shared_ptr<Map> map;
};