#pragma once
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <array>
#include <cmath>
#include <iostream>
#include <unordered_map>
#include <bits/ranges_algobase.h>
#include "Hash_Fancy.hpp"
#include "Random.hpp"


namespace ChessBoard {
#define count_bits(bitboard) __builtin_popcountll(bitboard)
#define IsSlider(attacker, square) (attacker & ~king_attacks[square] & ~knight_attacks[square])
#define opponent_slider_rays_to_square(attacker, square) (line_between[get_ls1b_index(attacker)][king_square])
#define get_ls1b_index(bitboard) __builtin_ctzll(bitboard)
#define Bitloop(X) for(;X; X = X&=X-1)
#define bPawnsAble2Push(wpawns, empty) (nortOne(empty) & (wpawns))
#define wPawnsAble2Push(wpawns, empty) (soutOne(empty) & (wpawns))
#define wPawnsAblePromotePush(wpawns, empty) (wPawnsAble2Push((wpawns), (empty)) & 0x000000000000FF00)
#define bPawnsAblePromotePush(wpawns, empty) (bPawnsAble2Push((wpawns), (empty)) & 0x00FF000000000000)
#define soutOne(b) ((b) << 8)
#define nortOne(b) ((b) >> 8)

    using U64 = uint64_t;

    enum class Side : int { WHITE = 0, BLACK = 1, BOTH = 2 };

    enum {
        P = 0, N = 1, B = 2, R = 3, Q = 4, K = 5,
        p = 6, n = 7, b = 8, r = 9, q = 10, k = 11,
        NO_PIECE = 12,
    };

    constexpr inline std::array<int, 64> castling_rights = {
        7, 15, 15, 15, 3, 15, 15, 11,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        13, 15, 15, 15, 12, 15, 15, 14
    };

    enum class Castling : int { wk = 1, wq = 2, bk = 4, bq = 8 };

    enum {
        a8, b8, c8, d8, e8, f8, g8, h8,
        a7, b7, c7, d7, e7, f7, g7, h7,
        a6, b6, c6, d6, e6, f6, g6, h6,
        a5, b5, c5, d5, e5, f5, g5, h5,
        a4, b4, c4, d4, e4, f4, g4, h4,
        a3, b3, c3, d3, e3, f3, g3, h3,
        a2, b2, c2, d2, e2, f2, g2, h2,
        a1, b1, c1, d1, e1, f1, g1, h1,
        no_sq
    };

    static inline constexpr U64 wPawnsAble2DblPush(U64 wpawns, U64 empty) {
        return wPawnsAble2Push(wpawns, soutOne(empty & 0x000000FF00000000) & empty);
    }

    static inline constexpr U64 bPawnsAble2DblPush(U64 wpawns, U64 empty) {
        return bPawnsAble2Push(wpawns, nortOne(empty & 0x00000000FF000000) & empty);
    }

    constexpr U64 not_a_file = 18374403900871474942ULL;
    constexpr U64 not_h_file = 9187201950435737471ULL;
    constexpr U64 not_hg_file = 4557430888798830399ULL;
    constexpr U64 not_ab_file = 18229723555195321596ULL;

    static inline constexpr void set_bit(U64 &bitboard, const int square) {
        bitboard |= (1ULL << square);
    }

    static inline constexpr U64 get_bit(const U64 bitboard, const int square) {
        return bitboard & (1ULL << square);
    }

    static inline constexpr void pop_bit(U64 &bitboard, const int square) {
        (bitboard) &= ~(1ULL << (square));
    }

    constexpr U64 mask_pawn_attacks(const Side side, const int square) {
        U64 attacks = 0ULL;
        U64 bitboard = 0ULL;
        set_bit(bitboard, square);

        if (side == Side::WHITE) {
            if (((bitboard >> 7) & not_a_file) && (square >= 8)) attacks |= (bitboard >> 7);
            if (((bitboard >> 9) & not_h_file) && (square >= 8)) attacks |= (bitboard >> 9);
        } else {
            if (((bitboard << 7) & not_h_file) && (square < 56)) attacks |= (bitboard << 7);
            if (((bitboard << 9) & not_a_file) && (square < 56)) attacks |= (bitboard << 9);
        }
        return attacks;
    }

    constexpr U64 mask_knight_attacks(const int square) {
        U64 attacks = 0ULL;
        U64 bitboard = 1ULL << square;
        set_bit(bitboard, square);

        if ((bitboard >> 17) & not_h_file) attacks |= (bitboard >> 17);
        if ((bitboard >> 15) & not_a_file) attacks |= (bitboard >> 15);
        if ((bitboard >> 10) & not_hg_file) attacks |= (bitboard >> 10);
        if ((bitboard >> 6) & not_ab_file) attacks |= (bitboard >> 6);
        if ((bitboard << 17) & not_a_file) attacks |= (bitboard << 17);
        if ((bitboard << 15) & not_h_file) attacks |= (bitboard << 15);
        if ((bitboard << 10) & not_ab_file) attacks |= (bitboard << 10);
        if ((bitboard << 6) & not_hg_file) attacks |= (bitboard << 6);

        return attacks;
    }

    constexpr U64 mask_king_attacks(const int square) {
        U64 attacks = 0ULL;
        U64 bitboard = 0ULL;
        set_bit(bitboard, square);

        if (bitboard >> 8) attacks |= (bitboard >> 8);
        if ((bitboard >> 9) & not_h_file) attacks |= (bitboard >> 9);
        if ((bitboard >> 7) & not_a_file) attacks |= (bitboard >> 7);
        if ((bitboard >> 1) & not_h_file) attacks |= (bitboard >> 1);
        if (bitboard << 8) attacks |= (bitboard << 8);
        if ((bitboard << 9) & not_a_file) attacks |= (bitboard << 9);
        if ((bitboard << 7) & not_h_file) attacks |= (bitboard << 7);
        if ((bitboard << 1) & not_a_file) attacks |= (bitboard << 1);

        return attacks;
    }

