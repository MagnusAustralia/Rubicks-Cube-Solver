#ifndef GEN_SCRAMBLE
#define GEN_SCRAMBLE

#include "./rubicks_2x2.h"
#include <string>

bool prune(moves new_move, moves previous_moves);

std::string generate_scramble(const int NUM_MOVES = 50);

#endif