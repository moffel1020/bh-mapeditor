#include "serializer.h"
#include "mapobject.h"
#include "objecttypes.h"
#include "tinyxml2.h"

// create xml string for dynamic.swz
std::string MapSerializer::createLevelXml(std::shared_ptr<Map> map) {
    tinyxml2::XMLPrinter p(0, false);
    p.OpenElement("LevelDesc");
    p.PushAttribute("AssetDir", "Custom");
    p.PushAttribute("LevelName", map->name.c_str());

    p.OpenElement("CameraBounds");
    p.PushAttribute("H", map->camBounds.h);
    p.PushAttribute("W", map->camBounds.w);
    p.PushAttribute("X", map->camBounds.x);
    p.PushAttribute("Y", map->camBounds.y);
    p.CloseElement();

    p.OpenElement("SpawnBotBounds");
    // placeholder values until bot stuff is implemented
    p.PushAttribute("H", map->camBounds.h);
    p.PushAttribute("W", map->camBounds.w);
    p.PushAttribute("X", map->camBounds.x);
    p.PushAttribute("Y", map->camBounds.y);
    p.CloseElement();

    p.OpenElement("Background");
    p.PushAttribute("AssetName", map->background->getFilename().c_str());
    p.PushAttribute("H", map->background->tex.height);
    p.PushAttribute("W", map->background->tex.width);
    p.CloseElement();

    for (auto& mo : map->mapObjects) {
        switch (mo->getType()) {
        case MapObjectType::Platform: {
            auto plat = static_cast<Platform*>(mo.get());
            p.OpenElement("Platform");
            p.PushAttribute("InstanceName", "dontcare");
            p.PushAttribute("AssetName", plat->img->getFilename().c_str());
            p.PushAttribute("H", plat->h);
            p.PushAttribute("W", plat->w);
            p.PushAttribute("X", plat->x);
            p.PushAttribute("Y", plat->y);
            p.CloseElement();
            break;
        }
        case MapObjectType::Respawn: {
            auto respawn = static_cast<Respawn*>(mo.get());
            p.OpenElement("Respawn");
            if (respawn->init) {
                p.PushAttribute("Initial", "true");
            }
            p.PushAttribute("X", respawn->x);
            p.PushAttribute("Y", respawn->y);
            p.CloseElement();
            break;
        }
        case MapObjectType::ItemSpawn: {
            auto itemspawn = static_cast<ItemSpawn*>(mo.get());
            if (itemspawn->init) {
                p.OpenElement("ItemInitSpawn");
                p.PushAttribute("X", itemspawn->x);
                p.PushAttribute("Y", itemspawn->y);
                p.CloseElement();
                break;
            }
            p.OpenElement("ItemSpawn");
            p.PushAttribute("W", "256.1"); // idk what w is used for
            p.PushAttribute("X", itemspawn->x);
            p.PushAttribute("Y", itemspawn->y);
            p.CloseElement();
            break;
        }
        case MapObjectType::SoftCollision: {
            auto col = static_cast<SoftCollision*>(mo.get());
            p.OpenElement("SoftCollision");
            if (col->x1 == col->x2) {
                p.PushAttribute("X", col->x1);
            } else {
                p.PushAttribute("X1", col->x1);
                p.PushAttribute("X2", col->x2);
            }

            if (col->y1 == col->y2) {
                p.PushAttribute("Y", col->y1);
            } else {
                p.PushAttribute("Y1", col->y1);
                p.PushAttribute("Y2", col->y2);
            }
            p.CloseElement();
            break;
        }
        case MapObjectType::HardCollision: {
            auto col = static_cast<HardCollision*>(mo.get());
            for (size_t i = 0; i < col->points.size(); i++) {
                const auto& point = col->points[i];
                const auto& nextPoint =
                    i == col->points.size() - 1 ? col->points[0] : col->points[i + 1];

                p.OpenElement("HardCollision");
                if (point.x == nextPoint.x) {
                    p.PushAttribute("X", point.x);
                } else {
                    p.PushAttribute("X1", point.x);
                    p.PushAttribute("X2", nextPoint.x);
                }

                if (point.y == nextPoint.y) {
                    p.PushAttribute("Y", point.y);
                } else {
                    p.PushAttribute("Y1", point.y);
                    p.PushAttribute("Y2", nextPoint.y);
                }
                p.CloseElement();
            }
            break;
        }
        default:
            Logger::warn("tried to serialize unknown mapobject type");
            break;
        }
    }

    // placeholder navnodes
    const char* str = R"""(
    <NavNode NavID="W1" Path="W2" X="193" Y="2416" />
    <NavNode NavID="W2" Path="D1,W3" X="177" Y="2196" />
    <NavNode NavID="W3" Path="L4,D2" X="179" Y="1945" />
    <NavNode NavID="L4" Path="D3,12,24,25" X="259" Y="1804" />
    <NavNode NavID="11" Path="12,17,A25,A30,A26,A28" X="1107" Y="1799" />
    <NavNode NavID="12" Path="4,11,A25,A24" X="694" Y="1791" />
    <NavNode NavID="17" Path="11,18,A30,A31" X="1587" Y="1797" />
    <NavNode NavID="L18" Path="D19,A30,17,A31" X="1936" Y="1811" />
    <NavNode NavID="W19" Path="D20,L18" X="2010" Y="1944" />
    <NavNode NavID="W20" Path="W19,D21" X="2009" Y="2195" />
    <NavNode NavID="W21" Path="W20" X="2010" Y="2415" />
    <NavNode NavID="24" Path="25,D12,A26,D4" X="275" Y="1443" />
    <NavNode NavID="25" Path="24,A26,D12,D4,D11,A30" X="678" Y="1436" />
    <NavNode NavID="26" Path="24,28,D11,D25" X="857" Y="1088" />
    <NavNode NavID="28" Path="26,D11,D30,31" X="1298" Y="1096" />
    <NavNode NavID="30" Path="31,D17,D18,28,A25,D11" X="1509" Y="1437" />
    <NavNode NavID="31" Path="D18,28,30,D17" X="1921" Y="1440" />)""";

    p.PushRaw(str);
    p.CloseElement();
    return std::string(p.CStr());
}