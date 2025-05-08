#pragma once
#include "Board.hpp"
#include <chrono>
#include <string>
#include <vector>
#include <algorithm> // For std::max
#include <iostream> // For std::cout, std::cerr, std::endl
#include <sstream>  // For std::stringstream

using namespace ChessBoard;

namespace Search {
    static constexpr int material_score[12] = {
        100, // white pawn score
        300, // white knight scrore
        350, // white bishop score
        500, // white rook score
        1000, // white queen score
        -100, // black pawn score
        -300, // black knight scrore
        -350, // black bishop score
        -500, // black rook score
        -1000, // black queen score
    };

    static constexpr int mg_pawn_table[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        98, 134, 61, 95, 68, 126, 34, -11,
        -6, 7, 26, 31, 65, 56, 25, -20,
        -14, 13, 6, 21, 23, 12, 17, -23,
        -27, -2, -5, 12, 17, 6, 10, -25,
        -26, -4, -4, -10, 3, 3, 33, -12,
        -35, -1, -20, -23, -15, 24, 38, -22,
        0, 0, 0, 0, 0, 0, 0, 0,
    };

    static constexpr int eg_pawn_table[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        178, 173, 158, 134, 147, 132, 165, 187,
        94, 100, 85, 67, 56, 53, 82, 84,
        32, 24, 13, 5, -2, 4, 17, 17,
        13, 9, -3, -7, -7, -8, 3, -1,
        4, 7, -6, 1, 0, -5, -1, -8,
        13, 8, 8, 10, 13, 0, 2, -7,
        0, 0, 0, 0, 0, 0, 0, 0,
    };

    static constexpr int mg_knight_table[64] = {
        -167, -89, -34, -49, 61, -97, -15, -107,
        -73, -41, 72, 36, 23, 62, 7, -17,
        -47, 60, 37, 65, 84, 129, 73, 44,
        -9, 17, 19, 53, 37, 69, 18, 22,
        -13, 4, 16, 13, 28, 19, 21, -8,
        -23, -9, 12, 10, 19, 17, 25, -16,
        -29, -53, -12, -3, -1, 18, -14, -19,
        -105, -21, -58, -33, -17, -28, -19, -23,
    };

    static constexpr int eg_knight_table[64] = {
        -58, -38, -13, -28, -31, -27, -63, -99,
        -25, -8, -25, -2, -9, -25, -24, -52,
        -24, -20, 10, 9, -1, -9, -19, -41,
        -17, 3, 22, 22, 22, 11, 8, -18,
        -18, -6, 16, 25, 16, 17, 4, -18,
        -23, -3, -1, 15, 10, -3, -20, -22,
        -42, -20, -10, -5, -2, -20, -23, -44,
        -29, -51, -23, -15, -22, -18, -50, -64,
    };

    static constexpr int mg_bishop_table[64] = {
        -29, 4, -82, -37, -25, -42, 7, -8,
        -26, 16, -18, -13, 30, 59, 18, -47,
        -16, 37, 43, 40, 35, 50, 37, -2,
        -4, 5, 19, 50, 37, 37, 7, -2,
        -6, 13, 13, 26, 34, 12, 10, 4,
        0, 15, 15, 15, 14, 27, 18, 10,
        4, 15, 16, 0, 7, 21, 33, 1,
        -33, -3, -14, -21, -13, -12, -39, -21,
    };

    static constexpr int eg_bishop_table[64] = {
        -14, -21, -11, -8, -7, -9, -17, -24,
        -8, -4, 7, -12, -3, -13, -4, -14,
        2, -8, 0, -1, -2, 6, 0, 4,
        -3, 9, 12, 9, 14, 10, 3, 2,
        -6, 3, 13, 19, 7, 10, -3, -9,
        -12, -3, 8, 10, 13, 3, -7, -15,
        -14, -18, -7, -1, 4, -9, -15, -27,
        -23, -9, -23, -5, -9, -16, -5, -17,
    };

    static constexpr int mg_rook_table[64] = {
        32, 42, 32, 51, 63, 9, 31, 43,
        27, 32, 58, 62, 80, 67, 26, 44,
        -5, 19, 26, 36, 17, 45, 61, 16,
        -24, -11, 7, 26, 24, 35, -8, -20,
        -36, -26, -12, -1, 9, -7, 6, -23,
        -45, -25, -16, -17, 3, 0, -5, -33,
        -44, -16, -20, -9, -1, 11, -6, -71,
        -19, -13, 1, 17, 16, 7, -37, -26,
    };

