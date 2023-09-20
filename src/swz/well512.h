#pragma once
#include <cstdint>
#include <vector>

class Well512 {
  public:
    Well512(uint32_t seed);
    void setSeed(uint32_t seed);
    uint32_t nextUint();

  private:
    int index;
    uint32_t state[16];
};
