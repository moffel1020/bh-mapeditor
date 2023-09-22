#include "editor.h"
#include "logger.h"
#include "raylib.h"
#include <memory>

int main() {
    SetTraceLogCallback(Logger::rlLog);

    std::unique_ptr<Editor> editor = std::make_unique<Editor>();
    editor->start();
    editor->run();

    // uint32_t key = 887927120;

    // Swz swz(key);
    // auto files = swz.decrypt("Dynamic.swz");
    // swz.writeFiles(files, std::filesystem::current_path().append("dump"));

    // auto encryptedBytes = swz.encrypt(files);
    // swz.writeSwz(encryptedBytes,
    //              std::filesystem::current_path().append("encrypt/test.swz"));
}