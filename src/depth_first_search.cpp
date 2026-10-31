#include "../include/depth_first_search.h"
#include "../include/generate_data.h"
#include "../include/generate_scramble.h"
#include "../include/lehmer_code.h"
#include "../include/rubicks_2x2.h"
#include <fcntl.h>
#include <iostream>
#include <stack>
#include <sys/mman.h>
#include <unistd.h>

unsigned char *open_file(const std::string FILENAME, const uint32_t NUM_POSSIBLE_STATES)
{
    int fd = open(FILENAME.c_str(), O_RDONLY);
    if (fd == -1)
    {
        std::cerr << "ERROR: Open failed\n";
    }
    void *mapping = mmap(nullptr, NUM_POSSIBLE_STATES / 2, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapping == MAP_FAILED)
    {
        std::cerr << "ERROR: mmap failed\n";
    }
    close(fd);

    return static_cast<unsigned char *>(mapping);
}

void open_data_files(pattern_dbs &db)
{
    db.corner_depths = open_file(CORNERS_DB_FILENAME, POSSIBLE_CORNER_STATES);
    db.edge_case_1_depths = open_file(EDGES_1_DB_FILENAME, POSSIBLE_EDGE_STATES);
    db.edge_case_2_depths = open_file(EDGES_2_DB_FILENAME, POSSIBLE_EDGE_STATES);
    db.corner_slice_depths = open_file(CORNERS_SLICE_DB_FILENAME, POSSIBLE_CORNER_SLICE_STATES);
    db.edge_slice_depths = open_file(EDGES_SLICE_DB_FILENAME, POSSIBLE_EDGE_SLICE_STATES);
    // db.corner_slice_perm_depths = open_file(CORNERS_SLICE_DB_FILENAME, POSSIBLE_CORNER_SLICE_PERM_STATES);
    // db.edge_slice_perm_depths = open_file(EDGES_SLICE_DB_FILENAME, POSSIBLE_EDGE_SLICE_PERM_STATES);
}

void close_data_files(pattern_dbs &db)
{
    munmap(static_cast<void *>(db.corner_depths), POSSIBLE_CORNER_STATES / 2);
    munmap(static_cast<void *>(db.edge_case_1_depths), POSSIBLE_EDGE_STATES / 2);
    munmap(static_cast<void *>(db.edge_case_2_depths), POSSIBLE_EDGE_STATES / 2);
    munmap(static_cast<void *>(db.corner_slice_depths), POSSIBLE_CORNER_SLICE_STATES / 2);
    munmap(static_cast<void *>(db.edge_slice_depths), POSSIBLE_EDGE_SLICE_STATES / 2);
    // munmap(static_cast<void *>(db.corner_slice_perm_depths), POSSIBLE_CORNER_SLICE_PERM_STATES / 2);
    // munmap(static_cast<void *>(db.edge_slice_perm_depths), POSSIBLE_EDGE_SLICE_PERM_STATES / 2);
}

int moves_till_solved(const rubicks_2x2 cube, const pattern_dbs &db, const pdb_type PBD)
{
    int distance_from_solved = 0;
    uint32_t index = 0;
    unsigned char byte = 0;

    switch (PBD)
    {
    case CORNERS_PDB: // Works
    {
        lehmer_data<8, 8, rubicks_2x2> indexer;
        indexer.cube.corners = cube.corners;
        index = indexer.lehmer_index();
        byte = db.corner_depths[index >> 1];
        break;
    }
    default:
        std::cerr << "Invalid pattern database type for a 2x2";
    }
    if (index & 1)
    {
        distance_from_solved = (byte >> 4) & 0b1111;
    }
    else
    {
        distance_from_solved = byte & 0b1111;
    }

    return distance_from_solved;
}

