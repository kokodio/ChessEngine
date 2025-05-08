#pragma once
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "Board.hpp"
#include "Console.hpp"
#include "Evaluation.hpp"
using namespace ChessBoard;


namespace OldSearch{

#define infinity 50000
#define mate_value 49000
#define mate_score 48000
static char ascii_pieces[] = "PNBRQKpnbrqk";


static int movestogo = 30;
static int movetime = -1;
static int time_uci = -1;
static int inc = 0;
static int starttime = 0;


/**********************************\
 ==================================

       Miscellaneous functions
          forked from VICE
         by Richard Allbert

 ==================================
\**********************************/

// get time in milliseconds



//inline char *unicode_pieces[12] = {"♙", "♘", "♗", "♖", "♕", "♔", "♟︎", "♞", "♝", "♜", "♛", "♚"};



// print board



/**********************************\
 ==================================

              Attacks

 ==================================
\**********************************/

/*
     not A file          not H file         not HG files      not AB files
      bitboard            bitboard            bitboard          bitboard

 8  0 1 1 1 1 1 1 1    1 1 1 1 1 1 1 0    1 1 1 1 1 1 0 0    0 0 1 1 1 1 1 1
 7  0 1 1 1 1 1 1 1    1 1 1 1 1 1 1 0    1 1 1 1 1 1 0 0    0 0 1 1 1 1 1 1
 6  0 1 1 1 1 1 1 1    1 1 1 1 1 1 1 0    1 1 1 1 1 1 0 0    0 0 1 1 1 1 1 1
 5  0 1 1 1 1 1 1 1    1 1 1 1 1 1 1 0    1 1 1 1 1 1 0 0    0 0 1 1 1 1 1 1
 4  0 1 1 1 1 1 1 1    1 1 1 1 1 1 1 0    1 1 1 1 1 1 0 0    0 0 1 1 1 1 1 1
 3  0 1 1 1 1 1 1 1    1 1 1 1 1 1 1 0    1 1 1 1 1 1 0 0    0 0 1 1 1 1 1 1
 2  0 1 1 1 1 1 1 1    1 1 1 1 1 1 1 0    1 1 1 1 1 1 0 0    0 0 1 1 1 1 1 1
 1  0 1 1 1 1 1 1 1    1 1 1 1 1 1 1 0    1 1 1 1 1 1 0 0    0 0 1 1 1 1 1 1

    a b c d e f g h    a b c d e f g h    a b c d e f g h    a b c d e f g h

*/


void print_attacked_squares(Board& board, Side side)
{
    printf("\n");

    // loop over board ranks
    for (int rank = 0; rank < 8; rank++)
    {
        // loop over board files
        for (int file = 0; file < 8; file++)
        {
            // init square
            int square = rank * 8 + file;

            // print ranks
            if (!file)
                printf("  %d ", 8 - rank);

            // check whether current square is attacked or not
            bool is_att;

            if (side == Side::WHITE){
                is_att = is_square_attacked<Side::WHITE>(board, square);
            }
            else{
               is_att = is_square_attacked<Side::BLACK>(board, square);
            }
            printf(" %d", 0);
        }

        // print new line every rank
        printf("\n");
    }

    // print files
    printf("\n     a b c d e f g h\n\n");
}

void print_move(int move)
{
    if (get_move_promoted(move))
        printf("%s%s%c", square_to_coordinates[get_move_source(move)],
                           square_to_coordinates[get_move_target(move)],
                           promoted_pieces[get_move_promoted(move)]);
    else
        printf("%s%s", square_to_coordinates[get_move_source(move)],
                           square_to_coordinates[get_move_target(move)]);
}

// print move list
void print_move_list(MoveList& move_list)
{
    // do nothing on empty move list
    if (!move_list.count)
    {
        printf("\n     No move in the move list!\n");
        return;
    }

    printf("\n     move    piece     capture   double    enpass    castling\n\n");

    // loop over moves within a move list
    for (int move_count = 0; move_count < move_list.count; move_count++)
    {
        // init move
        int move = move_list.moves[move_count];

        #ifdef WIN64
            // print move
            printf("      %s%s%c   %c         %d         %d         %d         %d\n", square_to_coordinates[get_move_source(move)],
                                                                                  square_to_coordinates[get_move_target(move)],
                                                                                  get_move_promoted(move) ? promoted_pieces[get_move_promoted(move)] : ' ',
                                                                                  ascii_pieces[get_move_piece(move)],
                                                                                  get_move_capture(move) ? 1 : 0,
                                                                                  get_move_double(move) ? 1 : 0,
                                                                                  get_move_enpassant(move) ? 1 : 0,
                                                                                  get_move_castling(move) ? 1 : 0);
        #else
            // print move
            printf("     %s%s%c   %s         %d         %d         %d         %d\n", square_to_coordinates[get_move_source(move)],
                                                                                  square_to_coordinates[get_move_target(move)],
                                                                                  get_move_promoted(move) ? promoted_pieces[get_move_promoted(move)] : ' ',
                                                                                  unicode_pieces[get_move_piece(move)],
                                                                                  get_move_capture(move) ? 1 : 0,
                                                                                  get_move_double(move) ? 1 : 0,
                                                                                  get_move_enpassant(move) ? 1 : 0,
                                                                                  get_move_castling(move) ? 1 : 0);
        #endif

    }

    // print total number of moves
    printf("\n\n     Total number of moves: %d\n\n", move_list.count);

}

// preserve board state
// #define copy_board()                                                      \
//     U64 bitboards_copy[12], occupancies_copy[3];                          \
//     int side_copy, enpassant_copy, castle_copy;                           \
//     memcpy(bitboards_copy, bitboards, 96);                                \
//     memcpy(occupancies_copy, occupancies, 24);                            \
//     side_copy = side, enpassant_copy = enpassant, castle_copy = castle;   \
//
// // restore board state
// #define take_back()                                                       \
//     memcpy(bitboards, bitboards_copy, 96);                                \
//     memcpy(occupancies, occupancies_copy, 24);                            \
//     side = side_copy, enpassant = enpassant_copy, castle = castle_copy;   \

// move types
enum { all_moves, only_captures };

/*
                           castling   move     in      in
                              right update     binary  decimal

 king & rooks didn't move:     1111 & 1111  =  1111    15

        white king  moved:     1111 & 1100  =  1100    12
  white king's rook moved:     1111 & 1110  =  1110    14
 white queen's rook moved:     1111 & 1101  =  1101    13

         black king moved:     1111 & 0011  =  1011    3
  black king's rook moved:     1111 & 1011  =  1011    11
 black queen's rook moved:     1111 & 0111  =  0111    7

*/

int material_score[12] = {
    100,      // white pawn score
    300,      // white knight scrore
    350,      // white bishop score
    500,      // white rook score
   1000,      // white queen score
   -100,      // black pawn score
   -300,      // black knight scrore
   -350,      // black bishop score
   -500,      // black rook score
  -1000,      // black queen score
};

int mg_pawn_table[64] = {
      0,   0,   0,   0,   0,   0,  0,   0,
     98, 134,  61,  95,  68, 126, 34, -11,
     -6,   7,  26,  31,  65,  56, 25, -20,
    -14,  13,   6,  21,  23,  12, 17, -23,
    -27,  -2,  -5,  12,  17,   6, 10, -25,
    -26,  -4,  -4, -10,   3,   3, 33, -12,
    -35,  -1, -20, -23, -15,  24, 38, -22,
      0,   0,   0,   0,   0,   0,  0,   0,
};

int eg_pawn_table[64] = {
      0,   0,   0,   0,   0,   0,   0,   0,
    178, 173, 158, 134, 147, 132, 165, 187,
     94, 100,  85,  67,  56,  53,  82,  84,
     32,  24,  13,   5,  -2,   4,  17,  17,
     13,   9,  -3,  -7,  -7,  -8,   3,  -1,
      4,   7,  -6,   1,   0,  -5,  -1,  -8,
     13,   8,   8,  10,  13,   0,   2,  -7,
      0,   0,   0,   0,   0,   0,   0,   0,
};

int mg_knight_table[64] = {
    -167, -89, -34, -49,  61, -97, -15, -107,
     -73, -41,  72,  36,  23,  62,   7,  -17,
     -47,  60,  37,  65,  84, 129,  73,   44,
      -9,  17,  19,  53,  37,  69,  18,   22,
     -13,   4,  16,  13,  28,  19,  21,   -8,
     -23,  -9,  12,  10,  19,  17,  25,  -16,
     -29, -53, -12,  -3,  -1,  18, -14,  -19,
    -105, -21, -58, -33, -17, -28, -19,  -23,
};

int eg_knight_table[64] = {
    -58, -38, -13, -28, -31, -27, -63, -99,
    -25,  -8, -25,  -2,  -9, -25, -24, -52,
    -24, -20,  10,   9,  -1,  -9, -19, -41,
    -17,   3,  22,  22,  22,  11,   8, -18,
    -18,  -6,  16,  25,  16,  17,   4, -18,
    -23,  -3,  -1,  15,  10,  -3, -20, -22,
    -42, -20, -10,  -5,  -2, -20, -23, -44,
    -29, -51, -23, -15, -22, -18, -50, -64,
};

int mg_bishop_table[64] = {
    -29,   4, -82, -37, -25, -42,   7,  -8,
    -26,  16, -18, -13,  30,  59,  18, -47,
    -16,  37,  43,  40,  35,  50,  37,  -2,
     -4,   5,  19,  50,  37,  37,   7,  -2,
     -6,  13,  13,  26,  34,  12,  10,   4,
      0,  15,  15,  15,  14,  27,  18,  10,
      4,  15,  16,   0,   7,  21,  33,   1,
    -33,  -3, -14, -21, -13, -12, -39, -21,
};

int eg_bishop_table[64] = {
    -14, -21, -11,  -8, -7,  -9, -17, -24,
     -8,  -4,   7, -12, -3, -13,  -4, -14,
      2,  -8,   0,  -1, -2,   6,   0,   4,
     -3,   9,  12,   9, 14,  10,   3,   2,
     -6,   3,  13,  19,  7,  10,  -3,  -9,
    -12,  -3,   8,  10, 13,   3,  -7, -15,
    -14, -18,  -7,  -1,  4,  -9, -15, -27,
    -23,  -9, -23,  -5, -9, -16,  -5, -17,
};

int mg_rook_table[64] = {
     32,  42,  32,  51, 63,  9,  31,  43,
     27,  32,  58,  62, 80, 67,  26,  44,
     -5,  19,  26,  36, 17, 45,  61,  16,
    -24, -11,   7,  26, 24, 35,  -8, -20,
    -36, -26, -12,  -1,  9, -7,   6, -23,
    -45, -25, -16, -17,  3,  0,  -5, -33,
    -44, -16, -20,  -9, -1, 11,  -6, -71,
    -19, -13,   1,  17, 16,  7, -37, -26,
};

int eg_rook_table[64] = {
    13, 10, 18, 15, 12,  12,   8,   5,
    11, 13, 13, 11, -3,   3,   8,   3,
     7,  7,  7,  5,  4,  -3,  -5,  -3,
     4,  3, 13,  1,  2,   1,  -1,   2,
     3,  5,  8,  4, -5,  -6,  -8, -11,
    -4,  0, -5, -1, -7, -12,  -8, -16,
    -6, -6,  0,  2, -9,  -9, -11,  -3,
    -9,  2,  3, -1, -5, -13,   4, -20,
};

int mg_queen_table[64] = {
    -28,   0,  29,  12,  59,  44,  43,  45,
    -24, -39,  -5,   1, -16,  57,  28,  54,
    -13, -17,   7,   8,  29,  56,  47,  57,
    -27, -27, -16, -16,  -1,  17,  -2,   1,
     -9, -26,  -9, -10,  -2,  -4,   3,  -3,
    -14,   2, -11,  -2,  -5,   2,  14,   5,
    -35,  -8,  11,   2,   8,  15,  -3,   1,
     -1, -18,  -9,  10, -15, -25, -31, -50,
};

int eg_queen_table[64] = {
     -9,  22,  22,  27,  27,  19,  10,  20,
    -17,  20,  32,  41,  58,  25,  30,   0,
    -20,   6,   9,  49,  47,  35,  19,   9,
      3,  22,  24,  45,  57,  40,  57,  36,
    -18,  28,  19,  47,  31,  34,  39,  23,
    -16, -27,  15,   6,   9,  17,  10,   5,
    -22, -23, -30, -16, -16, -23, -36, -32,
    -33, -28, -22, -43,  -5, -32, -20, -41,
};

int mg_king_table[64] = {
    -65,  23,  16, -15, -56, -34,   2,  13,
     29,  -1, -20,  -7,  -8,  -4, -38, -29,
     -9,  24,   2, -16, -20,   6,  22, -22,
    -17, -20, -12, -27, -30, -25, -14, -36,
    -49,  -1, -27, -39, -46, -44, -33, -51,
    -14, -14, -22, -46, -44, -30, -15, -27,
      1,   7,  -8, -64, -43, -16,   9,   8,
    -15,  36,  12, -54,   8, -28,  24,  14,
};

int eg_king_table[64] = {
    -74, -35, -18, -18, -11,  15,   4, -17,
    -12,  17,  14,  17,  17,  38,  23,  11,
     10,  17,  23,  15,  20,  45,  44,  13,
     -8,  22,  24,  27,  26,  33,  26,   3,
    -18,  -4,  21,  24,  27,  23,   9, -11,
    -19,  -3,  11,  21,  23,  16,   7,  -9,
    -27, -11,   4,  13,  14,   4,  -5, -17,
    -53, -34, -21, -11, -28, -14, -24, -43
};

int* mg_pesto_table[6] =
{
    mg_pawn_table,
    mg_knight_table,
    mg_bishop_table,
    mg_rook_table,
    mg_queen_table,
    mg_king_table
};

int* eg_pesto_table[6] =
{
    eg_pawn_table,
    eg_knight_table,
    eg_bishop_table,
    eg_rook_table,
    eg_queen_table,
    eg_king_table
};


const int flip_table[64] = {
    56, 57, 58, 59, 60, 61, 62, 63,
    48, 49, 50, 51, 52, 53, 54, 55,
    40, 41, 42, 43, 44, 45, 46, 47,
    32, 33, 34, 35, 36, 37, 38, 39,
    24, 25, 26, 27, 28, 29, 30, 31,
    16, 17, 18, 19, 20, 21, 22, 23,
     8,  9, 10, 11, 12, 13, 14, 15,
     0,  1,  2,  3,  4,  5,  6,  7,
};

static int mvv_lva[12][12] = {
    105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
   104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
   103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
   102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
   101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
   100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600,

   105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
   104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
   103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
   102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
   101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
   100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600
};

static int killer_moves[2][64];
static int move_scores[128][256];
static int ply = 0;
static int history_moves[12][64];

static int pv_length[64];
static int pv_table [64][64];

static int follow_pv, score_pv;
static int hash_move_found;
static int hash_move;

static int score_move(int move) {
    int score = 0;
    if (hash_move_found)
    {
        if (hash_move == move)
        {
            hash_move_found = 0;
            return 30000;
        }
    }
    if (score_pv)
    {
        if (pv_table[0][ply] == move)
        {
            score_pv = 0;
            return 20000;
        }
    }
//    if (get_move_capture(move))
//    {
//        int target_piece = current_board->piece_at[get_move_target(move)];
//        score += mvv_lva[get_move_piece(move)][target_piece] + 10000;
//    }
    else {
        if (killer_moves[0][ply] == move) return 9000;
        else if (killer_moves[1][ply] == move) return 8000;
        else return history_moves[get_move_piece(move)][get_move_target(move)];
    }

    return score;
}

static void score_moves(int* scores,  MoveList& move_list) {
    for (int i = 0; i < move_list.count; i++) {
        scores[i] = score_move(move_list.moves[i]);
    }
}

const int mg_value[12] = { 82, 337, 365, 477, 1025,  0, 82, 337, 365, 477, 1025,  0};
const int eg_value[12] = { 94, 281, 297, 512,  936,  0, 94, 281, 297, 512,  936,  0};
const int bonus[64] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    60, 60, 60, 60, 60, 60, 60, 60,
    50, 50, 50, 50, 50, 50, 50, 50,
    40, 40, 40, 40, 40, 40, 40, 40,
    25, 25, 25, 25, 25, 25, 25, 25,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
};
const int piece_count = 32;
int gamephaseInc[12] = {0,1,1,2,4,0,0,1,1,2,4,0};
template<Side Us>
static inline int evaluate(Board& board)
{
    int score_mg = 0;
    int score_eg = 0;
    int phase = 0;
    for (int bb_piece = P; bb_piece <= k; bb_piece++)
    {
        U64 bitboard = board.bitboards[bb_piece];

        while (bitboard)
        {
            int square = get_ls1b_index(bitboard);
            pop_bit(bitboard, square);

            if (bb_piece <= 5) {
                score_mg += mg_pesto_table[bb_piece][square] + mg_value[bb_piece];
                score_eg += eg_pesto_table[bb_piece][square] + eg_value[bb_piece];
            }
            else {
                // Black pieces
                score_mg -= mg_pesto_table[bb_piece - 6][flip_table[square]] + mg_value[bb_piece];
                score_eg -= eg_pesto_table[bb_piece - 6][flip_table[square]] + eg_value[bb_piece];
            }
            phase += gamephaseInc[bb_piece];
        }
    }

    int mgPhase = phase;
    if (mgPhase > 24) mgPhase = 24;
    int egPhase = 24 - mgPhase;
    int score = (score_mg * mgPhase + score_eg * egPhase) / 24;

    if constexpr (Us == Side::WHITE) {
        return score;
    }
    else {
        return -score;

    }
}

