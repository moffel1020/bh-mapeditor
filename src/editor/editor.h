#pragma once
#include "map.h"
#include "raylib.h"

class Editor {
  public:
    Editor();
    void start();
    void run();
    ~Editor();
    static Editor& get() { return *instance; }
    std::string getBrawlDir() { return brawlDir; }
    const Camera2D& getCam() { return cam; }

  private:
    void gui();
    void findBrawlDir();
    bool isValidBrawlDir(const std::filesystem::path& dir) const;
    static Editor* instance;

    std::string brawlDir;
    std::unique_ptr<Map> map;
    Camera2D cam = {};
};