    constexpr std::array<U64, 64> generate_knight_attacks_table() {
        std::array<U64, 64> table{};
        for (int sq = 0; sq < 64; ++sq) {
            table[sq] = mask_knight_attacks(sq);
        }
        return table;
    }

    constexpr std::array<U64, 64> generate_king_attacks_table() {
        std::array<U64, 64> table{};
        for (int sq = 0; sq < 64; ++sq) {
            table[sq] = mask_king_attacks(sq);
        }
        return table;
    }

    constexpr std::array<std::array<U64, 64>, 2> generate_pawn_attacks_table() {
        std::array<std::array<U64, 64>, 2> table{};
        for (int sq = 0; sq < 64; ++sq) {
            table[static_cast<int>(Side::WHITE)][sq] = mask_pawn_attacks(Side::WHITE, sq);
            table[static_cast<int>(Side::BLACK)][sq] = mask_pawn_attacks(Side::BLACK, sq);
        }
        return table;
    }

    constexpr auto knight_attacks = generate_knight_attacks_table();
    constexpr auto king_attacks = generate_king_attacks_table();
    constexpr auto pawn_attacks = generate_pawn_attacks_table();

    static void print_bitboard(const U64 bitboard) {
        printf("\n");

        for (int rank = 0; rank < 8; rank++) {
            for (int file = 0; file < 8; file++) {
                const int square = rank * 8 + file;

                if (!file) printf("  %d ", 8 - rank);

                printf(" %d", get_bit(bitboard, square) ? 1 : 0);
            }

            printf("\n");
        }

        printf("\n     a b c d e f g h\n\n");
        printf("     Bitboard: %llud\n\n", bitboard);
    }

    using Move = int32_t;

    constexpr inline int get_move_source(Move move) { return move & 0x3f; }
    constexpr inline int get_move_target(Move move) { return (move >> 6) & 0x3f; }
    constexpr inline int get_move_piece(Move move) { return (move >> 12) & 0xf; }
    constexpr inline int get_move_promoted(Move move) { return (move >> 16) & 0xf; }
    constexpr inline bool get_move_capture(Move move) { return (move >> 20) & 1; }
    constexpr inline bool get_move_double(Move move) { return (move >> 21) & 1; }
    constexpr inline bool get_move_enpassant(Move move) { return (move >> 22) & 1; }
    constexpr inline bool get_move_castling(Move move) { return (move >> 23) & 1; }
    constexpr inline bool get_move_attacked(Move move) { return (move >> 24) & 1; }

    static inline int get_move_score(const Move mb) {
        return 100;
    }

    static inline int compare_moves(const void *a, const void *b) {
        const Move *ma = static_cast<const Move *>(a);
        const Move *mb = static_cast<const Move *>(b);

        return get_move_score(*mb) - get_move_score(*ma);
    }

    struct MoveList {
        static constexpr size_t MAX_MOVES = 256;
        std::array<Move, MAX_MOVES> moves{};
        int count = 0;

        void add(const Move move) {
            moves[count++] = move;
        }

        Move *begin() { return moves.data(); }
        Move *end() { return moves.data() + count; }
        [[nodiscard]] const Move *begin() const { return moves.data(); }
        [[nodiscard]] const Move *end() const { return moves.data() + count; }
        [[nodiscard]] size_t size() const { return count; }
        void clear() { count = 0; }
    };

    struct Board {
        std::array<U64, 12> bitboards{};
        std::array<U64, 3> occupancies{};
        std::array<int8_t, 64> piece_at{};
        Side side_to_move = Side::WHITE;
        int enpassant = no_sq;
        int castling_rights = 0;
        U64 hash_key = 0;
    };

    std::unordered_map<char, int> char_pieces = {
        {'P', P},
        {'N', N},
        {'B', B},
        {'R', R},
        {'Q', Q},
        {'K', K},
        {'p', p},
        {'n', n},
        {'b', b},
        {'r', r},
        {'q', q},
        {'k', k}
    };

    std::unordered_map<int, char> promoted_pieces = {
        {Q, 'q'},
        {R, 'r'},
        {B, 'b'},
        {N, 'n'},
        {q, 'q'},
        {r, 'r'},
        {b, 'b'},
        {n, 'n'}
    };

    static inline U64 generate_hash_key(const Board &board) {
        U64 key = 0ULL;
        for (int piece = P; piece <= k; piece++) {
            U64 bitboard = board.bitboards[piece];
            while (bitboard) {
                int square = get_ls1b_index(bitboard);
                pop_bit(bitboard, square);
                key ^= Random::piece_keys[piece][square];
            }
        }

        if (board.enpassant != no_sq) {
            key ^= Random::enpassant_key[board.enpassant];
        }

        key ^= Random::castle_key[board.castling_rights];

        if (board.side_to_move == Side::BLACK) {
            key ^= Random::side_key;
        }

        return key;
    }