static U64 nodes = 0llu;
static inline void enable_pv_scoring(MoveList& move_list)
{
    // disable following PV
    follow_pv = 0;

    // loop over the moves within a move list
    for (int count = 0; count < move_list.count; count++)
    {
        // make sure we hit PV move
        if (pv_table[0][ply] == move_list.moves[count])
        {
            // enable move scoring
            score_pv = 1;

            // enable following PV
            follow_pv = 1;
        }
    }
}

static int move_compare(const void* a, const void* b) {
    return score_move(*(int*)b) - score_move(*(int*)a);
}

static float flat_board[12][8][8] = {0};

static void bitboards_to_array(Board* board, float one_code[12][8][8]) {
    for (int piece = 0; piece < 12; piece++) {
        for (int square = 0; square < 64; square++) {
            int row = square / 8;  // Номер строки (0-7)
            int col = square % 8;  // Номер столбца (0-7)

            // Проверяем, установлен ли бит на позиции square
            one_code[piece][row][col] = (board->bitboards[piece] >> square) & 1;
        }
    }
}

static void bitboards_to_array_reverse(Board* board, float one_code[12][8][8]) {
    for (int piece = 0; piece < 6; piece++) {
        // Обработка белых фигур
        for (int square = 0; square < 64; square++) {
            int row = square / 8;  // Номер строки (0-7)
            int col = square % 8;  // Номер столбца (0-7)

            // Проверяем, установлен ли бит на позиции square для белой фигуры
            one_code[piece + 6][row][col] = (board->bitboards[piece] >> square) & 1;
        }
    }

    for (int piece = 6; piece < 12; piece++) {
        // Обработка черных фигур
        for (int square = 0; square < 64; square++) {
            int row = square / 8;  // Номер строки (0-7)
            int col = square % 8;  // Номер столбца (0-7)

            // Проверяем, установлен ли бит на позиции square для черной фигуры
            one_code[piece - 6][row][col] = (board->bitboards[piece] >> square) & 1;
        }
    }
}

