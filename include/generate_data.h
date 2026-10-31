#ifndef GEN_DB_H
#define GEN_DB_H

#include <cstdint>
#include <string>

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

#endif