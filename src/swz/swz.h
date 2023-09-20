#pragma once
#include "well512.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

class Swz {
  public:
    explicit Swz(uint32_t key);
    std::vector<std::string> decrypt(std::string path);
    std::vector<uint8_t> encrypt(std::vector<std::string>& stringEntries, uint32_t seed = 0);

    void writeFiles(const std::vector<std::string>& files, std::filesystem::path folder);

    void writeSwz(const std::vector<uint8_t>& bytes, const std::filesystem::path filePath);

    uint32_t getKey() { return m_Key; }

  private:
    std::string readStringEntry(std::ifstream& input, Well512& rand);
    uint32_t readUint32BE(std::ifstream& input);
    uint32_t rotateRight(uint32_t v, int bits);

    void writeUint32BE(std::vector<uint8_t>& buffer, uint32_t value);
    void writeStringEntry(std::string& input, Well512& rand, std::vector<uint8_t>& output);

    std::string generateFileName(const std::string& data);

    uint32_t m_Key;
};