int moves_till_solved(const rubicks_3x3 cube, const pattern_dbs &db, const pdb_type PBD)
{
    int distance_from_solved = 0;
    uint32_t index = 0;
    unsigned char byte = 0;

    switch (PBD)
    {
    case CORNERS_PDB: // Works
    {
        lehmer_data<8, 8, rubicks_2x2> indexer;
        indexer.cube.corners = cube.corners;
        index = indexer.lehmer_index();
        byte = db.corner_depths[index >> 1];
        break;
    }
    case EDGES_CASE_1_PDB: // Works
    {
        lehmer_data<12, 6, rubicks_3x3> indexer(CASE_ONE_3X3);
        indexer.cube = cube;
        index = indexer.lehmer_index();
        byte = db.edge_case_1_depths[index >> 1];
        break;
    }
    case EDGES_CASE_2_PDB: // Works
    {
        lehmer_data<12, 6, rubicks_3x3> indexer(CASE_TWO_3X3);
        indexer.cube = cube;
        index = indexer.lehmer_index();
        byte = db.edge_case_2_depths[index >> 1];
        break;
    }
    case CORNER_SLICE_PDB:
    {
        lehmer_data<12, 4, rubicks_3x3> indexer(CASE_MIDDLE_SLICE_3X3);
        indexer.cube = cube;
        index = indexer.lehmer_index();
        byte = db.corner_slice_depths[index >> 1];
        break;
    }
    case EDGE_SLICE_PDB:
    { // I can't remember D:
        // do something
        break;
    }
    case CORNER_SLICE_PERM_PDB:
    {
        // lehmer_data<8, 8, rubicks_3x3> indexer(CASE_MIDDLE_SLICE_3X3);
        // indexer.cube.corners = cube.corners;
        // indexer.cube.edges = cube.edges;
        // index = indexer.lehmer_index();
        // byte = db.corner_depths[index >> 1];
        break;
    }
    case EDGE_SLICE_PERM_PDB:
    { // Fix this one
        // lehmer_data<8, 8, rubicks_3x3> indexer(CASE_MIDDLE_SLICE_3X3);
        // indexer.cube.corners = cube.corners;
        // indexer.cube.edges = cube.edges;
        // index = indexer.lehmer_index();
        // byte = db.corner_depths[index >> 1];
        break;
    }
    }

    if (index & 1)
    {
        distance_from_solved = (byte >> 4) & 0b1111;
    }
    else
    {
        distance_from_solved = byte & 0b1111;
    }

    return distance_from_solved;
}

template <typename cube_type>
void iterative_deepening_a_star(const cube_type &cube, moves (&moves_list)[21], const pattern_dbs &db,
                                int (*heuristic)(const cube_type &, const pattern_dbs &, int))
{
    std::stack<node<cube_type>> node_stack;
    bool solved = false;
    // Initialises the next_bound as the heuristic of the cube
    int next_bound = heuristic(cube, db, INFINITE_DEPTH);
    int bound = 0;
    int estimated_moves = 0;
    node<cube_type> current_node;
    while (!solved)
    {
        if (node_stack.empty())
        {
            node_stack.push(node<cube_type>{cube, INVALID_MOVE, 0});
            bound = next_bound;
            next_bound = INFINITE_DEPTH; // i.e. Infinity
        }

        current_node = node_stack.top();
        node_stack.pop();

        if (current_node.depth > 0)
        {
            moves_list[current_node.depth - 1] = current_node.move;
        }
        moves_list[current_node.depth] = INVALID_MOVE;

        if (current_node.depth == bound)
        {
            if (current_node.cube.is_solved() || (heuristic(current_node.cube, db, INFINITE_DEPTH) == 0))
            {
                solved = true;

                return;
            }
        }
        else
        {
            std::stack<node<cube_type>> successors;
            for (int i = 0; i < NUM_MOVES; i++)
            {
                if (current_node.depth == 0 || !prune(moves(i), current_node.move))
                {
                    cube_type new_cube;
                    new_cube = current_node.cube;
                    new_cube.input_move(moves(i));
                    estimated_moves = current_node.depth + 1 + heuristic(new_cube, db, bound - current_node.depth - 1);
                    if (estimated_moves <= bound)
                    {
                        successors.push(node<cube_type>{new_cube, moves(i), estimated_moves});
                    }
                    else if (estimated_moves < next_bound)
                    {
                        next_bound = estimated_moves;
                    }
                }
            }
            while (!successors.empty())
            {
                node<cube_type> successor = successors.top();
                successors.pop();
                node_stack.push(node<cube_type>{successor.cube, successor.move, current_node.depth + 1});
            }
        }
    }
}

template void iterative_deepening_a_star<rubicks_2x2>(const rubicks_2x2 &, moves (&)[21], const pattern_dbs &,
                                                      int (*)(const rubicks_2x2 &, const pattern_dbs &, int));
template void iterative_deepening_a_star<rubicks_3x3>(const rubicks_3x3 &, moves (&)[21], const pattern_dbs &,
                                                      int (*)(const rubicks_3x3 &, const pattern_dbs &, int));