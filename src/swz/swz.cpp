#include "swz.h"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdint.h>
#include <string>
#include <vector>

Swz::Swz(uint32_t key, const std::filesystem::path& swzFile) : key(key), path(swzFile.string()) {
    if (!std::filesystem::exists(swzFile)) {
        std::cout << "file " << swzFile.string() << " does not exist" << std::endl;
        return;
    }

    auto contents = decrypt(swzFile.string());
    for (const auto& c : contents) {
        files[generateFileName(c)] = c;
    }
}

void Swz::dumpToDisk(const std::filesystem::path& directory) {
    std::filesystem::create_directories(directory);
    for (const auto& c : files) {
        std::ofstream file(directory.string() + "/" + c.first);
        if (!file.is_open()) {
            std::cout << "could not create file: " << c.first << std::endl;
            continue;
        }

        file << c.second;
        file.close();
    }
}

void Swz::encryptFiles(const std::filesystem::path& dest) {
    std::filesystem::create_directories(dest.parent_path());
    std::vector<uint8_t> encryptedData = encrypt(seed);

    std::ofstream swzFile(dest, std::ios::binary);
    if (!swzFile.is_open()) {
        std::cout << "could not open file: " << dest << std::endl;
        return;
    }

    for (const char b : encryptedData) {
        swzFile << b;
    }

    swzFile.close();
}

std::string Swz::generateFileName(const std::string& data) {
    std::string fileName;
    if (data.starts_with("<LevelDesc")) {
        std::string temp =
            data.substr(data.find("LevelName=") + std::string("LevelName=\"").size());
        fileName = temp.substr(0, temp.find('"')) + ".xml";
    } else if (data.starts_with('<')) {
        fileName = data.substr(1, data.find(">") - 1) + ".xml";
    } else {
        fileName = data.substr(0, data.find("\n") - 1) + ".csv";
    }

    return fileName;
}