    static inline void parse_fen(const char *fen, Board &board) {
        std::ranges::fill(board.bitboards, 0);
        std::ranges::fill(board.occupancies, 0);
        std::ranges::fill(board.piece_at, NO_PIECE);
        board.side_to_move = Side::WHITE;
        board.enpassant = no_sq;
        board.castling_rights = 0;

        for (int rank = 0; rank < 8; rank++) {
            for (int file = 0; file < 8; file++) {
                int square = rank * 8 + file;

                if ((*fen >= 'a' && *fen <= 'z') || (*fen >= 'A' && *fen <= 'Z')) {
                    const int piece = char_pieces.at(*fen);

                    set_bit(board.bitboards[piece], square);
                    board.piece_at[square] = piece;
                    fen++;
                }

                // match empty square numbers within FEN string
                if (*fen >= '0' && *fen <= '9') {
                    // init offset (convert char 0 to int 0)
                    int offset = *fen - '0';

                    // define piece variable
                    int piece = -1;

                    // loop over all piece bitboards
                    for (int bb_piece = 0; bb_piece <= 11; bb_piece++) {
                        if (get_bit(board.bitboards[bb_piece], square))
                            piece = bb_piece;
                    }

                    if (piece == -1)
                        file--;

                    file += offset;

                    fen++;
                }

                if (*fen == '/')
                    fen++;
            }
        }

        fen++;

        (*fen == 'w') ? (board.side_to_move = Side::WHITE) : (board.side_to_move = Side::BLACK);

        fen += 2;

        while (*fen != ' ') {
            switch (*fen) {
                case 'K': board.castling_rights |= static_cast<int>(Castling::wk);
                    break;
                case 'Q': board.castling_rights |= static_cast<int>(Castling::wq);
                    break;
                case 'k': board.castling_rights |= static_cast<int>(Castling::bk);
                    break;
                case 'q': board.castling_rights |= static_cast<int>(Castling::bq);
                    break;
                case '-': break;
                default: ;
            }

            fen++;
        }

        fen++;

        if (*fen != '-') {
            int file = fen[0] - 'a';
            int rank = 8 - (fen[1] - '0');

            board.enpassant = rank * 8 + file;
        } else
            board.enpassant = no_sq;

        for (int piece = 0; piece <= 5; piece++)
            board.occupancies[0] |= board.bitboards[piece];

        for (int piece = 6; piece <= 11; piece++)
            board.occupancies[1] |= board.bitboards[piece];


        board.occupancies[2] |= board.occupancies[0];
        board.occupancies[2] |= board.occupancies[1];

        board.hash_key = generate_hash_key(board);
    }

    constexpr int sign(const int x) {
        return (x > 0) - (x < 0);
    }

    constexpr std::array<std::array<U64, 64>, 64> generate_line_between() {
        std::array<std::array<U64, 64>, 64> table{};
        for (int s = 0; s < 64; s++) {
            for (int t = 0; t < 64; t++) {
                table[s][t] = 0ULL;
                if (s == t)
                    continue;

                int rankS = s / 8, fileS = s % 8;
                int rankT = t / 8, fileT = t % 8;
                int dRank = rankT - rankS;
                int dFile = fileT - fileS;

                if ((dRank == 0) || (dFile == 0) || ((dRank > 0 ? dRank : -dRank) == (dFile > 0 ? dFile : -dFile))) {
                    int stepRank = sign(dRank);
                    int stepFile = sign(dFile);

                    int curRank = rankS + stepRank;
                    int curFile = fileS + stepFile;
                    while ((curRank != rankT) || (curFile != fileT)) {
                        int curSq = curRank * 8 + curFile;
                        table[s][t] |= (1ULL << curSq);
                        curRank += stepRank;
                        curFile += stepFile;
                    }
                }
            }
        }
        return table;
    }

    constexpr auto line_between = generate_line_between();

    template<Side Us, bool excludeKing>
    static inline U64 mask_attacked(const Board &board) {
        constexpr Side Them = (Us == Side::WHITE) ? Side::BLACK : Side::WHITE;
        constexpr int Pawn = (Us == Side::WHITE) ? P : p;
        constexpr int Knight = (Us == Side::WHITE) ? N : n;
        constexpr int Bishop = (Us == Side::WHITE) ? B : b;
        constexpr int Rook = (Us == Side::WHITE) ? R : r;
        constexpr int Queen = (Us == Side::WHITE) ? Q : q;
        constexpr int King = (Us == Side::WHITE) ? K : k;
        constexpr int OurKing = (Us != Side::WHITE) ? K : k;

        U64 occupied = board.occupancies[static_cast<int>(Side::BOTH)];
        U64 attacked = 0ULL;

        if constexpr (excludeKing == true) {
            pop_bit(occupied, get_ls1b_index(board.bitboards[OurKing]));
        }
        int square;

        U64 pawns = board.bitboards[Pawn];
        Bitloop(pawns) {
            square = get_ls1b_index(pawns);
            attacked |= pawn_attacks[static_cast<int>(Us)][square];
        }

        U64 knights = board.bitboards[Knight];
        Bitloop(knights) {
            square = get_ls1b_index(knights);
            attacked |= knight_attacks[square];
        }

        U64 bishops = board.bitboards[Bishop] | board.bitboards[Queen];
        Bitloop(bishops) {
            square = get_ls1b_index(bishops);
            attacked |= Chess_Lookup::Fancy::GetBishopAttacks(square, occupied);
        }

        U64 rooks = board.bitboards[Rook] | board.bitboards[Queen];
        Bitloop(rooks) {
            square = get_ls1b_index(rooks);
            attacked |= Chess_Lookup::Fancy::GetRookAttacks(square, occupied);
        }

        U64 king = board.bitboards[King];
        if (king) {
            square = get_ls1b_index(king);
            attacked |= king_attacks[square];
        }

        return attacked;
    }

