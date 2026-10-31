#include "../include/korfs_algorithm.h"
#include "../include/depth_first_search.h"
#include "../include/generate_data.h"
#include "../include/generate_scramble.h"
#include "../include/lehmer_code.h"
#include "../include/rubicks_2x2.h"
#include <algorithm>
#include <iostream>
#include <stack>
#include <unistd.h>

int heuristic_korf(const rubicks_2x2 &cube, const pattern_dbs &db, const int)
{
    return moves_till_solved(cube, db, CORNERS_PDB);
}

int heuristic_korf(const rubicks_3x3 &cube, const pattern_dbs &db, const int MAX_HEURISTIC)
{
    rubicks_3x3 edges_cube;
    edges_cube.corners = cube.corners;

    uint64_t edges_case_1 = cube.edges;
    uint64_t edges_case_2 = cube.edges;

    // Iterates through each edge position, and replaces it with a blank edge in the edge_case that it's not meant to be
    // in (i.e. if the current_edge is 7 (which is only in CASE_TWO_3X3, then it replaces that edge in edge_case 1 with
    // a blank edge))
    for (int i = 0; i < NUM_EDGES; i++)
    {
        // Finds the edge number of the edge in the ith position
        int current_edge = (cube.edges >> (EDGE_WIDTH * i)) & EDGE_PIECE_MASK;
        if (current_edge >= 6)
        {
            edges_case_1 = (edges_case_1 & ~(EDGE_MASK << (EDGE_WIDTH * i))) | (uint64_t(NN) << (EDGE_WIDTH * i));
        }
        else
        {
            edges_case_2 = (edges_case_2 & ~(EDGE_MASK << (EDGE_WIDTH * i))) | (uint64_t(NN) << (EDGE_WIDTH * i));
        }
    }

    // Finds the number of moves required to solve the corners
    int h1 = moves_till_solved(cube, db, CORNERS_PDB);

    // If the estimated distance h1 is more than the highest considered distance,
    // then it returns to reduce required compute as it will be pruned anyway
    if (h1 > MAX_HEURISTIC)
    {
        return h1;
    }

    edges_cube.edges = edges_case_1;

    // Finds the number of moves required to solve the first 6 edges
    int h2 = moves_till_solved(edges_cube, db, EDGES_CASE_1_PDB);

    // If the estimated distance h2 is more than the highest considered distance,
    // then it returns to reduce required compute as it will be pruned anyway
    if (h2 > MAX_HEURISTIC)
    {
        return h2;
    }

    edges_cube.edges = edges_case_2;

    // Finds the number of moves required to solve the last 6 edges
    int h3 = moves_till_solved(edges_cube, db, EDGES_CASE_2_PDB);

    // Returns the maximum value of h1, h2 and h3
    return std::max({h1, h2, h3});
}

void korfs_algorithm(rubicks_2x2 &cube, const pattern_dbs &db)
{
    moves moves_list[21] = {};

    iterative_deepening_a_star(cube, moves_list, db, heuristic_korf);

    std::cout << "\nMoves Used: ";

    for (int i = 0; moves_list[i] != INVALID_MOVE; i++)
    {
        std::cout << print_move(moves_list[i]) << " ";
    }
    std::cout << "\n\n";
}

void korfs_algorithm(rubicks_3x3 &cube, const pattern_dbs &db)
{
    moves moves_list[21] = {};

    iterative_deepening_a_star(cube, moves_list, db, heuristic_korf);

    std::cout << "\nMoves Used: ";

    for (int i = 0; moves_list[i] != INVALID_MOVE; i++)
    {
        std::cout << print_move(moves_list[i]) << " ";
    }
    std::cout << "\n\n";
}

int main()
{
    srand(time(0));

    pattern_dbs db;

    open_data_files(db);

    std::string scramble = generate_scramble(10);

    rubicks_3x3 cube;

    cube.input_scramble(scramble);

    cube.print_cube();
    std::cout << "Scramble: " << scramble << "\n\n";

    korfs_algorithm(cube, db);

    close_data_files(db);

    return 0;
}