#ifndef GEN_SCRAMBLE
#define GEN_SCRAMBLE

#include "./rubicks_2x2.h"
#include <string>

/**
 * Returns a bool determining whether the new_move is valid after the previous move
 *
 * @param new_move The potential next move
 * @param previous_move The move the new_move is being compared against
 * @param restricted_moveset This boolean flag indicates whether to limit the valid moveset ot not
 *
 * @return True if the new_move is illegal, otherwise False
 */
bool prune(moves new_move, moves previous_move, const bool restricted_moveset = false);

/**
 * Generates a string that can be passed into a Rubicks Cube through the input_scramble() member function.
 *
 * @param NUM_MOVES_TO_GENERATE The number of moves that the scramble should be
 *
 * @return The scramble in the form of a string
 */
std::string generate_scramble(const int NUM_MOVES_TO_GENERATE = 50);

#endif