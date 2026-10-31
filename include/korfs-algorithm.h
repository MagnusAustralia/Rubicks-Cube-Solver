#ifndef KORF_H
#define KORF_H

#include "../include/depth_first_search.h"
#include "../include/rubicks_3x3.h"

/**
 * Finds an estimated number of moves that a given cube state is from solved, using Korf's Algorithm
 *
 * @param cube The cube state being analysed
 * @param db The pattern_dbs struct storing pointers to all the memory mapped pattern databases
 * @param MAX_HEURISTIC The maximum distance in which the new state won't be pruned
 *
 * @return The estimated number of moves to the solved state
 */
int heuristic_korf(const rubicks_3x3 &cube, const pattern_dbs &db, const int MAX_HEURISTIC);

/**
 * Finds the optimal solution for a 3x3 Rubicks Cube
 *
 * @param cube The cube state being analysed
 * @param db The pattern_dbs struct storing pointers to all the memory mapped pattern databases
 */
void korfs_algorithm(const rubicks_3x3 &cube, const pattern_dbs &db);

#endif