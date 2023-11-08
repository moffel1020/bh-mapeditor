#include "editor.h"
#include "logger.h"
#include "raylib.h"
#include <memory>

int main() {
    SetTraceLogCallback(Logger::rlLog);

    std::unique_ptr<Editor> editor = std::make_unique<Editor>();
    editor->start();
    editor->run();
}