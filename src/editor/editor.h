#pragma once
#include "map.h"
#include "raylib.h"

class Editor {
  public:
    void start();
    void run();
    ~Editor();

  private:
    void findBrawlhallaDir();

    std::unique_ptr<Map> map;
    Camera2D cam;
};