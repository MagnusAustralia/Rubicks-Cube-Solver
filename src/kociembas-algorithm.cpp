#include "../include/kociembas-algorithm.h"
#include "../include/depth_first_search.h"
#include "../include/generate_data.h"
#include "../include/generate_scramble.h"
#include "../include/lehmer_code.h"
#include "../include/rubicks_2x2.h"
#include <algorithm>
#include <iostream>
#include <stack>
#include <unistd.h>

int heuristic_kociemba(const rubicks_3x3 &cube, const pattern_dbs &db, const int MAX_HEURISTIC)
{
    uint64_t slice_edges = cube.edges;
    rubicks_3x3 corner_slice_cube;
    rubicks_3x3 edge_slice_cube = cube;

    // Iterates through each edge position, and sets any edges out of the middle slice into blank edge pieces
    for (int i = 0; i < NUM_EDGES; i++)
    {
        // Finds the edge number of the edge in the ith position
        int current_edge = (cube.edges >> (EDGE_WIDTH * i)) & EDGE_PIECE_MASK;
        if (current_edge < 4 || current_edge >= 8)
        {
            slice_edges = (slice_edges & ~(EDGE_MASK << (EDGE_WIDTH * i))) | (uint64_t(NN) << (EDGE_WIDTH * i));
        }
    }

    corner_slice_cube.edges = slice_edges;
    corner_slice_cube.corners = cube.corners;

    // Finds the number of moves required to reach the correct corner orientations and have
    // every middle slice edge, in the middle slice
    int h1 = moves_till_solved(corner_slice_cube, db, CORNER_SLICE_PDB);

    // If the estimated distance h1 is more than the highest considered distance,
    // then it returns to reduce required compute as it will be pruned anyway
    if (h1 > MAX_HEURISTIC)
    {
        return h1;
    }

    // Finds the number of moves required to reach the correct edge orientations and have
    // every middle slice edge, in the middle slice
    int h2 = moves_till_solved(edge_slice_cube, db, EDGE_SLICE_PDB);

    // Returns the maximum value of h1 and h2
    return std::max({h1, h2});
}

void kociembas_algorithm(const rubicks_3x3 &cube, const pattern_dbs &db)
{
    moves moves_list[21] = {};

    iterative_deepening_a_star(cube, moves_list, db, heuristic_kociemba);

    std::cout << "\nMoves Used: ";

    for (int i = 0; moves_list[i] != INVALID_MOVE; i++)
    {
        std::cout << print_move(moves_list[i]) << " ";
    }
    std::cout << "\n\n";
}