    template<Side Us>
    static inline void make_move(Board &board, const int move) {
        int source_square = get_move_source(move);
        int target_square = get_move_target(move);
        int piece = get_move_piece(move);
        int promoted_piece = get_move_promoted(move);
        bool capture = get_move_capture(move);
        bool double_push = get_move_double(move);
        bool enpass = get_move_enpassant(move);
        int castling = get_move_castling(move);

        pop_bit(board.bitboards[piece], source_square);
        set_bit(board.bitboards[piece], target_square);

        board.hash_key ^= Random::piece_keys[piece][source_square];
        board.hash_key ^= Random::piece_keys[piece][target_square];
        int old = board.piece_at[target_square];

        board.piece_at[source_square] = NO_PIECE;
        board.piece_at[target_square] = piece;


        if (capture) {
            if (get_bit(board.bitboards[old], target_square)) {
                pop_bit(board.bitboards[old], target_square);
                board.hash_key ^= Random::piece_keys[old][target_square];
            }
        }


        if (board.enpassant != no_sq) {
            board.hash_key ^= Random::enpassant_key[board.enpassant];
        }

        board.enpassant = no_sq;
        if (promoted_piece != NO_PIECE) {
            if constexpr (Us == Side::WHITE) {
                pop_bit(board.bitboards[P], target_square);

                board.hash_key ^= Random::piece_keys[P][target_square];
            } else {
                pop_bit(board.bitboards[p], target_square);

                board.hash_key ^= Random::piece_keys[p][target_square];
            }

            set_bit(board.bitboards[promoted_piece], target_square);
            board.hash_key ^= Random::piece_keys[promoted_piece][target_square];
            board.piece_at[target_square] = promoted_piece;
        } else if (enpass) {
            if constexpr (Us == Side::WHITE) {
                pop_bit(board.bitboards[p], target_square + 8);
                board.piece_at[target_square + 8] = -1;
                board.hash_key ^= Random::piece_keys[p][target_square + 8];
            } else {
                pop_bit(board.bitboards[P], target_square - 8);
                board.piece_at[target_square - 8] = -1;
                board.hash_key ^= Random::piece_keys[P][target_square - 8];
            }
        } else if (double_push) {
            if constexpr (Us == Side::WHITE) {
                board.enpassant = target_square + 8;
                board.hash_key ^= Random::enpassant_key[target_square + 8];
            } else {
                board.enpassant = target_square - 8;
                board.hash_key ^= Random::enpassant_key[target_square - 8];
            }
        } else if (castling) {
            if constexpr (Us == Side::WHITE) {
                if (target_square == g1) {
                    pop_bit(board.bitboards[R], h1);
                    set_bit(board.bitboards[R], f1);
                    board.piece_at[h1] = NO_PIECE;
                    board.piece_at[f1] = R;
                    board.hash_key ^= Random::piece_keys[R][h1];
                    board.hash_key ^= Random::piece_keys[R][f1];
                } else if (target_square == c1) {
                    pop_bit(board.bitboards[R], a1);
                    set_bit(board.bitboards[R], d1);
                    board.piece_at[a1] = NO_PIECE;
                    board.piece_at[d1] = R;
                    board.hash_key ^= Random::piece_keys[R][a1];
                    board.hash_key ^= Random::piece_keys[R][d1];
                }
            } else {
                if (target_square == g8) {
                    pop_bit(board.bitboards[r], h8);
                    set_bit(board.bitboards[r], f8);
                    board.piece_at[h8] = NO_PIECE;
                    board.piece_at[f8] = r;
                    board.hash_key ^= Random::piece_keys[r][h8];
                    board.hash_key ^= Random::piece_keys[r][f8];
                } else if (target_square == c8) {
                    pop_bit(board.bitboards[r], a8);
                    set_bit(board.bitboards[r], d8);
                    board.piece_at[a8] = NO_PIECE;
                    board.piece_at[d8] = r;
                    board.hash_key ^= Random::piece_keys[r][a8];
                    board.hash_key ^= Random::piece_keys[r][d8];
                }
            }
        }

        board.hash_key ^= Random::castle_key[board.castling_rights];

        board.castling_rights &= castling_rights[source_square];
        board.castling_rights &= castling_rights[target_square];

        board.hash_key ^= Random::castle_key[board.castling_rights];

        board.occupancies[0] =
                board.bitboards[0] | board.bitboards[1] | board.bitboards[2] |
                board.bitboards[3] | board.bitboards[4] | board.bitboards[5];

        board.occupancies[1] =
                board.bitboards[6] | board.bitboards[7] | board.bitboards[8] |
                board.bitboards[9] | board.bitboards[10] | board.bitboards[11];


        board.occupancies[2] = board.occupancies[0] | board.occupancies[1];

        board.hash_key ^= Random::side_key;

        if constexpr (Us == Side::WHITE) {
            board.side_to_move = Side::BLACK;
        } else {
            board.side_to_move = Side::WHITE;
        }
    }

    template<Side Us>
    static inline U64 mask_attackers(const Board &board, const int square) {
        U64 attackers = 0ULL;

        constexpr int Them = static_cast<int>((Us == Side::WHITE) ? Side::BLACK : Side::WHITE);
        constexpr int Pawn = (Us == Side::WHITE) ? P : p;
        constexpr int Knight = (Us == Side::WHITE) ? N : n;
        constexpr int Bishop = (Us == Side::WHITE) ? B : b;
        constexpr int Rook = (Us == Side::WHITE) ? R : r;
        constexpr int Queen = (Us == Side::WHITE) ? Q : q;
        constexpr int King = (Us == Side::WHITE) ? K : k;

        attackers |= knight_attacks[square] & board.bitboards[Knight];

        attackers |= pawn_attacks[Them][square] & board.bitboards[Pawn];

        attackers |= Chess_Lookup::Fancy::GetBishopAttacks(square, board.occupancies[static_cast<int>(Side::BOTH)]) &
                board.bitboards[Bishop];

        attackers |= Chess_Lookup::Fancy::GetRookAttacks(square, board.occupancies[static_cast<int>(Side::BOTH)]) &
                board.bitboards[Rook];

        attackers |= Chess_Lookup::Fancy::Queen(square, board.occupancies[static_cast<int>(Side::BOTH)]) & board.
                bitboards[Queen];

        attackers |= king_attacks[square] & board.bitboards[King];

        return attackers;
    }

    template<Side Us>
    static inline int is_square_attacked(const Board &board, int square) {
        constexpr int Them = static_cast<int>((Us == Side::WHITE) ? Side::BLACK : Side::WHITE);
        constexpr int Pawn = (Us == Side::WHITE) ? P : p;
        constexpr int Knight = (Us == Side::WHITE) ? N : n;
        constexpr int Bishop = (Us == Side::WHITE) ? B : b;
        constexpr int Rook = (Us == Side::WHITE) ? R : r;
        constexpr int Queen = (Us == Side::WHITE) ? Q : q;
        constexpr int King = (Us == Side::WHITE) ? K : k;

        if (pawn_attacks[Them][square] & board.bitboards[Pawn]) return 1;
        if (knight_attacks[square] & board.bitboards[Knight]) return 1;
        if (Chess_Lookup::Fancy::GetBishopAttacks(square, board.occupancies[2]) & board.bitboards[Bishop]) return 1;
        if (Chess_Lookup::Fancy::GetRookAttacks(square, board.occupancies[2]) & board.bitboards[Rook]) return 1;
        if (Chess_Lookup::Fancy::Queen(square, board.occupancies[2]) & board.bitboards[Queen]) return 1;
        if (king_attacks[square] & board.bitboards[King]) return 1;

        return 0;
    }