//static inline int is_repetition(Board* board)
//{
//    for (int index = 0; index < board.repetition_index; index++)
//    {
//        if (board->repetition_table[index] == board->hash_key)
//        {
//            return 1;
//        }
//    }
//
//    return 0;
//}
template<Side Us>
static int quiescence(Board& board, int alpha, int beta)
{
  constexpr Side Them = (Us == Side::WHITE) ? Side::BLACK : Side::WHITE;
    if((nodes & 2047) == 0)
        Console::communicate();
    nodes++;
//    if (ply && is_repetition(board))
//        return 0;

    int evaluation = evaluate<Us>(board);

    if (ply > 128 - 65) return evaluation;

    if (evaluation >= beta)
    {
        return beta;
    }
    if (evaluation > alpha)
    {
        alpha = evaluation;
    }

    MoveList move_list;// = &move_buffer[ply+64];
    move_list.count = 0;
    generate_moves_internal<Us>(board, move_list);

    //score_moves(move_scores[ply], move_list);
    //SortState state;
    //init_sort_state(&state, move_list->moves, move_scores[ply], move_list->count);
    //qsort(move_list->moves, move_list->count, sizeof(int), move_compare);
    for (int count = 0; count < move_list.count; count++)
    {
        ply++;

        int move = move_list.moves[count];//get_sorted_move(&state);//
        if (!get_move_capture(move)) {
            ply--;
            continue;
        }
        Board copy = board;
        make_move<Us>(copy, move);
        int score = -quiescence<Them>(copy, -beta, -alpha);
        ply--;

        if (Console::stopped == true) return 0;


        if (score >= beta)
        {
            return beta;
        }

        if (score > alpha)
        {
            alpha = score;
        }
    }
    return alpha;
}

