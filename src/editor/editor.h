#pragma once
#include "map.h"
#include "raylib.h"

class Editor {
  public:
    void start();
    void run();
    ~Editor();

  private:
    void gui();
    void findBrawlDir();
    std::string selectImageFile() const;
    bool isValidBrawlDir(const std::filesystem::path& dir) const;

    std::string brawlDir;
    std::unique_ptr<Map> map;
    Camera2D cam;
};