#pragma once
#include <cstdint>
#include <cstring>
#include <array>
#include <iostream>
#include "Board.hpp"


namespace Perft {
    static inline constexpr int depths[] = {
        5, 5, 6, 5, 5, 4, 5, 4, 6, 6, 6, 7, 4, 5, 6, 5, 6, 6, 10, 7, 6, 5, 4, 5, 4, 6, 6, 9, 4, 5
    };
    static inline constexpr unsigned long long expectedNodes[] = {
        4865609ULL, 5617302ULL, 11030083ULL, 15587335ULL, 89941194ULL,
        3894594ULL, 193690690ULL, 497787ULL, 1134888ULL, 1440467ULL,
        661072ULL, 15594314ULL, 1274206ULL, 58773923ULL, 3821001ULL,
        1004658ULL, 217342ULL, 92683ULL, 5966690ULL, 567584ULL,
        3114998ULL, 42761834ULL, 3050662ULL, 10574719ULL, 6871272ULL,
        71179139ULL, 28859283ULL, 7618365ULL, 28181ULL, 6323457ULL
    };

    static inline const std::string fens[] = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "2b1b3/1r1P4/3K3p/1p6/2p5/6k1/1P3p2/4B3 w - - 0 42",
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -",
        "r3k2r/pp3pp1/PN1pr1p1/4p1P1/4P3/3P4/P1P2PP1/R3K2R w KQkq - 4 4",
        "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
        "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -",
        "r3k1nr/p2pp1pp/b1n1P1P1/1BK1Pp1q/8/8/2PP1PPP/6N1 w kq - 0 1",
        "3k4/3p4/8/K1P4r/8/8/8/8 b - - 0 1",
        "8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1",
        "5k2/8/8/8/8/8/8/4K2R w K - 0 1",
        "3k4/8/8/8/8/8/8/R3K3 w Q - 0 1",
        "r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1",
        "r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0 1",
        "2K2r2/4P3/8/8/8/8/8/3k4 w - - 0 1",
        "8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0 1",
        "4k3/1P6/8/8/8/8/K7/8 w - - 0 1",
        "8/P1k5/K7/8/8/8/8/8 w - - 0 1",
        "K1k5/8/P7/8/8/8/8/8 w - - 0 1",
        "8/k1P5/8/1K6/8/8/8/8 w - - 0 1",
        "8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1",
        "r1bq2r1/1pppkppp/1b3n2/pP1PP3/2n5/2P5/P3QPPP/RNB1K2R w KQ a6 0 12",
        "r3k2r/pppqbppp/3p1n1B/1N2p3/1nB1P3/3P3b/PPPQNPPP/R3K2R w KQkq - 11 10",
        "4k2r/1pp1n2p/6N1/1K1P2r1/4P3/P5P1/1Pp4P/R7 w k - 0 6",
        "1Bb3BN/R2Pk2r/1Q5B/4q2R/2bN4/4Q1BK/1p6/1bq1R1rb w - - 0 1",
        "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1",
        "8/PPPk4/8/8/8/8/4Kppp/8 b - - 0 1",
        "8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1",
        "3r4/2p1p3/8/1P1P1P2/3K4/5k2/8/8 b - - 0 1",
        "8/1p4p1/8/q1PK1P1r/3p1k2/8/4P3/4Q3 b - - 0 1"
    };

    void divide(const ChessBoard::Board &board, int depth) {
        ChessBoard::MoveList move_list;
        move_list.count = 0;
        if (board.side_to_move == ChessBoard::Side::WHITE) {
            ChessBoard::generate_moves<ChessBoard::Side::WHITE>(board, move_list);
        } else {
            ChessBoard::generate_moves<ChessBoard::Side::BLACK>(board, move_list);
        }

        unsigned long long total_nodes = 0;

        for (int i = 0; i < move_list.count; i++) {
            int move = move_list.moves[i];
            ChessBoard::Board copy = board;
            if (board.side_to_move == ChessBoard::Side::WHITE) {
                make_move<ChessBoard::Side::WHITE>(copy, move);
            } else {
                make_move<ChessBoard::Side::BLACK>(copy, move);
            }
            unsigned long long nodes = perft(copy, depth - 1);

            total_nodes += nodes;

            printf("%s: %llu\n", ChessBoard::get_move_string(move), nodes);
        }

        printf("\nTotal nodes: %llu\n", total_nodes);
    }

    void test_perft() {
        printf("\n===== PERFT TESTS =====\n");

        int passed = 0;
        int total = 0;
        double totalTime = 0;

        for (size_t i = 0; i < std::size(fens); i++) {
            total++;
            ChessBoard::Board board;
            parse_fen(fens[i].c_str(), board);

            const clock_t start = clock();
            const unsigned long long nodes = perft(board, depths[i]);
            const clock_t end = clock();

            const double time_taken = static_cast<double>(end - start) / CLOCKS_PER_SEC;
            totalTime += time_taken;
            char message[256];
            sprintf(message, "Position %llu (depth %d): %llu nodes in %.3f sec (%.0f NPS)",
                    i, depths[i], nodes, time_taken, nodes / (time_taken + 0.001));

            if (nodes == expectedNodes[i]) {
                passed++;
                printf("PASS: %s\n", message);
            } else {
                printf("FAIL: %s - Expected: %llu\n", message, expectedNodes[i]);
            }
        }

        printf("\n%d/%d perft tests passed\n", passed, total);
        printf("\n%f s\n", totalTime);
    }
}