    static inline U64 pin_mask[64];

    static constexpr inline Move encode_move(
        int source,
        int target,
        int piece,
        int promoted,
        bool capture,
        bool double_push,
        bool enpassant,
        bool castling,
        bool attacked
    ) {
        return (source & 0x3f) |
               ((target & 0x3f) << 6) |
               ((piece & 0xf) << 12) |
               ((promoted & 0xf) << 16) |
               ((capture ? 1 : 0) << 20) |
               ((double_push ? 1 : 0) << 21) |
               ((enpassant ? 1 : 0) << 22) |
               ((castling ? 1 : 0) << 23) |
               ((attacked ? 1 : 0) << 24);
    }

    // Helper to initialize tables
    template<Side MovingSide, int PieceType, int PromotedPiece, bool IsCapture, bool IsDoublePush, bool IsEnpassant,
        bool IsCastle>
    constexpr std::array<std::array<Move, 64>, 64> InitializeMoveTable() {
        std::array<std::array<Move, 64>, 64> table{};
        constexpr int ActualPiece = (PieceType == K || PieceType == k)
                                        ? ((MovingSide == Side::WHITE) ? K : k)
                                        : (PieceType == Q || PieceType == q)
                                              ? ((MovingSide == Side::WHITE) ? Q : q)
                                              : (PieceType == R || PieceType == r)
                                                    ? ((MovingSide == Side::WHITE) ? R : r)
                                                    : (PieceType == B || PieceType == b)
                                                          ? ((MovingSide == Side::WHITE) ? B : b)
                                                          : (PieceType == N || PieceType == n)
                                                                ? ((MovingSide == Side::WHITE) ? N : n)
                                                                :
                                                                /* Pawn */ ((MovingSide == Side::WHITE) ? P : p);

        constexpr int ActualPromotedPiece = (PromotedPiece == NO_PIECE)
                                                ? NO_PIECE
                                                : // No promotion
                                                (PromotedPiece == Q || PromotedPiece == q)
                                                    ? ((MovingSide == Side::WHITE) ? Q : q)
                                                    : (PromotedPiece == R || PromotedPiece == r)
                                                          ? ((MovingSide == Side::WHITE) ? R : r)
                                                          : (PromotedPiece == B || PromotedPiece == b)
                                                                ? ((MovingSide == Side::WHITE) ? B : b)
                                                                :
                                                                /* Knight */ ((MovingSide == Side::WHITE) ? N : n);


        for (int source = 0; source < 64; ++source) {
            for (int target = 0; target < 64; ++target) {
                // Note: is_check is always false during generation here
                table[source][target] = encode_move(source, target, ActualPiece, ActualPromotedPiece,
                                                    IsCapture, IsDoublePush, IsEnpassant, IsCastle, false);
            }
        }
        return table;
    }


    template<Side Us>
    struct PrecomputedMoves {
        // Define piece constants for the current side 'Us'
        static constexpr int Pawn = (Us == Side::WHITE) ? P : p;
        static constexpr int Knight = (Us == Side::WHITE) ? N : n;
        static constexpr int Bishop = (Us == Side::WHITE) ? B : b;
        static constexpr int Rook = (Us == Side::WHITE) ? R : r;
        static constexpr int Queen = (Us == Side::WHITE) ? Q : q;
        static constexpr int King = (Us == Side::WHITE) ? K : k;

        // Define promotion piece constants for the current side 'Us'
        // Using the piece type enum directly might be cleaner if they map correctly
        static constexpr int PromoteQueen = (Us == Side::WHITE) ? Q : q;
        static constexpr int PromoteRook = (Us == Side::WHITE) ? R : r;
        static constexpr int PromoteKnight = (Us == Side::WHITE) ? N : n;
        static constexpr int PromoteBishop = (Us == Side::WHITE) ? B : b;


        // Tables indexed by [source_square][target_square]

        // King Moves
        static constexpr auto KingQuiet = InitializeMoveTable<Us, K, NO_PIECE, false, false, false, false>();
        static constexpr auto KingCapture = InitializeMoveTable<Us, K, NO_PIECE, true, false, false, false>();

        // Pawn Captures (Non-Promotion)
        static constexpr auto PawnCapture = InitializeMoveTable<Us, P, NO_PIECE, true, false, false, false>();

        // Pawn Captures (Promotion) - Use the generic piece type (Q/R/B/N) for the PromotedPiece template arg
        static constexpr auto PawnCapturePromoteQueen = InitializeMoveTable<Us, P, Q, true, false, false, false>();
        static constexpr auto PawnCapturePromoteRook = InitializeMoveTable<Us, P, R, true, false, false, false>();
        static constexpr auto PawnCapturePromoteKnight = InitializeMoveTable<Us, P, N, true, false, false, false>();
        static constexpr auto PawnCapturePromoteBishop = InitializeMoveTable<Us, P, B, true, false, false, false>();

        // Pawn Push (Single, Non-Promotion)
        static constexpr auto PawnPush = InitializeMoveTable<Us, P, NO_PIECE, false, false, false, false>();

        // Pawn Push (Single, Promotion)
        static constexpr auto PawnPushPromoteQueen = InitializeMoveTable<Us, P, Q, false, false, false, false>();
        static constexpr auto PawnPushPromoteRook = InitializeMoveTable<Us, P, R, false, false, false, false>();
        static constexpr auto PawnPushPromoteKnight = InitializeMoveTable<Us, P, N, false, false, false, false>();
        static constexpr auto PawnPushPromoteBishop = InitializeMoveTable<Us, P, B, false, false, false, false>();