    static constexpr int eg_rook_table[64] = {
        13, 10, 18, 15, 12, 12, 8, 5,
        11, 13, 13, 11, -3, 3, 8, 3,
        7, 7, 7, 5, 4, -3, -5, -3,
        4, 3, 13, 1, 2, 1, -1, 2,
        3, 5, 8, 4, -5, -6, -8, -11,
        -4, 0, -5, -1, -7, -12, -8, -16,
        -6, -6, 0, 2, -9, -9, -11, -3,
        -9, 2, 3, -1, -5, -13, 4, -20,
    };

    static constexpr int mg_queen_table[64] = {
        -28, 0, 29, 12, 59, 44, 43, 45,
        -24, -39, -5, 1, -16, 57, 28, 54,
        -13, -17, 7, 8, 29, 56, 47, 57,
        -27, -27, -16, -16, -1, 17, -2, 1,
        -9, -26, -9, -10, -2, -4, 3, -3,
        -14, 2, -11, -2, -5, 2, 14, 5,
        -35, -8, 11, 2, 8, 15, -3, 1,
        -1, -18, -9, 10, -15, -25, -31, -50,
    };

    static constexpr int eg_queen_table[64] = {
        -9, 22, 22, 27, 27, 19, 10, 20,
        -17, 20, 32, 41, 58, 25, 30, 0,
        -20, 6, 9, 49, 47, 35, 19, 9,
        3, 22, 24, 45, 57, 40, 57, 36,
        -18, 28, 19, 47, 31, 34, 39, 23,
        -16, -27, 15, 6, 9, 17, 10, 5,
        -22, -23, -30, -16, -16, -23, -36, -32,
        -33, -28, -22, -43, -5, -32, -20, -41,
    };

    static constexpr int mg_king_table[64] = {
        -65, 23, 16, -15, -56, -34, 2, 13,
        29, -1, -20, -7, -8, -4, -38, -29,
        -9, 24, 2, -16, -20, 6, 22, -22,
        -17, -20, -12, -27, -30, -25, -14, -36,
        -49, -1, -27, -39, -46, -44, -33, -51,
        -14, -14, -22, -46, -44, -30, -15, -27,
        1, 7, -8, -64, -43, -16, 9, 8,
        -15, 36, 12, -54, 8, -28, 24, 14,
    };

    static constexpr int eg_king_table[64] = {
        -74, -35, -18, -18, -11, 15, 4, -17,
        -12, 17, 14, 17, 17, 38, 23, 11,
        10, 17, 23, 15, 20, 45, 44, 13,
        -8, 22, 24, 27, 26, 33, 26, 3,
        -18, -4, 21, 24, 27, 23, 9, -11,
        -19, -3, 11, 21, 23, 16, 7, -9,
        -27, -11, 4, 13, 14, 4, -5, -17,
        -53, -34, -21, -11, -28, -14, -24, -43
    };

    static const int *mg_pesto_table[6] =
    {
        mg_pawn_table,
        mg_knight_table,
        mg_bishop_table,
        mg_rook_table,
        mg_queen_table,
        mg_king_table
    };

    static const int *eg_pesto_table[6] =
    {
        eg_pawn_table,
        eg_knight_table,
        eg_bishop_table,
        eg_rook_table,
        eg_queen_table,
        eg_king_table
    };


    static constexpr int flip_table[64] = {
        56, 57, 58, 59, 60, 61, 62, 63,
        48, 49, 50, 51, 52, 53, 54, 55,
        40, 41, 42, 43, 44, 45, 46, 47,
        32, 33, 34, 35, 36, 37, 38, 39,
        24, 25, 26, 27, 28, 29, 30, 31,
        16, 17, 18, 19, 20, 21, 22, 23,
        8, 9, 10, 11, 12, 13, 14, 15,
        0, 1, 2, 3, 4, 5, 6, 7,
    };

