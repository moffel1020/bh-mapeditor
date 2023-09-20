#include "swz.h"
#include <stdint.h>
#include <string>
#include <zlib.h>

std::vector<std::string> Swz::decrypt(std::string path) {
    std::ifstream input(path, std::ios::binary);
    if (!input.is_open()) {
        std::cout << "could not open swz file: " << path << std::endl;
    }
    uint32_t checksum = readUint32BE(input);
    uint32_t seed = readUint32BE(input);

    auto rand = Well512(seed ^ m_Key);

    uint32_t hash = 0x2DF4A1CDu;
    uint32_t hash_rounds = m_Key % 0x1F + 5;

    for (uint32_t i = 0; i < hash_rounds; i++) {
        hash ^= rand.nextUint();
    }

    if (hash != checksum) {
        std::cout << "hash not equal to checksum" << std::endl;
    }

    std::vector<std::string> results;
    while (input && input.peek() != EOF) {
        results.emplace_back(readStringEntry(input, rand));
    }

    return results;
}

uint32_t Swz::readUint32BE(std::ifstream& input) {
    uint8_t buffer[4];
    input.read((char*)buffer, sizeof(uint8_t) * 4);
    return ((uint32_t)buffer[3] | ((uint32_t)buffer[2] << 8) | ((uint32_t)buffer[1] << 16) |
            ((uint32_t)buffer[0] << 24));
}

std::string Swz::readStringEntry(std::ifstream& input, Well512& rand) {
    uint32_t compressedSize = readUint32BE(input) ^ rand.nextUint();
    uint32_t decompressedSize = readUint32BE(input) ^ rand.nextUint();
    uint32_t checksum = readUint32BE(input);

    uint8_t* buffer = new uint8_t[compressedSize];
    input.read((char*)buffer, sizeof(uint8_t) * compressedSize);

    uint32_t hash = rand.nextUint();

    for (uint32_t i = 0; i < compressedSize; i++) {
        int shift = i & 0xF;
        buffer[i] ^= (uint8_t)(((0xFFu << shift) & rand.nextUint()) >> shift);

        hash = buffer[i] ^ rotateRight(hash, (i % 7) + 1);
    }

    if (hash != checksum) {
        std::cout << "hash not equal to checksum 2" << std::endl;
    }

    uint8_t out[decompressedSize];

    uLong outSize = decompressedSize;
    int a = uncompress((Bytef*)out, &outSize, (Bytef*)buffer, (uLong)compressedSize);

    if (a != Z_OK) {
        std::cout << "could not uncompress file, error: " << a << std::endl;
    }

    delete[] buffer;
    return std::string((const char*)out, decompressedSize);
}

uint32_t Swz::rotateRight(uint32_t v, int bits) { return (v >> bits) | (v << (32 - bits)); }