        static constexpr auto PawnDoublePush = InitializeMoveTable<Us, P, NO_PIECE, false, true, false, false>();
        static constexpr auto Enpassant = InitializeMoveTable<Us, P, NO_PIECE, true, false, true, false>();

        static constexpr auto KnightQuiet = InitializeMoveTable<Us, N, NO_PIECE, false, false, false, false>();
        static constexpr auto KnightCapture = InitializeMoveTable<Us, N, NO_PIECE, true, false, false, false>();

        static constexpr auto BishopQuiet = InitializeMoveTable<Us, B, NO_PIECE, false, false, false, false>();
        static constexpr auto BishopCapture = InitializeMoveTable<Us, B, NO_PIECE, true, false, false, false>();

        static constexpr auto RookQuiet = InitializeMoveTable<Us, R, NO_PIECE, false, false, false, false>();
        static constexpr auto RookCapture = InitializeMoveTable<Us, R, NO_PIECE, true, false, false, false>();

        static constexpr auto QueenQuiet = InitializeMoveTable<Us, Q, NO_PIECE, false, false, false, false>();
        static constexpr auto QueenCapture = InitializeMoveTable<Us, Q, NO_PIECE, true, false, false, false>();

        static constexpr Move CastleKingside = encode_move(
            (Us == Side::WHITE) ? e1 : e8,
            (Us == Side::WHITE) ? g1 : g8,
            King, NO_PIECE, false, false, false, true, false);

        static constexpr Move CastleQueenside = encode_move(
            (Us == Side::WHITE) ? e1 : e8,
            (Us == Side::WHITE) ? c1 : c8,
            King, NO_PIECE, false, false, false, true, false);
    };


