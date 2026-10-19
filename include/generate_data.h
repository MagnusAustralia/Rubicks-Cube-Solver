#ifndef GEN_DB_H
#define GEN_DB_H

const int POSSIBLE_2X2_STATES = 88179840;
const int POSSIBLE_3X3_STATES = 88179840;

/**
 * This algorithm, uses the Lehmer code to index each corner state,
 * such that when using a BFS it can save the minimum number of moves required to get to that
 * state in the 2x2 data file, to be used for lookups when solving the cube.
 */

#endif