const int R_Prune_Depth = 2;
template<Side Us>
int negamax(Board& board, int alpha, int beta, int depth) {
    constexpr Side Them = (Us == Side::WHITE) ? Side::BLACK : Side::WHITE;

    int score;
    //int hash_type = hash_type_alpha;
    hash_move = 0;
    hash_move_found = 0;
    pv_length[ply] = ply;

//    if (ply)
//    {
//        if (is_repetition(board))
//        {
//            return 0;
//        }
//        TT* hash_entry = read_hash_entry(board, alpha, beta, depth);
//        if (hash_entry != NULL && hash_entry->hash == board->hash_key) {
//            if (hash_entry->depth >= depth)
//            {
//                int adj_score = hash_entry->value;
//
//                if (adj_score < -48000) adj_score += ply;
//                if (adj_score > 48000) adj_score -= ply;
//
//                if (hash_entry->type == hash_type_exact)
//                    return adj_score;
//                else if (hash_entry->type == hash_type_alpha && adj_score <= alpha)
//                    return alpha;
//                else if (hash_entry->type == hash_type_beta && adj_score >= beta)
//                    return beta;
//                if (hash_entry->move != 0)
//                {
//                    hash_move_found = 1;
//                    hash_move = hash_entry->move;
//                }
//            }
//        }
//    }

    if((nodes & 2047 ) == 0)
        Console::communicate();

    if (ply > 128 - 1) return evaluate<Us>(board);
    if (depth == 0) return quiescence<Us>(board, alpha, beta);
    int king = get_ls1b_index(board.bitboards[board.side_to_move == Side::WHITE ? K : k]);

    int in_check = is_square_attacked<Them>(board, king);

//    if (depth >= 3 && in_check == 0 && ply)
//    {
//        ply++;
//        board->repetition_index++;
//        board->repetition_table[board->repetition_index] = board->hash_key;
//
//        Board copy = *board;
//        make_null_move(&copy);
//        score = -negamax(&copy, -beta, -beta + 1, depth - 1 - R_Prune_Depth);
//
//        board->repetition_index--;
//        ply--;
//
//        if (stopped == 1) return 0;
//
//        if (score >= beta)
//            return beta;
//    }


    MoveList move_list;
    move_list.count = 0;
    generate_moves_internal<Us>(board, move_list);


    if (move_list.count == 0) {
        if (in_check) return -mate_value + ply;
        return 0;
    }
    if (follow_pv) enable_pv_scoring(move_list);

//    score_moves(move_scores[ply], move_list);
//    SortState state;
//    init_sort_state(&state, move_list->moves, move_scores[ply], move_list->count);
    int bestMove = 0;

    for (int count = 0; count < move_list.count; count++)
    {
        int move = move_list.moves[count];//get_sorted_move(&state);
        ply++;
        //board->repetition_index++;
        //board->repetition_table[board->repetition_index] = board->hash_key;
        Board copy = board;
        make_move<Us>(copy, move);

        if (count == 0)
            score = -negamax<Them>(copy,-beta, -alpha, depth - 1);
        else
        {
            if (
                count >= 4 &&
                depth >= 3 &&
                in_check == 0 &&
                get_move_capture(move) == 0 &&
                get_move_promoted(move) == 0
            ) {

                int R = (int)(log(depth) * log(move_list.count) / 2.0);
                R = R > 0 ? R : 1;

                score = -negamax<Them>(copy, -alpha - 1, -alpha, depth - R - 1);
            }
            else score = alpha + 1;

            if(score > alpha)
            {
                score = -negamax<Them>(copy,-alpha - 1, -alpha, depth-1);

                if((score > alpha) && (score < beta))
                    score = -negamax<Them>(copy,-beta, -alpha, depth-1);
            }
        }
//        unmake_move(board, ply);
//        board->repetition_index--;
        ply--;

        if (Console::stopped == 1) return 0;

        if (score > alpha)
        {
            //hash_type = hash_type_exact;

            if (get_move_capture(move) == 0)
                history_moves[get_move_piece(move)][get_move_target(move)] += depth;

            alpha = score;

            pv_table[ply][ply] = move;
            bestMove = move;

            for (int next_ply = ply + 1; next_ply < pv_length[ply + 1]; next_ply++)
                pv_table[ply][next_ply] = pv_table[ply + 1][next_ply];

            pv_length[ply] = pv_length[ply + 1];

            if (score >= beta)
            {
                //write_hash_entry(board, beta, depth, hash_type_beta, move, ply);

                if (get_move_capture(move) == 0)
                {
                    killer_moves[1][ply] = killer_moves[0][ply];
                    killer_moves[0][ply] = move;
                }

                return beta;
            }
        }
    }

    //write_hash_entry(board, alpha, depth, hash_type, bestMove, ply);

    return alpha;
}

