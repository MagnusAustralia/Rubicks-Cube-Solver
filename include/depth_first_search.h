#ifndef DEPTH_FIRST_SEARCH_H
#define DEPTH_FIRST_SEARCH_H

#include "../include/generate_data.h"
#include "../include/lehmer_code.h"
#include "../include/rubicks_2x2.h"
#include "../include/rubicks_3x3.h"
#include <cstdint>

const int MAX_ALLOWED_MOVES = 50;
const int INFINITE_DEPTH = 100;

/**
 * A struct which stores pointers to the different memory-mapped pattern databses
 *
 * @field corner_depth A pointer to the memory-mapped pattern database for the corner2 depths
 * @field edge_case_1_depths A pointer to the memory-mapped pattern database for the edge2-case-1 depths
 * @field edge_case_2_depths A pointer to the memory-mapped pattern database for the edge2-case-2 depths
 * @field corner_slice_depths A pointer to the memory-mapped pattern database for the corners-slice depths
 * @field edge_slice_depths A pointer to the memory-mapped pattern database for the edges-slice depths
 * @field corner_slice_perm_depths A pointer to the memory-mapped pattern database for the corners-slices-perm depths
 * @field edge_slice_perm_depths A pointer to the memory-mapped pattern database for the edges-slices-perm depths
 */
struct pattern_dbs
{
    unsigned char *corner_depths;
    unsigned char *edge_case_1_depths;
    unsigned char *edge_case_2_depths;
    unsigned char *corner_slice_depths;
    unsigned char *edge_slice_depths;
    unsigned char *corner_slice_perm_depths;
    unsigned char *edge_slice_perm_depths;
};

/**
 * A struct which stores a node of in the iterative deepening depth first search
 *
 * @field cube Stores the current cube state of the node
 * @field move Stores the previous move used to reach this state
 * @field depth Stores the current depth of this node
 */
template <typename cube_type> struct node
{
    cube_type cube;
    moves move;
    int depth;
};

/**
 * Opens the passed in file, memory maps it to a pointer and then closes the file
 *
 * @param FILENAME The string storing the filepath of the specific data file
 * @param NUM_POSSIBLE_STATES The number used to ensure that the memory map is the correct size
 *
 * @return A pointer to the created memory mapped pattern database
 */
unsigned char *open_file(const std::string FILENAME, const uint32_t NUM_POSSIBLE_STATES);

/**
 * Updates the passed in pattern_dbs struct with the pointers to the memory maps of each pattern database
 *
 * @param db The pattern_dbs struct memory-map the pointers of
 */
void open_data_files(pattern_dbs &db);

/**
 * Un-memory maps all of the pattern databases in the the passed in pattern_dbs struct.
 *
 * @param db The pattern_dbs struct un-memory-map the pointers of
 */
void close_data_files(pattern_dbs &db);

/**
 * Returns an estimate of the number of moves required to solve the passed in cube state, according to a specified
 * pattern database
 *
 * @param cube The cube state to analyse
 * @param db The pattern_dbs struct, storing pointers to the memory mapped pattern databases
 * @param PDB The specific pattern database to use to analyse the cube state
 *
 * @return The estimated number of moves to satify the specified pattern database
 */
int moves_till_solved(const rubicks_2x2 cube, const pattern_dbs &db, const pdb_type PDB);

/**
 * Returns an estimate of the number of moves required to solve the passed in cube state, according to a specified
 * pattern database
 *
 * @param cube The cube state to analyse
 * @param db The pattern_dbs struct, storing pointers to the memory mapped pattern databases
 * @param PDB The specific pattern database to use to analyse the cube state
 *
 * @return The estimated number of moves to satify the specified pattern database
 */
int moves_till_solved(const rubicks_3x3 cube, const pattern_dbs &db, const pdb_type PDB);

/**
 * Preforms an iterative deepening A* depth first search to find the moves needed to reach the desired state
 *
 * @param cube The inital cube state to analyse
 * @param moves_list A list storing the moves used to reach the desired state
 * @param db The pattern_dbs struct, storing pointers to the memory mapped pattern databases
 * @param heuristic The heuristic function being used to estimate the moves from solved
 */
template <typename cube_type>
void iterative_deepening_a_star(cube_type &cube, moves (&moves_list)[MAX_ALLOWED_MOVES], const pattern_dbs &db,
                                int (*heuristic)(const cube_type &, const pattern_dbs &, int), int &depth,
                                const bool restricted_moveset = false);

#endif