    template<Side Us>
    static inline constexpr void generate_moves_internal(const Board &board, MoveList &move_list) {
        constexpr Side Them = (Us == Side::WHITE) ? Side::BLACK : Side::WHITE;
        const U64 attackedSquares = mask_attacked<Them, true>(board);
        U64 mask_pins = 0ULL;
        U64 capture_mask = 0xFFFFFFFFFFFFFFFFULL;
        U64 push_mask = 0xFFFFFFFFFFFFFFFFULL;

        constexpr int PawnDir = (Us == Side::WHITE) ? -8 : +8;
        constexpr int PawnDirDouble = (Us == Side::WHITE) ? -16 : +16;

        constexpr int TheyQueen = Us == Side::WHITE ? q : Q;
        constexpr int TheyRook = Us == Side::WHITE ? r : R;
        constexpr int TheyBishop = Us == Side::WHITE ? b : B;
        constexpr U64 Rank8 = (Us == Side::WHITE) ? 0xFFULL : 0xFFULL << 56;


        const U64 us_pieces = board.occupancies[static_cast<int>(Us)];
        const U64 them_pieces = board.occupancies[static_cast<int>(Them)];
        const U64 occupied = board.occupancies[static_cast<int>(Side::BOTH)];
        const U64 empty = ~occupied;


        constexpr int Pawn = (Us == Side::WHITE) ? P : p;
        constexpr int Knight = (Us == Side::WHITE) ? N : n;
        constexpr int Bishop = (Us == Side::WHITE) ? B : b;
        constexpr int Rook = (Us == Side::WHITE) ? R : r;
        constexpr int Queen = (Us == Side::WHITE) ? Q : q;
        constexpr int King = (Us == Side::WHITE) ? K : k;


        U64 pieces = board.bitboards[King];
        int source_square = get_ls1b_index(pieces);
        int king_square = source_square;
        U64 checkers = mask_attackers<Them>(board, source_square);
        const int attackersCount = count_bits(checkers);

        U64 attacks = king_attacks[source_square] & ~us_pieces & ~attackedSquares;

        U64 quiet_attacks = attacks & ~them_pieces;
        Bitloop(quiet_attacks) {
            const int target_square = get_ls1b_index(quiet_attacks);
            move_list.add(PrecomputedMoves<Us>::KingQuiet[source_square][target_square]);
        }

        U64 capture_attacks = attacks & them_pieces;
        Bitloop(capture_attacks) {
            const int target_square = get_ls1b_index(capture_attacks);
            move_list.add(PrecomputedMoves<Us>::KingCapture[source_square][target_square]);
        }


        if (attackersCount > 1) {
            return;
        } else if (attackersCount == 1) {
            capture_mask = checkers;
            if (IsSlider(checkers, king_square) != 0ULL) {
                push_mask = opponent_slider_rays_to_square(checkers, king_square);
            } else {
                push_mask = 0ULL;
            }
        }
        U64 pinboard = (board.bitboards[TheyQueen] | board.bitboards[TheyRook]) &
                       Chess_Lookup::Fancy::GetRookAttacks(king_square, 0ULL) | (
                           board.bitboards[TheyQueen] | board.bitboards[TheyBishop]) &
                       Chess_Lookup::Fancy::GetBishopAttacks(king_square, 0ULL);

        Bitloop(pinboard) {
            const int square = get_ls1b_index(pinboard);
            const U64 line = line_between[square][king_square];

            if (const U64 pseudo_pins = line & occupied; count_bits(pseudo_pins) == 1) {
                if (const U64 pins = pseudo_pins & us_pieces) {
                    mask_pins |= pins;
                    pin_mask[get_ls1b_index(pins)] = line | (1ULL << square);
                }
            }
        }

        pieces = board.bitboards[Pawn];

        Bitloop(pieces) {
            U64 full_mask = 0xFFFFFFFFFFFFFFFFULL;
            source_square = get_ls1b_index(pieces);
            const U64 condition = -((mask_pins & (1ULL << source_square)) != 0);
            full_mask = (pin_mask[source_square] & condition) | (0xFFFFFFFFFFFFFFFFULL & ~condition);

            attacks = pawn_attacks[static_cast<int>(Us)][source_square] & them_pieces & capture_mask & full_mask;
            capture_attacks = attacks & Rank8;
            quiet_attacks = attacks & ~capture_attacks;

            Bitloop(quiet_attacks) {
                const int target_square = get_ls1b_index(quiet_attacks);
                move_list.add(PrecomputedMoves<Us>::PawnCapture[source_square][target_square]);
            }

            Bitloop(capture_attacks) {
                const int target_square = get_ls1b_index(capture_attacks);
                move_list.add(PrecomputedMoves<Us>::PawnCapturePromoteQueen[source_square][target_square]);
                move_list.add(PrecomputedMoves<Us>::PawnCapturePromoteRook[source_square][target_square]);
                move_list.add(PrecomputedMoves<Us>::PawnCapturePromoteKnight[source_square][target_square]);
                move_list.add(PrecomputedMoves<Us>::PawnCapturePromoteBishop[source_square][target_square]);
            }
        }

        if (board.enpassant != no_sq) {
            U64 enpassant_attacks = pawn_attacks[static_cast<int>(Them)][board.enpassant] & board.bitboards[Pawn];
            Bitloop(enpassant_attacks) {
                source_square = get_ls1b_index(enpassant_attacks);
                int target_square = board.enpassant;
                const bool is_pinned = (mask_pins & (1ULL << source_square)) != 0;
                if (is_pinned) {
                    int captured_pawn_sq = target_square + ((Us == Side::WHITE) ? +8 : -8);
                    U64 move_and_capture_squares = (1ULL << target_square) | (1ULL << captured_pawn_sq);
                    if ((pin_mask[source_square] & move_and_capture_squares) == 0) {
                        continue;
                    }
                }
                int move = PrecomputedMoves<Us>::Enpassant[source_square][target_square];
                Board enpassant_board = board;
                make_move<Us>(enpassant_board, move);
                if (!is_square_attacked<Them>(enpassant_board, king_square)) {
                    move_list.add(move);
                }
            }
        }
        pieces = board.bitboards[Pawn];
        U64 promote;
        if constexpr (Us == Side::WHITE) {
            promote = wPawnsAblePromotePush(pieces, empty);
            pieces = wPawnsAble2Push(pieces, empty) & ~promote & (push_mask << 8);
        } else {
            promote = bPawnsAblePromotePush(pieces, empty);
            pieces = bPawnsAble2Push(pieces, empty) & ~promote & (push_mask >> 8);
        }

        Bitloop(pieces) {
            source_square = get_ls1b_index(pieces);
            const int target_square = source_square + PawnDir;

            U64 masked = (mask_pins >> source_square) & 1;
            U64 valid_if_masked = (pin_mask[source_square] >> target_square) & 1;
            int valid = static_cast<int>(masked ^ 1 | valid_if_masked);

            move_list.moves[move_list.count] = PrecomputedMoves<Us>::PawnPush[source_square][target_square];
            move_list.count += valid;
        }

        if constexpr (Us == Side::WHITE) {
            pieces = promote & (push_mask << 8);
        } else {
            pieces = promote & (push_mask >> 8);
        }

        Bitloop(pieces) {
            source_square = get_ls1b_index(pieces);
            const int target_square = source_square + PawnDir;

            U64 masked = (mask_pins >> source_square) & 1ull;
            U64 valid_if_masked = (pin_mask[source_square] >> target_square) & 1ull;
            int valid = static_cast<int>(masked ^ 1 | valid_if_masked);

            move_list.moves[move_list.count] = PrecomputedMoves<Us>::PawnPushPromoteQueen[source_square][target_square];
            move_list.count += valid;
            move_list.moves[move_list.count] = PrecomputedMoves<Us>::PawnPushPromoteRook[source_square][target_square];
            move_list.count += valid;
            move_list.moves[move_list.count] = PrecomputedMoves<Us>::PawnPushPromoteKnight[source_square][
                target_square];
            move_list.count += valid;
            move_list.moves[move_list.count] = PrecomputedMoves<Us>::PawnPushPromoteBishop[source_square][
                target_square];
            move_list.count += valid;
        }

        pieces = board.bitboards[Pawn];
        if constexpr (Us == Side::WHITE) {
            pieces = wPawnsAble2DblPush(pieces, empty) & (push_mask << 16);
        } else {
            pieces = bPawnsAble2DblPush(pieces, empty) & (push_mask >> 16);
        }

        Bitloop(pieces) {
            source_square = get_ls1b_index(pieces);
            const int target_square = source_square + PawnDirDouble;
            U64 masked = (mask_pins >> source_square) & 1;
            U64 valid_if_masked = (pin_mask[source_square] >> target_square) & 1;
            int valid = static_cast<int>(masked ^ 1 | valid_if_masked);

            move_list.moves[move_list.count] = PrecomputedMoves<Us>::PawnDoublePush[source_square][target_square];
            move_list.count += valid;
        }

        pieces = board.bitboards[Knight];
        Bitloop(pieces) {
            U64 full_mask = 0xFFFFFFFFFFFFFFFFULL;
            source_square = get_ls1b_index(pieces);
            if ((mask_pins & (1ULL << source_square)) != 0) {
                full_mask = pin_mask[source_square];
            }
            attacks = knight_attacks[source_square] & ~us_pieces & full_mask;

            quiet_attacks = attacks & ~them_pieces & push_mask;
            Bitloop(quiet_attacks) {
                int target_square = get_ls1b_index(quiet_attacks);
                move_list.add(PrecomputedMoves<Us>::KnightQuiet[source_square][target_square]);
            }

            capture_attacks = attacks & them_pieces & capture_mask;
            Bitloop(capture_attacks) {
                int target_square = get_ls1b_index(capture_attacks);
                move_list.add(PrecomputedMoves<Us>::KnightCapture[source_square][target_square]);
            }
        }


        pieces = board.bitboards[Bishop];
        Bitloop(pieces) {
            U64 full_mask = 0xFFFFFFFFFFFFFFFFULL;
            source_square = get_ls1b_index(pieces);
            if ((mask_pins & (1ULL << source_square)) != 0) {
                full_mask = pin_mask[source_square];
            }
            attacks = Chess_Lookup::Fancy::GetBishopAttacks(source_square, occupied) & ~us_pieces & full_mask;

            quiet_attacks = attacks & ~them_pieces & push_mask;
            Bitloop(quiet_attacks) {
                int target_square = get_ls1b_index(quiet_attacks);
                move_list.add(PrecomputedMoves<Us>::BishopQuiet[source_square][target_square]);
            }

            capture_attacks = attacks & them_pieces & capture_mask;
            Bitloop(capture_attacks) {
                int target_square = get_ls1b_index(capture_attacks);
                move_list.add(PrecomputedMoves<Us>::BishopCapture[source_square][target_square]);
            }
        }

        pieces = board.bitboards[Rook];
        Bitloop(pieces) {
            U64 full_mask = 0xFFFFFFFFFFFFFFFFULL;
            source_square = get_ls1b_index(pieces);
            if ((mask_pins & (1ULL << source_square)) != 0) {
                full_mask = pin_mask[source_square];
            }
            attacks = Chess_Lookup::Fancy::GetRookAttacks(source_square, occupied) & ~us_pieces & full_mask;

            quiet_attacks = attacks & ~them_pieces & push_mask;
            Bitloop(quiet_attacks) {
                int target_square = get_ls1b_index(quiet_attacks);
                move_list.add(PrecomputedMoves<Us>::RookQuiet[source_square][target_square]);
            }

            capture_attacks = attacks & them_pieces & capture_mask;
            Bitloop(capture_attacks) {
                int target_square = get_ls1b_index(capture_attacks);
                move_list.add(PrecomputedMoves<Us>::RookCapture[source_square][target_square]);
            }
        }

        pieces = board.bitboards[Queen];
        Bitloop(pieces) {
            U64 full_mask = 0xFFFFFFFFFFFFFFFFULL;
            source_square = get_ls1b_index(pieces);
            if ((mask_pins & (1ULL << source_square)) != 0) {
                full_mask = pin_mask[source_square];
            }
            attacks = Chess_Lookup::Fancy::Queen(source_square, occupied) & ~us_pieces & full_mask;

            quiet_attacks = attacks & ~them_pieces & push_mask;
            Bitloop(quiet_attacks) {
                int target_square = get_ls1b_index(quiet_attacks);
                move_list.add(PrecomputedMoves<Us>::QueenQuiet[source_square][target_square]);
            }

            capture_attacks = attacks & them_pieces & capture_mask;
            Bitloop(capture_attacks) {
                int target_square = get_ls1b_index(capture_attacks);
                move_list.add(PrecomputedMoves<Us>::QueenCapture[source_square][target_square]);
            }
        }

        if constexpr (Us == Side::WHITE) {
            if (board.castling_rights & static_cast<int>(Castling::wk)) {
                if (!(board.occupancies[2] & (1ull << f1 | 1ull << g1))) {
                    if (!(attackedSquares & (1ull << e1 | 1ull << f1 | 1ull << g1)))
                        move_list.add(PrecomputedMoves<Us>::CastleKingside);
                }
            }

            if (board.castling_rights & static_cast<int>(Castling::wq)) {
                if (!(board.occupancies[2] & (1ull << d1 | 1ull << c1 | 1ull << b1))) {
                    if (!(attackedSquares & (1ull << e1 | 1ull << d1 | 1ull << c1)))
                        move_list.add(PrecomputedMoves<Us>::CastleQueenside);
                }
            }
        } else {
            if (board.castling_rights & static_cast<int>(Castling::bk)) {
                if (!(board.occupancies[2] & (1ull << f8 | 1ull << g8))) {
                    if (!(attackedSquares & (1ull << e8 | 1ull << f8 | 1ull << g8)))
                        move_list.add(PrecomputedMoves<Us>::CastleKingside);
                }
            }

            if (board.castling_rights & static_cast<int>(Castling::bq)) {
                if (!(board.occupancies[2] & (1ull << d8 | 1ull << c8 | 1ull << b8))) {
                    if (!(attackedSquares & (1ull << e8 | 1ull << d8 | 1ull << c8)))
                        move_list.add(PrecomputedMoves<Us>::CastleQueenside);
                }
            }
        }
    }