void search_position(Board& board, int depth) {
    nodes = 0;
    follow_pv = 0;
    Console::stopped = 0;
    Side side = board.side_to_move;
    int score = 0;

    memset(history_moves, 0, sizeof(history_moves));
    memset(killer_moves, 0, sizeof(killer_moves));
    memset(pv_table, 0, sizeof(pv_table));
    memset(pv_length, 0, sizeof(pv_length));

    for (int current_depth = 1; current_depth <= depth; current_depth++) {
        if(Console::stopped == 1) break;
        follow_pv = 1;
        if (side == Side::WHITE)
            score = negamax<Side::WHITE>(board, -infinity, infinity, current_depth);
        else
            score = negamax<Side::BLACK>(board, -infinity, infinity, current_depth);
        if(Console::stopped == 1) break;
        printf("info score cp %d depth %d nodes %llu time %d old_time %d pv ", score, current_depth, nodes, Console::get_time_ms() - starttime, Console::stoptime);
        for (int count = 0; count < pv_length[0]; count++) {
            print_move(pv_table[0][count]);
            printf(" ");
        }
        printf("\n");

    }
    if (side == Side::WHITE)
        make_move<Side::WHITE>(board, pv_table[0][0]);
    else
        make_move<Side::BLACK>(board, pv_table[0][0]);

    printf("bestmove ");
    print_move(pv_table[0][0]);
    printf("\n");
}

