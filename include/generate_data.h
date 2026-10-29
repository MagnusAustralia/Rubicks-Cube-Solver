#ifndef GEN_DB_H
#define GEN_DB_H

#include <cstdint>
#include <string>

const uint32_t POSSIBLE_CORNER_STATES = 88'179'840;         // 3^7 * 8P8
const uint32_t POSSIBLE_EDGE_STATES = 42'577'920;           // 2^6 * 12P6
const uint32_t POSSIBLE_CORNER_SLICE_STATES = 1'082'565;    // 3^7 * 12C4
const uint32_t POSSIBLE_EDGE_SLICE_STATES = 1'013'760;      // 2^11 * 12C4
const uint32_t POSSIBLE_CORNER_SLICE_PERM_STATES = 967'680; // 8P8 * 4P4
const uint32_t POSSIBLE_EDGE_SLICE_PERM_STATES = 967'680;   // 8P8 * 4P4

const std::string CORNERS_DB_FILENAME = "./data/korf-pdbs/2x2.bin";
const std::string EDGES_1_DB_FILENAME = "./data/korf-pdbs/edges-1.bin";
const std::string EDGES_2_DB_FILENAME = "./data/korf-pdbs/edges-2.bin";
const std::string CORNERS_SLICE_DB_FILENAME = "./data/kociemba-pdbs/corners-slice.bin";
const std::string EDGES_SLICE_DB_FILENAME = "./data/kociemba-pdbs/edges-slice.bin";
const std::string CORNERS_SLICE_PERM_DB_FILENAME = "./data/kociemba-pdbs/corners-slice-perm.bin";
const std::string EDGES_SLICE_DB_PERM_FILENAME = "./data/kociemba-pdbs/edges-slice-perm.bin";

enum pdb_type
{
    // Korf's Algorithm PDB
    CORNERS_PDB,
    EDGES_CASE_1_PDB,
    EDGES_CASE_2_PDB,
    // Kociemba's Algorithm PDB
    CORNER_SLICE_PDB,
    EDGE_SLICE_PDB,
    CORNER_SLICE_PERM_PDB,
    EDGE_SLICE_PERM_PDB
};

struct cube_state_data
{
    uint64_t edges;
    uint64_t corners;
};

/**
 * This algorithm, uses the Lehmer code to index each corner state,
 * such that when using a BFS it can save the minimum number of moves required to get to that
 * state in the 2x2 data file, to be used for lookups when solving the cube.
 */

#endif