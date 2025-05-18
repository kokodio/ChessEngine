#pragma once

namespace Random {
    struct RandomKeys {
        unsigned long long piece_keys[12][64];
        unsigned long long enpassant_key[64];
        unsigned long long castle_key[16];
        unsigned long long side_key;
    };

    static inline constexpr unsigned int get_random_U32_number(unsigned int& random_state) {
        unsigned int number = random_state;

        number ^= number << 13;
        number ^= number >> 17;
        number ^= number << 5;

        random_state = number;

        return number;
    }

    static constexpr inline unsigned long long get_random_U64_number(unsigned int& random_state) {
        unsigned long long n1, n2, n3, n4;

        n1 = static_cast<unsigned long long>(get_random_U32_number(random_state)) & 0xFFFF;
        n2 = static_cast<unsigned long long>(get_random_U32_number(random_state)) & 0xFFFF;
        n3 = static_cast<unsigned long long>(get_random_U32_number(random_state)) & 0xFFFF;
        n4 = static_cast<unsigned long long>(get_random_U32_number(random_state)) & 0xFFFF;

        return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
    }

    static consteval inline auto init_random_keys() {
        RandomKeys keys = {};
        unsigned int random_state = 1804289383;

        for (int piece = 0; piece <= 11; piece++) {
            for (int square = 0; square < 64; square++)
                keys.piece_keys[piece][square] = get_random_U64_number(random_state);
        }

        for (int square = 0; square < 64; square++)
            keys.enpassant_key[square] = get_random_U64_number(random_state);

        for (int index = 0; index < 16; index++)
            keys.castle_key[index] = get_random_U64_number(random_state);

        keys.side_key = get_random_U64_number(random_state);

        return keys;
    }

    static constexpr inline auto keys = init_random_keys();
    static_assert(keys.side_key == 1772028295636336235);
}
