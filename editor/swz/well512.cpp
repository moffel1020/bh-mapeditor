#include "well512.h"

Well512::Well512(uint32_t seed) {
    index = 0;

    setSeed(seed);
}

void Well512::setSeed(uint32_t seed) {
    index = 0;

    state[0] = seed;
    for (uint32_t i = 1u; i < 0x10; i++) {
        state[i] = i + 0x6C078965 * (state[i - 1] ^ (state[i - 1] >> 30));
    }
}

uint32_t Well512::nextUint() {
    uint32_t a, b, c, d;

    a = state[index];
    c = state[(index + 13) & 15];
    b = a ^ c ^ (a << 16) ^ (c << 15);
    c = state[(index + 9) & 15];
    c ^= c >> 11;
    a = state[index] = b ^ c;
    d = a ^ ((a << 5) & 0xDA442D24);
    index = (index + 15) & 15;
    a = state[index];
    state[index] = a ^ b ^ d ^ (a << 2) ^ (b << 18) ^ (c << 28);

    return state[index];
}
