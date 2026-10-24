#ifndef DEPTH_FIRST_SEARCH_H
#define DEPTH_FIRST_SEARCH_H

#include "../include/lehmer_code.h"
#include "../include/rubicks_2x2.h"

/**
 * This algorithm uses the corner data, to lookup the max_moves_remaining
 * And uses that as a hueristic in the IDA* search algorithm, to solve the cube.
 */

struct node
{
    rubicks_2x2 cube;
    moves move;
    int depth;

    // Overload the < operator
    bool operator<(const node &other) const
    {
        // Higher depth means LOWER priority (comes later)
        return depth > other.depth;
    }
};

/**
 *
 */
int moves_till_solved(const rubicks_2x2 &cube);

int heuristic(const rubicks_2x2 cube);

void iterative_depthening_a_star(rubicks_2x2 cube);

#endif