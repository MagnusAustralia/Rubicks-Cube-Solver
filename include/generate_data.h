#ifndef GEN_DB_H
#define GEN_DB_H

#include "./rubicks_3x3.h"
#include <cstdint>
#include <queue>
#include <string>
#include <unordered_map>

// The number of elements for the different pattern databases
const uint32_t POSSIBLE_CORNER_STATES = 88'179'840;         // 3^7 * 8P8
const uint32_t POSSIBLE_EDGE_STATES = 42'577'920;           // 2^6 * 12P6
const uint32_t POSSIBLE_CORNER_SLICE_STATES = 1'082'565;    // 3^7 * 12C4
const uint32_t POSSIBLE_EDGE_SLICE_STATES = 1'013'760;      // 2^11 * 12C4
const uint32_t POSSIBLE_CORNER_SLICE_PERM_STATES = 967'680; // 8P8 * 4P4
const uint32_t POSSIBLE_EDGE_SLICE_PERM_STATES = 967'680;   // 8P8 * 4P4

// Filepaths to the pattern databases used in Korf's Algorithm
const std::string CORNERS_DB_FILENAME = "./data/korf-pdbs/corners.bin";
const std::string EDGES_1_DB_FILENAME = "./data/korf-pdbs/edges-case-1.bin";
const std::string EDGES_2_DB_FILENAME = "./data/korf-pdbs/edges-case-2.bin";

// Filepaths to the pattern databases used in Kociemba's Algorithm
const std::string CORNERS_SLICE_DB_FILENAME = "./data/kociemba-pdbs/corners-slice.bin";
const std::string EDGES_SLICE_DB_FILENAME = "./data/kociemba-pdbs/edges-slice.bin";
const std::string CORNERS_SLICE_PERM_DB_FILENAME = "./data/kociemba-pdbs/corners-slice-perm.bin";
const std::string EDGES_SLICE_DB_PERM_FILENAME = "./data/kociemba-pdbs/edges-slice-perm.bin";

/**
 * A enum to clarify which pattern database is being used
 */
enum pdb_type
{
    CORNERS_PDB,
    EDGES_CASE_1_PDB,
    EDGES_CASE_2_PDB,
    CORNER_SLICE_PDB,
    EDGE_SLICE_PDB,
    CORNER_SLICE_PERM_PDB,
    EDGE_SLICE_PERM_PDB
};

/**
 * A struct storing a compact represensation of the cube state of a 3x3 Rubicks Cube
 *
 * @field edges The edges state of a given Rubicks Cube
 * @field corners The corners state of a given Rubicks Cube
 */
struct cube_state_data
{
    uint64_t edges;
    uint64_t corners;
};

/**
 * Finds all new states reachable from the inital cube state, and updated the known states with them and with the depth
 * that the new states were found at
 *
 * @param INITIAL_CUBE_STATE A store of the initial cube state
 * @param states A queue storing the new states found, which will hence need to be processed in the next depth of the
 * breadth first search
 * @param known_states A dictionary storing the lehmer index of proccessed states, mapped to the first depth it was
 * found at
 * @param depth The current depth at which the new states are being saved with
 * @param num_states A store of how many states have been processed
 * @param PBD The type of pattern database being generated, and hence the classifier determining how the lehmer index is
 * found
 */
void process_moves(const cube_state_data &INITIAL_CUBE_STATE, std::queue<rubicks_3x3> &states,
                   std::unordered_map<uint32_t, uint8_t> &known_states, const uint8_t depth, int &num_states,
                   const pdb_type PDB);

/**
 * Preforms a breadth first search on the 3x3 Rubicks cube where each state is classified by its lehmer index.
 *
 * @param depth The current depth of the bfs
 * @param current_depth The states to explore in the current depth of the bfs
 * @param num_states_processed The number of cube states found
 * @param known_states A dictionary storing the lehmer index of proccessed states, mapped to the first depth it was
 * found at
 * @param PDB The type of pattern database being generated, and hence the classifier determining how the lehmer index is
 * found
 */
void bfs(uint8_t &depth, std::queue<rubicks_3x3> &current_depth, int &num_states_processd,
         std::unordered_map<uint32_t, uint8_t> &known_states, const pdb_type PDB);

/**
 * Generates the patteren database of the specfied pattern database
 *
 * @param PDB The pattern database to generate
 */
void generate_db(const pdb_type PDB);

#endif