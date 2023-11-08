#include "serializer.h"
#include "map.h"
#include "mapobject.h"
#include "objecttypes.h"
#include "swz.h"
#include "tinyxml2.h"
#include <filesystem>
#include <map>
#include <memory>
#include <string>

std::string colToHexString(float* color) {
    const uint16_t r = color[0] * 255;
    const uint16_t g = color[1] * 255;
    const uint16_t b = color[2] * 255;
    char hexcol[16] = {};
    snprintf(hexcol, sizeof(hexcol), "%02X%02X%02X", r, g, b);

    return "0x" + std::string(hexcol);
}

MapSerializer::MapSerializer(std::shared_ptr<Map> map) : map(map) {}

void MapSerializer::serialize(const std::string& brawlDir, bool moveToBrawlDir) {
    uint32_t key = 644158000;
    Swz dynamic(key, brawlDir + "/Dynamic.swz");
    Swz game(key, brawlDir + "/Game.swz");
    Swz init(key, brawlDir + "/Init.swz");

    addToPlaylists(game.getFiles());
    addLevelEntry(init.getFiles());
    addLevelXml(dynamic.getFiles());

    if (moveToBrawlDir) {
        dynamic.encryptFiles(brawlDir + "/Dynamic.swz");
        game.encryptFiles(brawlDir + "/Game.swz");
        init.encryptFiles(brawlDir + "/Init.swz");
        moveImages(brawlDir);
    } else {
        dynamic.dumpToDisk("dump/dynamic");
        game.dumpToDisk("dump/game");
        init.dumpToDisk("dump/init");

        dynamic.encryptFiles("encrypt/Dynamic.swz");
        game.encryptFiles("encrypt/Game.swz");
        init.encryptFiles("encrypt/Init.swz");
    }
}

// create xml string for dynamic.swz
std::string MapSerializer::createLevelXml() {
    tinyxml2::XMLPrinter p(0, false);
    p.OpenElement("LevelDesc");
    p.PushAttribute("AssetDir", map->name.c_str());
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

void MapSerializer::addLevelXml(std::map<std::string, std::string>& files) {
    files[map->name + ".xml"] = createLevelXml();
}

void MapSerializer::addToPlaylists(std::map<std::string, std::string>& files) {
    tinyxml2::XMLDocument doc;
    std::string file = files["LevelSetTypes.xml"];
    if (doc.Parse(file.c_str()) != tinyxml2::XML_SUCCESS) {
        Logger::error("could not parse LevelSetTypes.xml file");
        return;
    }

    auto root = doc.FirstChildElement("LevelSetTypes");
    for (auto* e = root->FirstChildElement("LevelSetType"); e != nullptr;
         e = e->NextSiblingElement("LevelSetType")) {

        if (std::string(e->Attribute("LevelSetName")) == std::string("Auto", 4)) {
            continue;
        }

        auto lt = e->FirstChildElement("LevelTypes");
        std::string text = lt->GetText();
        if (text.find(map->name) == text.npos) {
            lt->SetText((map->name + "," + text).c_str());
        }
    }

    tinyxml2::XMLPrinter printer;
    doc.Print(&printer);
    files["LevelSetTypes.xml"] = printer.CStr();
}

void MapSerializer::addLevelEntry(std::map<std::string, std::string>& files) {
    tinyxml2::XMLDocument doc;
    auto file = files["LevelTypes.xml"];
    if (doc.Parse(file.c_str()) != tinyxml2::XML_SUCCESS) {
        Logger::error("could not parse LevelTypes.xml file");
        return;
    }

    auto root = doc.FirstChildElement("LevelTypes");

    auto last = root->LastChildElement("LevelType");
    int highestId = last->FirstChildElement("LevelID")->IntText();
    auto newElem = root->InsertNewChildElement("LevelType");
    newElem->SetAttribute("LevelName", map->name.c_str());
    newElem->SetAttribute("DevOnly", "false");
    newElem->SetAttribute("TestLevel", "false");
    newElem->InsertNewChildElement("DisplayName")->InsertNewText(map->name.c_str());
    newElem->InsertNewChildElement("LevelID")->InsertNewText(std::to_string(highestId + 1).c_str());
    newElem->InsertNewChildElement("FileName")->InsertNewText("Level_Ruins.swf");
    newElem->InsertNewChildElement("AssetName")->InsertNewText("a_Level_SmallEnigma");
    newElem->InsertNewChildElement("CrateColorA")
        ->InsertNewText(colToHexString(map->weaponColor.outer).c_str());
    newElem->InsertNewChildElement("CrateColorB")
        ->InsertNewText(colToHexString(map->weaponColor.inner).c_str());
    newElem->InsertNewChildElement("LeftKill")
        ->InsertNewText(std::to_string(map->killBounds.left).c_str());
    newElem->InsertNewChildElement("RightKill")
        ->InsertNewText(std::to_string(map->killBounds.right).c_str());
    newElem->InsertNewChildElement("TopKill")->InsertNewText(
        std::to_string(map->killBounds.top).c_str());
    newElem->InsertNewChildElement("BottomKill")
        ->InsertNewText(std::to_string(map->killBounds.bottom).c_str());
    newElem->InsertNewChildElement("BGMusic")->InsertNewText("Level06Theme");
    newElem->InsertNewChildElement("ThumbnailPNGFile")
        ->InsertNewText(map->thumbnail->getFilename().c_str());
    newElem->InsertNewChildElement("BotTint")->InsertNewText("0");
    newElem->InsertNewChildElement("BotOffset")->InsertNewText("0xEDBEB0");
    newElem->InsertNewChildElement("BotFraction")->InsertNewText("0.55");
    newElem->InsertNewChildElement("ColorExclusionList")
        ->InsertNewText("StPaddy"); // this color sucks balls

    tinyxml2::XMLPrinter printer;
    printer.SetShortCloseMode(false); // brawlhalla doesnt like this sometimes
    doc.Print(&printer);
    files["LevelTypes.xml"] = printer.CStr();
}

void copyFile(std::filesystem::path src, std::filesystem::path dst) {
    // c++ gets mad if the paths are the same
    if (!std::filesystem::equivalent(src, dst)) {
        std::filesystem::copy(src, dst, std::filesystem::copy_options::overwrite_existing);
    }
}

void MapSerializer::moveImages(const std::string& brawlDir) {
    copyFile(map->background->path,
             brawlDir + "/mapArt/Backgrounds/" + map->background->getFilename());
    copyFile(map->thumbnail->path,
             brawlDir + "/images/thumbnails/" + map->thumbnail->getFilename());

    std::filesystem::create_directories(brawlDir + "/mapArt/" + map->name);
    for (const auto& mo : map->mapObjects) {
        if (mo->getType() != MapObjectType::Platform) {
            continue;
        }

        auto plat = static_cast<Platform*>(mo.get());
        copyFile(plat->img->path,
                 brawlDir + "/mapArt/" + map->name + "/" + plat->img->getFilename());
    }
}