    static constexpr int mvv_lva[12][12] = {
        105, 205, 305, 405, 505, 605, 105, 205, 305, 405, 505, 605,
        104, 204, 304, 404, 504, 604, 104, 204, 304, 404, 504, 604,
        103, 203, 303, 403, 503, 603, 103, 203, 303, 403, 503, 603,
        102, 202, 302, 402, 502, 602, 102, 202, 302, 402, 502, 602,
        101, 201, 301, 401, 501, 601, 101, 201, 301, 401, 501, 601,
        100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600,

        105, 205, 305, 405, 505, 605, 105, 205, 305, 405, 505, 605,
        104, 204, 304, 404, 504, 604, 104, 204, 304, 404, 504, 604,
        103, 203, 303, 403, 503, 603, 103, 203, 303, 403, 503, 603,
        102, 202, 302, 402, 502, 602, 102, 202, 302, 402, 502, 602,
        101, 201, 301, 401, 501, 601, 101, 201, 301, 401, 501, 601,
        100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600
    };
    static constexpr int mg_value[12] = { 82, 337, 365, 477, 1025,  0, 82, 337, 365, 477, 1025,  0};
    static constexpr int eg_value[12] = { 94, 281, 297, 512,  936,  0, 94, 281, 297, 512,  936,  0};
    static constexpr int piece_count = 32;
    static constexpr int gamephaseInc[12] = {0, 1, 1, 2, 4, 0, 0, 1, 1, 2, 4, 0};
#define infinity 50000
#define mate_value 49000
#define mate_score 48000
    static U64 nodes = 0ull;
    static inline int evaluate(Board& board) {
        nodes++;
        int score_mg = 0;
        int score_eg = 0;
        int phase = 0;
        for (int bb_piece = P; bb_piece <= k; bb_piece++) {
            U64 bitboard = board.bitboards[bb_piece];

            while (bitboard) {
                int square = get_ls1b_index(bitboard);
                pop_bit(bitboard, square);

                if (bb_piece <= 5) {
                    // White pieces
                    score_mg += mg_pesto_table[bb_piece][square] + mg_value[bb_piece];
                    score_eg += eg_pesto_table[bb_piece][square] + eg_value[bb_piece];
                } else {
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

        return (board.side_to_move == Side::WHITE) ? score : -score;
    }

    // --- Search (Negamax with Alpha-Beta Pruning - preparing structure) ---
    // Although not fully implementing alpha-beta yet, the structure allows it.
    // We add 'ply' for mate distance scoring.
    inline MoveList search_moves[128]; // Move lists for search recursion depth

    template<Side Us>
    int negamax(Board &board, int depth, int ply) {
        constexpr Side Them = (Us == Side::WHITE) ? Side::BLACK : Side::WHITE;
        // Added ply parameter
        if (depth <= 0) {
            return evaluate(board); // Return evaluation at leaf nodes
        }

        int max_score = -infinity; // Initialize with lowest possible score
        MoveList current_move_list = search_moves[ply]; // Use ply for move list index
        current_move_list.count = 0;

        generate_moves_internal<Us>(board, current_move_list);

        // Check for checkmate or stalemate
        if (current_move_list.count == 0) {
            U64 king_bb = board.bitboards[(board.side_to_move == Side::WHITE) ? K : k];
            if (!king_bb) return -infinity; // Should not happen - king missing? Treat as loss.
            int king_square = get_ls1b_index(king_bb);

            bool in_check = is_square_attacked<Them>(board, king_square);


            if (in_check) {
                // Checkmate: Return a high negative score, penalizing based on ply (faster mate is worse)
                return -mate_value + ply; // Mate value adjusted by ply
            } else {
                // Stalemate
                return 0;
            }
        }


        for (int i = 0; i < current_move_list.count; ++i) {
            Board next_board = board;
            Move move = current_move_list.moves[i];

            make_move<Us>(next_board, move);

            int score = -negamax<Them>(next_board, depth - 1, ply + 1); // Recursive call, negate result, increment ply

            if (score > max_score) {
                max_score = score;
            }
        }

        return max_score;
    }

    Move parse_move(Board &board, const std::string &move_string) {
        MoveList legal_moves;
        legal_moves.count = 0; // Important: Initialize count

        // Generate legal moves for the current side
        if (board.side_to_move == Side::WHITE) {
            generate_moves_internal<Side::WHITE>(board, legal_moves);
        } else {
            generate_moves_internal<Side::BLACK>(board, legal_moves);
        }

        // Iterate through legal moves and compare string representations
        for (int i = 0; i < legal_moves.count; ++i) {
            Move current_move = legal_moves.moves[i];
            if (get_move_string(current_move) == move_string) {
                return current_move; // Found the move
            }
        }

        return 0; // Move not found or illegal
    }

    template<Side Us>
    Move search_position(Board &board, int max_depth) {
        constexpr Side Them = (Us == Side::WHITE) ? Side::BLACK : Side::WHITE;
        Move overall_best_move = 0;
        int overall_best_score = -infinity;
        auto search_start_time = std::chrono::high_resolution_clock::now(); // For total time if needed later

        // Generate root moves once
        MoveList root_moves;
        root_moves.count = 0;
        generate_moves_internal<Us>(board, root_moves);


        if (root_moves.count == 0) {
            std::cout << "info string No legal moves found!" << std::endl;
            std::cout << "bestmove 0000" << std::endl; // UCI standard for no move
            std::cout.flush();
            return 0; // Return invalid move
        }

        // --- Iterative Deepening Loop ---
        for (int current_depth = 1; current_depth <= max_depth; ++current_depth) {
            nodes = 0; // Reset node count for this depth iteration
            int best_score_this_depth = -infinity;
            Move best_move_this_depth = 0; // Best move found at *this* specific depth
            auto start_time_depth = std::chrono::high_resolution_clock::now();

            // Future optimization: Order moves based on previous iteration's results (PV move first)

            // Iterate through all legal moves at the root
            for (int i = 0; i < root_moves.count; ++i) {
                Board next_board = board;
                Move current_move = root_moves.moves[i];

                make_move<Us>(next_board, current_move);


                // Start negamax search from this position (ply starts at 1)
                int score = -negamax<Them>(next_board, current_depth - 1, 1);

                // Check if this move gives a better score *for this depth*
                if (score > best_score_this_depth) {
                    best_score_this_depth = score;
                    best_move_this_depth = current_move;
                }
            } // End loop through root moves for current_depth

            // If we found a best move at this depth, update the overall best move
            // This ensures we always have the best move from the deepest fully completed search
            if (best_move_this_depth != 0) {
                overall_best_move = best_move_this_depth;
                overall_best_score = best_score_this_depth; // Store score from this depth too
            } else if (root_moves.count > 0) {
                // If somehow no move improved the score (e.g., all moves lead to mate),
                // keep the overall_best_move from the previous depth or pick the first legal move.
                // For robustness, ensure overall_best_move is at least set to *something* if moves exist.
                if (overall_best_move == 0) overall_best_move = root_moves.moves[0];
            }

            auto end_time_depth = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed_depth = end_time_depth - start_time_depth;
            auto elapsed_ms = static_cast<long long>(elapsed_depth.count());
            if (elapsed_ms == 0) elapsed_ms = 1;

            std::string score_uci;
            if (best_score_this_depth > mate_score) {
                int plies_to_mate = mate_value - best_score_this_depth + 1; // +1 because ply=1 means mate in 1 ply
                int moves_to_mate = (plies_to_mate + 1) / 2;
                score_uci = "mate " + std::to_string(moves_to_mate);
            } else if (best_score_this_depth < -mate_score) {
                int plies_to_mated = mate_value + best_score_this_depth + 1;
                int moves_to_mated = (plies_to_mated + 1) / 2;
                score_uci = "mate -" + std::to_string(moves_to_mated);
            } else {
                score_uci = "cp " + std::to_string(best_score_this_depth);
            }

            printf("info depth %d score %s nodes %llu time %lld pv %s\n",
                   current_depth,
                   score_uci.c_str(),
                   nodes,
                   elapsed_ms,
                   (overall_best_move != 0) ? get_move_string(overall_best_move) : "0000"
            );
            std::cout.flush();

            // Optional: Add time control checks here to break early if needed
            // e.g., if (elapsed_total_ms > time_limit) break;
        }


        if (overall_best_move == 0 && root_moves.count > 0) {
            overall_best_move = root_moves.moves[0];
            std::cout <<
                    "info string Warning: Search completed without finding an improving move? Using first legal move."
                    << std::endl;
            std::cout.flush();
        }

        std::cout << "bestmove " << get_move_string(overall_best_move) << std::endl;
        std::cout.flush();

        return overall_best_move;
    }

    // --- UCI Loop (Unchanged, but now calls the ID search) ---
    void uci_loop() {
        Board board;
        parse_fen(start_position, board);
        // Corrected Engine ID lines
        std::cout << "id name SimpleEngineID" << std::endl; // Choose a name
        std::cout << "id author YourName/CMK" << std::endl; // Your name or alias
        std::cout << "uciok" << std::endl;
        std::cout.flush(); // Make sure initial messages are sent

        std::string line;
        std::string token;

        while (std::getline(std::cin, line)) {
            std::stringstream ss(line);
            ss >> token;

            if (token == "uci") {
                std::cout << "id name SimpleEngineID" << std::endl;
                std::cout << "id author YourName/CMK" << std::endl;
                // Add any UCI options here if you implement them later
                std::cout << "uciok" << std::endl;
            } else if (token == "isready") {
                std::cout << "readyok" << std::endl;
            } else if (token == "ucinewgame") {
                parse_fen(start_position, board); // Reset board to start position
                // Clear any other necessary state (e.g., history, hash table if added later)
            } else if (token == "position") {
                std::string pos_type;
                ss >> pos_type; // Read "startpos" or "fen"

                if (pos_type == "startpos") {
                    parse_fen(start_position, board);
                    ss >> token; // Check if "moves" follows
                } else if (pos_type == "fen") {
                    std::string fen_part;
                    std::string current_fen;
                    // Read the 6 parts of the FEN string
                    for (int i = 0; i < 6; ++i) {
                        ss >> fen_part;
                        if (ss.fail()) {
                            // Check if read failed (e.g., incomplete FEN)
                            std::cerr << "info string Error: Incomplete FEN string." << std::endl;
                            goto next_command; // Skip processing this command further
                        }
                        current_fen += fen_part + (i == 5 ? "" : " ");
                    }
                    parse_fen(current_fen.c_str(), board);
                    ss >> token; // Check if "moves" follows
                } else {
                    std::cerr << "info string Error: Expected 'startpos' or 'fen' after 'position'" << std::endl;
                    goto next_command; // Skip to next command
                }


                // If the token read after position type is "moves", parse them
                // Need to check if the token was actually read before comparing
                if (!ss.fail() && token == "moves") {
                    std::string move_str;
                    while (ss >> move_str) {
                        // Read each subsequent move string
                        Move uci_move = parse_move(board, move_str);
                        if (uci_move != 0) {
                            // Apply the move to the board
                            if (board.side_to_move == Side::WHITE) {
                                make_move<Side::WHITE>(board, uci_move);
                            } else {
                                make_move<Side::BLACK>(board, uci_move);
                            }
                        } else {
                            std::cerr << "info string Error: Illegal move " << move_str <<
                                    " received in position command or move not found." << std::endl;
                            // Decide how to handle error: stop parsing moves?
                            // For robustness, stop processing further moves for this 'position' command.
                            break;
                        }
                    }
                }
                // else: No "moves" token found or finished processing moves. Board is set up.
            } else if (token == "go") {
                int depth = 5; // Default search depth if not specified
                // --- Parse 'go' parameters ---
                std::string go_param;
                int parsed_depth = -1; // Use -1 to indicate depth not specified

                while (ss >> go_param) {
                    if (go_param == "depth") {
                        if (!(ss >> parsed_depth)) {
                            std::cerr << "info string Error: Invalid value after depth." << std::endl;
                            parsed_depth = 6; // Fallback to default
                        }
                    } else if (go_param == "movetime") {
                        long long ms;
                        if (ss >> ms) {
                            // Basic time management: map movetime to depth?
                            // Very crude mapping: e.g., 1 sec -> depth 6, 5 sec -> depth 8 etc.
                            // For now, we'll prioritize explicit depth command.
                            if (parsed_depth == -1) {
                                // Only use if depth wasn't set
                                if (ms < 1000) depth = 5;
                                else if (ms < 5000) depth = 6;
                                else if (ms < 15000) depth = 7;
                                else depth = 8; // Or higher
                            }
                        }
                    }
                    // Add handling for wtime, btime, winc, binc later for more complex time management
                }

                if (parsed_depth != -1) {
                    // If depth was explicitly given, use it
                    depth = parsed_depth;
                }
                if (depth <= 0) depth = 1; // Ensure depth is at least 1

                std::cout << "info string Starting search with depth " << depth << std::endl;
                std::cout.flush();
                if (board.side_to_move == Side::WHITE) {
                    Search::search_position<Side::WHITE>(board, depth);
                } else {
                    Search::search_position<Side::BLACK>(board, depth);
                }
                 // Start the iterative deepening search
            } else if (token == "quit") {
                break; // Exit the loop
            } else if (token == "stop") {
                // Basic stop handling: In a real engine, this would set a flag
                // checked within the search loop to terminate early.
                // For now, we let the current depth finish.
                std::cout << "info string Stop command received (basic handling)" << std::endl;
                std::cout.flush();
                // The search will naturally stop after the current depth or max_depth
                // and print bestmove then.
            } else {
                // std::cout << "info string Unknown command: " << token << std::endl; // Optional: Log unknown commands
            }
        next_command:; // Label for goto on error
            std::cout.flush(); // Crucial: Ensure output is sent immediately to the GUI
        }
    }
} // namespace Search