    static inline int test_ply;
    static inline MoveList moves[128];

    static inline unsigned long long perft(const Board &board, const int depth) {
        if (depth == 0) {
            return 1ULL;
        }

        unsigned long long nodes = 0;
        MoveList &move_list = moves[depth];
        move_list.count = 0;

        if (board.side_to_move == Side::WHITE) {
            generate_moves_internal<Side::WHITE>(board, move_list);
        } else {
            generate_moves_internal<Side::BLACK>(board, move_list);
        }

        for (int i = 0; i < move_list.count; i++) {
            const int move = move_list.moves[i];
            test_ply++;
            Board copy = board;
            if (board.side_to_move == Side::WHITE) {
                make_move<Side::WHITE>(copy, move);
            } else {
                make_move<Side::BLACK>(copy, move);
            }
            nodes += perft(copy, depth - 1);
            test_ply--;
        }

        return nodes;
    }

#define start_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define king_position "8/4p3/8/1k6/8/8/1K6/8 b - - 0 1"

    static const char *square_to_coordinates[] = {
        "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
        "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
        "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
        "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
        "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
        "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
        "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
        "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    };

    static inline char *get_move_string(int move) {
        static char move_str[6]; // Max length: "e2e4Q" + null terminator

        if (get_move_promoted(move)) {
            snprintf(move_str, sizeof(move_str), "%s%s%c",
                     square_to_coordinates[get_move_source(move)],
                     square_to_coordinates[get_move_target(move)],
                     promoted_pieces[get_move_promoted(move)]);
        } else {
            snprintf(move_str, sizeof(move_str), "%s%s",
                     square_to_coordinates[get_move_source(move)],
                     square_to_coordinates[get_move_target(move)]);
        }
        move_str[sizeof(move_str) - 1] = '\0';
        return move_str;
    }
}
