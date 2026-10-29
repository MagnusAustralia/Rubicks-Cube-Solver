#ifndef DEPTH_FIRST_SEARCH_H
#define DEPTH_FIRST_SEARCH_H

#include "../include/lehmer_code.h"
#include "../include/rubicks_2x2.h"
#include "../include/rubicks_3x3.h"

struct stats
{
    // per iteration
    uint64_t expanded = 0, generated = 0, kept = 0, rem1 = 0, rem2 = 0;
    uint64_t prev_expanded = 0;
    int iteration = 0;
    // per scramble
    uint64_t total_expanded = 0, total_generated = 0, total_rem1 = 0, total_rem2 = 0;
    // heuristic diagnostics (per scramble)
    uint64_t exit_corner = 0, exit_edge1 = 0, full_eval = 0;
    uint64_t win_corner = 0, win_edge1 = 0, win_edge2 = 0;
};
stats g_stats;

const int INFINITE_DEPTH = 100;

/**
 * This algorithm uses the corner data, to lookup the max_moves_remaining
 * And uses that as a hueristic in the IDA* search algorithm, to solve the cube.
 */

struct pattern_dbs
{
    unsigned char *corner_depths;
    unsigned char *edge_case_1_depths;
    unsigned char *edge_case_2_depths;
    unsigned char *corner_slice_depths;
    unsigned char *edge_slice_depths;
};

struct node
{
    rubicks_3x3 cube;
    moves move;
    int depth;
};

/**
 *
 */
int moves_till_solved(const rubicks_3x3 &cube);

int heuristic_korf(const rubicks_3x3 cube);

void korfs_algorithm(rubicks_3x3 cube);

#endif