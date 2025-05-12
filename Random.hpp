#pragma once

namespace Random {
    static inline unsigned long long enpassant_key[64];
    static inline unsigned long long castle_key[16];
    static inline unsigned long long piece_keys[12][64];
    static inline unsigned long long side_key;
    static inline unsigned int random_state = 1804289383;

    static inline unsigned int get_random_U32_number() {
        unsigned int number = random_state;

        number ^= number << 13;
        number ^= number >> 17;
        number ^= number << 5;

        random_state = number;

        return number;
    }

    static inline unsigned long long get_random_U64_number() {
        unsigned long long n1, n2, n3, n4;

        n1 = static_cast<unsigned long long>(get_random_U32_number()) & 0xFFFF;
        n2 = static_cast<unsigned long long>(get_random_U32_number()) & 0xFFFF;
        n3 = static_cast<unsigned long long>(get_random_U32_number()) & 0xFFFF;
        n4 = static_cast<unsigned long long>(get_random_U32_number()) & 0xFFFF;

        return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
    }

    static inline void init_random_keys() {
        random_state = 1804289383;

        for (int piece = 0; piece <= 11; piece++) {
            for (int square = 0; square < 64; square++)
                piece_keys[piece][square] = get_random_U64_number();
        }

        for (int square = 0; square < 64; square++)
            enpassant_key[square] = get_random_U64_number();

        for (int index = 0; index < 16; index++)
            castle_key[index] = get_random_U64_number();

        side_key = get_random_U64_number();
    }
}
