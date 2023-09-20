#include "swz.h"
#include <filesystem>
#include <stdint.h>
#include <vector>

Swz::Swz(uint32_t key) { m_Key = key; }

void Swz::writeSwz(const std::vector<uint8_t>& bytes, const std::filesystem::path filePath) {

    std::filesystem::create_directories(filePath.parent_path());
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "could not open file: " << filePath << std::endl;
        return;
    }

    for (char b : bytes) {
        file << b;
    }
    file.close();
}

void Swz::writeFiles(const std::vector<std::string>& files, std::filesystem::path folder) {

    std::filesystem::create_directories(folder);
    folder.append("temp.xml");

    for (auto data : files) {
        auto name = generateFileName(data);
        std::ofstream file(folder.replace_filename(name), std::ios::binary);
        file << data;
        file.close();
    }
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