int parse_move(Board& board, char* move_string)
{
  MoveList move_list;
    if (board.side_to_move == Side::WHITE) {
        generate_moves_internal<Side::WHITE>(board, move_list);
    } else {
        generate_moves_internal<Side::BLACK>(board, move_list);
    }

    int source_square = (move_string[0] - 'a') + (8 - (move_string[1] - '0')) * 8;

    int target_square = (move_string[2] - 'a') + (8 - (move_string[3] - '0')) * 8;

    for (int move_count = 0; move_count < move_list.count; move_count++)
    {
        int move = move_list.moves[move_count];

        if (source_square == get_move_source(move) && target_square == get_move_target(move))
        {
            int promoted_piece = get_move_promoted(move);

            if (promoted_piece)
            {
                if ((promoted_piece == Q || promoted_piece == q) && move_string[4] == 'q')
                    return move;

                else if ((promoted_piece == R || promoted_piece == r) && move_string[4] == 'r')
                    return move;

                else if ((promoted_piece == B || promoted_piece == b) && move_string[4] == 'b')
                    return move;

                else if ((promoted_piece == N || promoted_piece == n) && move_string[4] == 'n')
                    return move;

                continue;
            }

            return move;
        }
    }

    return 0;
}

void parse_position(Board& board, char* command) {
    command += 9;

    char *current_char = command;

    if (strncmp(command, "startpos", 8) == 0)
            parse_fen(start_position, board);

    else
    {
        current_char = strstr(command, "fen");

        if (current_char == NULL)
                parse_fen(start_position, board);

        else
        {
            current_char += 4;

            parse_fen(current_char, board);
        }
    }

    current_char = strstr(command, "moves");

    if (current_char != NULL)
    {
        current_char += 6;

        while(*current_char)
        {
            int move = parse_move(board, current_char);

            if (move == 0) {
                break;
            }

            //game->board.repetition_index++;
            //game->board.repetition_table[game->board.repetition_index] = game->board.hash_key;

            if (board.side_to_move == Side::WHITE) {
                make_move<Side::WHITE>(board, move);
            }
            else {
                make_move<Side::BLACK>(board, move);
            }


            while (*current_char && *current_char != ' ') current_char++;

            current_char++;
        }

        printf("%s\n", current_char);
    }
}

