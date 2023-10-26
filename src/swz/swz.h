#pragma once
#include "well512.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

class Swz {
  public:
    Swz(uint32_t key, const std::filesystem::path& swzFile);
    void dumpToDisk(const std::filesystem::path& directory);
    void encryptFiles(const std::filesystem::path& dest);

    uint32_t getKey() { return key; }
    uint32_t getSeed() { return seed; }
    const std::map<std::string, std::string>& getFiles() { return files; }

  private:
    std::vector<std::string> decrypt(std::string path);
    std::vector<uint8_t> encrypt(uint32_t seed);

    std::string readStringEntry(std::ifstream& input, Well512& rand);
    uint32_t readUint32BE(std::ifstream& input);
    uint32_t rotateRight(uint32_t v, int bits);

    void writeUint32BE(std::vector<uint8_t>& buffer, uint32_t value);
    void writeStringEntry(std::string& input, Well512& rand, std::vector<uint8_t>& output);

    std::string generateFileName(const std::string& data);

    uint32_t seed;
    uint32_t key;
    std::string path;
    std::map<std::string, std::string> files;
};