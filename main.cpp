#include "Board.hpp"
#include "Perft.hpp"
#include "Search.hpp"
#include "OldSearch.hpp"

void init() {
    Random::init_random_keys();
}

constexpr int debug = 3;

int main() {
    init();

    if (debug == 1) {
        Board board;
        parse_fen(start_position, board);
        //Search::search_position(board,5);
    }
    else if (debug == 2) {
        Perft::test_perft();
    }
    else {
        OldSearch::uci_loop();
    }
}