void parse_go(Board& board, char* command) {
    int depth = -1;

    // init argument
    char *argument = NULL;

    // infinite search
    if ((argument = strstr(command,"infinite"))) {}

    // match UCI "binc" command
    if ((argument = strstr(command,"binc")) && board.side_to_move == Side::BLACK)
        // parse black time increment
        inc = atoi(argument + 5);

    // match UCI "winc" command
    if ((argument = strstr(command,"winc")) && board.side_to_move == Side::WHITE)
        // parse white time increment
        inc = atoi(argument + 5);

    // match UCI "wtime" command
    if ((argument = strstr(command,"wtime")) && board.side_to_move == Side::WHITE)
        // parse white time limit
        time_uci = atoi(argument + 6);

    // match UCI "btime" command
    if ((argument = strstr(command,"btime")) && board.side_to_move == Side::BLACK)
        // parse black time limit
        time_uci = atoi(argument + 6);

    // match UCI "movestogo" command
    if ((argument = strstr(command,"movestogo")))
        // parse number of moves to go
        movestogo = atoi(argument + 10);

    // match UCI "movetime" command
    if ((argument = strstr(command,"movetime")))
        // parse amount of time allowed to spend to make a move
        movetime = atoi(argument + 9);

    // match UCI "depth" command
    if ((argument = strstr(command,"depth")))
        // parse search depth
        depth = atoi(argument + 6);

    // if move time is not available
    if(movetime != -1)
    {
        // set time equal to move time
        time_uci = movetime;

        // set moves to go to 1
        movestogo = 1;
    }

    // init start time
    starttime = Console::get_time_ms();
    depth = depth;

    // if time control is available
    if(time_uci != -1)
    {
        // flag we're playing with time control
        Console::timeset = true;

        // set up timing
        time_uci /= movestogo;
        time_uci -= 50;
        Console::stoptime = starttime + time_uci + inc;//
    }

    // if depth is not available
    if(depth == -1)
        // set depth to 64 plies (takes ages to complete...)
        depth = 64;

    // print debug info
    printf("time:%d start:%d stop:%d depth:%d timeset:%d\n",
    time_uci, starttime, Console::stoptime, depth, Console::timeset);

    // search position
    search_position(board, depth);
}

