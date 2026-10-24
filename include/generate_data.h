#ifndef GEN_DB_H
#define GEN_DB_H

#include <string>

const int POSSIBLE_2X2_STATES = 88'179'840;
const int POSSIBLE_EDGE_STATES = 42'577'920; // For 6/12 edges

//                                  0    0    0    0    0    0    0    0    0    0    0    0
const uint64_t EDGE_CASE_ONE_3X3 = 0b000000000100010000110010000101011110111101111011110111101111;
const uint64_t EDGE_CASE_TWO_3X3 = 0b011110111101111011110111111111001100011101000010010101001011;

const std::string CORNERS_DB_FILENAME = "./data/2x2.bin";
const std::string EDGES_1_DB_FILENAME = "./data/edges-1.bin";
const std::string EDGES_2_DB_FILENAME = "./data/edges-2.bin";

/**
 * This algorithm, uses the Lehmer code to index each corner state,
 * such that when using a BFS it can save the minimum number of moves required to get to that
 * state in the 2x2 data file, to be used for lookups when solving the cube.
 */

#endif