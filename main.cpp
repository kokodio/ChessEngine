#include "Board.hpp"
#include "Perft.hpp"

constexpr int debug = 3;

int main() {
    Random::init_random_keys();
    Perft::test_perft();
}
