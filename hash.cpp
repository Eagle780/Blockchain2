#include "header.h"

static inline uint64_t rotl(uint64_t x, unsigned r)
{
    return (x << r) | (x >> (64 - r));
}

void customHash256(const string &input, uint64_t out[4])
{

    out[0] = 0x123;
    out[1] = 0x0FEDCBA987654321;
    out[2] = 0xAAAAAAAA55555555;
    out[3] = 0xFFFFFFFF00000000;

    for (size_t i = 0; i < input.size(); i++)
    {
        uint64_t c = (unsigned char)input[i];
        size_t lane = i % 4;

        out[lane] ^= c * 0x100000001b3ULL;
        out[lane] = rotl(out[lane], (int)((i * 7) % 64));
        out[lane] *= 0xff51afd7ed558ccdULL;
        out[lane] ^= (out[lane] >> 32);

        size_t other = (lane + 1) % 4;
        out[other] ^= rotl(c + out[lane], (int)((i * 13) % 64));
        out[other] *= 0x9e3779b97f4a7c15ULL;
    }

    for (int round = 0; round < 4; ++round)
    {
        for (int j = 0; j < 4; ++j)
        {
            uint64_t x = out[j];
            x ^= rotl(out[(j + 1) % 4], j * 17 + round * 11);
            x *= 0xc2b2ae3d27d4eb4fULL;
            x ^= (x >> 29);
            out[j] = x;
        }
    }
}

string stringHash(string str)
{
    uint64_t hash[4];
    customHash256(str, hash);

    stringstream ss;
    ss << hex << setfill('0');
    for (int i = 0; i < 4; i++)
    {
        ss << setw(16) << hash[i];
    }

    return ss.str();
}