// main UCI loop
static void uci_loop()
{
    Board board;
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    // define user / GUI input buffer
    char input[2000];

    // print engine info
    printf("id name BBC\n");
    printf("id name Code Monkey King\n");
    printf("uciok\n");

    // main loop
    while (true)
    {
        // reset user /GUI input
        memset(input, 0, sizeof(input));

        // make sure output reaches the GUI
        fflush(stdout);

        // get user / GUI input
        if (!fgets(input, 2000, stdin))
            // continue the loop
            continue;

        // make sure input is available
        if (input[0] == '\n')
            // continue the loop
            continue;

        // parse UCI "isready" command
        if (strncmp(input, "isready", 7) == 0)
        {
            printf("readyok\n");
            continue;
        }

        // parse UCI "position" command
        else if (strncmp(input, "position", 8) == 0)
            // call parse position function
            parse_position(board, input);

        // parse UCI "ucinewgame" command
        else if (strncmp(input, "ucinewgame", 10) == 0)
        {
            board = {0};
            parse_position(board, "position startpos");

            //clear_transposition_table();
        }

        // parse UCI "go" command
        else if (strncmp(input, "go", 2) == 0)
            // call parse go function
            parse_go(board, input);

        // parse UCI "quit" command
        else if (strncmp(input, "quit", 4) == 0)
            // quit from the chess engine program execution
            break;

        // parse UCI "uci" command
        else if (strncmp(input, "uci", 3) == 0)
        {
            // print engine info
            printf("id name BBC\n");
            printf("id name Code Monkey King\n");
            printf("uciok\n");
        }
    }
}

// Функция для печати массива 8x8
void print_board(int board[8][8]) {
    for (int row = 7; row >= 0; row--) {  // Печатаем сверху вниз (как шахматную доску)
        for (int col = 0; col < 8; col++) {
            printf("%d ", board[row][col]);
        }
        printf("\n");
    }
    printf("\n");
}
}