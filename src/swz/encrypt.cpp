#include "swz.h"
#include <miniz.h>
#include <vector>

std::vector<uint8_t> Swz::encrypt(uint32_t seed) {
    Well512 rand(seed ^ key);

    uint32_t hash = 0x2DF4A1CDu;
    int hash_rounds = key % 0x1F + 5;

    for (int i = 0; i < hash_rounds; i++) {
        hash ^= rand.nextUint();
    }

    std::vector<uint8_t> buffer;
    writeUint32BE(buffer, hash);
    writeUint32BE(buffer, seed);

    for (auto& str : files) {
        writeStringEntry(str.second, rand, buffer);
    }

    return buffer;
}

void Swz::writeUint32BE(std::vector<uint8_t>& buffer, uint32_t value) {
    buffer.emplace_back((uint8_t)((value >> 24) & 0xFF));
    buffer.emplace_back((uint8_t)((value >> 16) & 0xFF));
    buffer.emplace_back((uint8_t)((value >> 8) & 0xFF));
    buffer.emplace_back((uint8_t)((value >> 0) & 0xFF));
}

void Swz::writeStringEntry(std::string& input, Well512& rand, std::vector<uint8_t>& output) {

    auto outSize = compressBound(input.size());
    Bytef* compressedInput = new Bytef[outSize];
    Bytef* source = new Bytef[input.size()];
    input.copy((char*)source, input.size());

    int status = compress2(compressedInput, &outSize, source, input.size(), Z_BEST_COMPRESSION);

    delete[] source;

    if (status != Z_OK) {
        std::cout << "compression failed with status: " << status << std::endl;
        delete[] compressedInput;
        return;
    }

    uint32_t compressedSize = (uint32_t)outSize ^ rand.nextUint();
    uint32_t decompressedSize = (uint32_t)input.size() ^ rand.nextUint();

    uint32_t checksum = rand.nextUint();

    for (uint32_t i = 0; i < outSize; i++) {
        checksum = compressedInput[i] ^ rotateRight(checksum, (i % 7) + 1);

        int shift = i & 0xF;
        compressedInput[i] ^= (uint8_t)(((0xFFu << shift) & rand.nextUint()) >> shift);
    }

    writeUint32BE(output, compressedSize);
    writeUint32BE(output, decompressedSize);
    writeUint32BE(output, checksum);

    for (uint32_t i = 0; i < outSize; i++) {
        output.emplace_back(compressedInput[i]);
    }

    delete[] compressedInput;
}