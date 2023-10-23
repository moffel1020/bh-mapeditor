#pragma once
#include "map.h"
#include "mapobject.h"
#include "raylib.h"
#include <memory>

class Editor {
  public:
    Editor();
    void start();
    void run();
    ~Editor();
    static Editor& get() { return *instance; }
    const std::string& getBrawlDir() const { return brawlDir; }
    const Camera2D& getCam() const { return cam; }
    std::weak_ptr<MapObject> getSelectedobject() const { return selectedObject; };
    void setSelectedObject(std::weak_ptr<MapObject> object) { selectedObject = object; }

  private:
    void gui();
    void findBrawlDir();
    bool isValidBrawlDir(const std::filesystem::path& dir) const;
    void loadMousePickingFramebuffer();
    std::weak_ptr<MapObject> getObjectAtCoords(int screenX, int screenY);

    std::string brawlDir;
    std::shared_ptr<Map> map;
    Camera2D cam = {};

    std::weak_ptr<MapObject> selectedObject;

    static Editor* instance;

    struct {
        RenderTexture2D framebuffer;
        Shader shader;
    } objectIdFBO;
};
