#include "objecttypes.h"
#include "editor.h"
#include "imageresource.h"
#include "raylib.h"
#include <memory>

const static int opacity = 150;

Respawn::Respawn(float x, float y, bool init) : x(x), y(y), init(init) {}

void Respawn::draw() const {
    auto col = GREEN;
    col.a = opacity;
    if (init) {
        col = LIME;
        col.a = opacity;
    }
    DrawCircle(x, y, 70, col);
}

ItemSpawn::ItemSpawn(float x, float y, bool init) : x(x), y(y), init(init) {}

void ItemSpawn::draw() const {
    auto col = BLUE;
    col.a = opacity;
    if (init) {
        col = DARKBLUE;
        col.a = opacity;
    }
    DrawCircle(x, y, 50, col);
}

Platform::Platform(float x, float y, float w, float h, const std::string& path)
    : x(x), y(y), w(w), h(h) {

    if (path != "") {
        img = std::make_unique<ImageResource>(path);
        return;
    }

    img = std::make_unique<ImageResource>(Editor::get().getBrawlDir() +
                                          "/images/thumbnails/CorruptFile.png");
}

void Platform::draw() const {
    if (!IsTextureReady(img->tex)) {
        return;
    }

    DrawTexturePro(img->tex, Rectangle{0, 0, (float)img->tex.width, (float)img->tex.height},
                   Rectangle{x, y, w, h}, Vector2{0, 0}, 0, WHITE);
}

SoftCollision::SoftCollision(float x1, float y1, float x2, float y2)
    : x1(x1), y1(y1), x2(x2), y2(y2) {}

SoftCollision::SoftCollision(float x, float y) : x1(x), y1(y), x2(x + 300), y2(y) {}

void SoftCollision::draw() const {
    auto col = ORANGE;
    col.a = opacity;
    DrawLine(x1, y1, x2, y2, col);
}

HardCollision::HardCollision(float x, float y) {
    points.emplace_back(x - 150, y - 150);
    points.emplace_back(x - 150, y + 150);
    points.emplace_back(x + 150, y + 150);
    points.emplace_back(x + 150, y - 150);
}

void HardCollision::draw() const {
    auto col = RED;
    col.a = opacity;
    for (size_t i = 0; i < points.size(); i++) {
        if (i == points.size() - 1) {
            DrawLineV(points[i], points[0], col);
            break;
        }
        DrawLineV(points[i], points[i + 1], col);
    }
}

void HardCollision::addPoint() {
    Vector2 coords{
        .x = (points.front().x + points.back().x) / 2,
        .y = (points.front().y + points.back().y) / 2 + 20,
    };
    points.emplace_back(coords);
}

void HardCollision::removePoint() {
    if (points.size() > 3) {
        points.erase(points.end());
    }
}