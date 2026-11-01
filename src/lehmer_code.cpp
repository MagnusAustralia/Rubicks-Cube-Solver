#include "../include/lehmer_code.h"
#include "../include/rubicks_2x2.h"
#include "../include/rubicks_3x3.h"
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <vector>

uint32_t factorial(int n)
{
    if (n <= 1)
        return 1;
    return n * factorial(n - 1);
}

uint32_t choose(int n, int k)
{
    if (k > n)
    {
        return 0;
    }

    return factorial(n) / (factorial(k) * factorial(n - k));
}

uint32_t permutate(int n, int k)
{
    if (k > n)
    {
        return 0;
    }
    return factorial(n) / (factorial(n - k));
}

void lehmer_data::find_considered_elements_index_list()
{
    uint64_t remaining_edges = cube.edges;
    int index = 0;

    switch (pdb)
    {
    case CORNERS_PDB:
        std::cerr << "All corners are considered when finding the lehmer rank for the CORNERS_PDB\n";
        return;
    case EDGES_CASE_1_PDB:
    {
        for (int j = 0; j < num_elements; j++)
        {
            // Finds the edge number of the jth edge
            uint8_t current_edge = (uint8_t)((remaining_edges >> (j * EDGE_WIDTH)) & EDGE_PIECE_MASK);
            if (current_edge < num_considered)
            {
                considered_element_index_list[current_edge] = j;
            }
        }
        break;
    }
    case EDGES_CASE_2_PDB:
    {
        for (int j = 0; j < num_elements; j++)
        {
            uint8_t current_edge = (uint8_t)((remaining_edges >> (j * EDGE_WIDTH)) & EDGE_PIECE_MASK);
            if (current_edge >= num_considered && current_edge != uint8_t(NN))
            {
                considered_element_index_list[current_edge - num_considered] = j;
            }
        }
        break;
    }
    case CORNER_SLICE_PDB:
    case EDGE_SLICE_PDB:
    case CORNER_SLICE_PERM_PDB:
    {
        for (int j = 0; index < 4; j++)
        {
            uint8_t current_edge = (uint8_t)((remaining_edges >> (j * EDGE_WIDTH)) & EDGE_PIECE_MASK);
            // std::cout << current_edge << "\n";
            if (current_edge >= 4 && current_edge <= 7)
            {
                considered_element_index_list[index] = j;
                index++;
            }
        }
        break;
    }
    case EDGE_SLICE_PERM_PDB:
        std::cerr << "All edges are considered when finding the lehmer rank for the EDGE_SLICE_PERM_PDB\n";
        break;
    }
}

int lehmer_data::find_middle_slice()
{
    int total = 0;
    for (int i = 0; i < 4; i++)
    {
        total += choose(considered_element_index_list[i], i + 1);
    }

    return total;
}

int lehmer_data::find_middle_slice_permutation_rank()
{
    const int EDGE_CONSIDERED = 4;
    int indexes[EDGE_CONSIDERED];

    // Pre-shifts remaining_edges, such that the middle four edges will be on the end
    uint64_t remaining_edges = cube.edges >> (EDGE_WIDTH * 4);

    for (int i = EDGE_CONSIDERED - 1; i >= 0; i--)
    {
        indexes[i] = (uint8_t)(remaining_edges & EDGE_PIECE_MASK);
        remaining_edges >>= EDGE_WIDTH;
    }

    int rank = 0;

    for (int i = 0; i < EDGE_CONSIDERED; i++)
    {
        int count = 0;
        for (int j = i + 1; j < 4; j++)
        {
            if (indexes[i] > indexes[j])
                count++;
        }
        rank += permutate(EDGE_CONSIDERED - 1 - i, EDGE_CONSIDERED - 1 - i) * count;
    }
    return rank;
}

int lehmer_data::rank_orientations()
{
    int total = 0;
    uint64_t remaining_corners = cube.corners;
    uint64_t remaining_edges = cube.edges;

    switch (pdb)
    {
    case CORNERS_PDB:
    case CORNER_SLICE_PDB:
        // This converts the remaining corners into a sum, by treating each corner as a base 3 number
        // and iterating through each element, followed by shifting up by 3
        // Only corners [8,2] are considered as the eighth corner is deterministic if 7 are known
        {
            for (int i = NUM_CORNERS - 1; i >= 1; i--) // i >= 1 to exclude the last element as it is deterministic
            {
                total = (total * NUM_CORNER_ORIENTATIONS) +
                        ((remaining_corners & CORNER_ORIENTATION_MASK) >> CORNER_ORIENTATION_SHIFT);
                remaining_corners >>= CORNER_WIDTH;
            }
            break;
        }
    case EDGES_CASE_1_PDB:
    case EDGES_CASE_2_PDB:
        for (int i = num_considered - 1; i >= 0; i--)
        {
            total = (total * NUM_EDGE_ORIENTATIONS) +
                    (((remaining_edges >> (EDGE_WIDTH * considered_element_index_list[i])) & EDGE_ORIENTATION_MASK) >>
                     EDGE_ORIENTATION_SHIFT);
        }
        break;
    case EDGE_SLICE_PDB:
    {
        for (int i = NUM_EDGES - 1; i >= 1; i--) // i >= 1 to exclude the last element as it is deterministic
        {
            total =
                (total * NUM_EDGE_ORIENTATIONS) + ((remaining_edges & EDGE_ORIENTATION_MASK) >> EDGE_ORIENTATION_SHIFT);
            remaining_edges >>= EDGE_WIDTH;
        }
        break;
    }
    case CORNER_SLICE_PERM_PDB:
        std::cerr << "Orientations should not be considered for CORNER_SLICE_PERM_PDB\n";
        break;
    case EDGE_SLICE_PERM_PDB:
        std::cerr << "Orientations should not be considered for EDGE_SLICE_PERM_PDB\n";
        break;
    }

    return total;
}

void lehmer_data::find_position_permutation_list()
{
    switch (pdb)
    {
    case CORNERS_PDB:
    case CORNER_SLICE_PERM_PDB:
    {
        int indexes[NUM_CORNERS];

        uint64_t remaining_corners = cube.corners;

        for (int i = NUM_CORNERS - 1; i >= 0; i--)
        {
            indexes[i] = (uint8_t)(remaining_corners & CORNER_PIECE_MASK);
            remaining_corners >>= CORNER_WIDTH;
        }

        for (int i = 0; i < NUM_CORNERS; i++)
        {
            int count = 0;
            for (int j = i + 1; j < NUM_CORNERS; j++)
            {
                if (indexes[i] > indexes[j])
                    count++;
            }
            position_permutation_list[i] = count;
        }
        break;
    }
    case EDGES_CASE_1_PDB:
    case EDGES_CASE_2_PDB:
    {
        bool elements_seen[12] = {};

        for (int i = 0; i < num_considered; i++)
        {
            int count = 0;
            int position = considered_element_index_list[i];
            for (int j = 0; j < position; j++)
            {
                if (!elements_seen[j])
                {
                    count++;
                }
            }
            position_permutation_list[i] = count;
            elements_seen[position] = true;
        }
        break;
    }
    case CORNER_SLICE_PDB:
        std::cerr << "The position permutation is not considered for CORNER_SLICE_PDB\n";
        break;
    case EDGE_SLICE_PDB:
        std::cerr << "The position permutation is not considered for EDGE_SLICE_PDB\n";
        break;
    case EDGE_SLICE_PERM_PDB:
    {
        int indexes[NUM_EDGES - 4];
        uint8_t current_edge = 0;

        uint64_t remaining_edges = cube.edges;

        int output_index = 0;

        for (int i = NUM_EDGES - 1; i >= 0 && output_index < num_considered; i--)
        {
            current_edge = (uint8_t)(remaining_edges & EDGE_PIECE_MASK);
            if (current_edge < 4 || current_edge >= 8) // Excludes the 4 middle slice edges
            {
                indexes[output_index++] = current_edge;
            }

            remaining_edges >>= EDGE_WIDTH;
        }

        for (int i = 0; i < NUM_EDGES - 4; i++)
        {
            int count = 0;
            for (int j = i + 1; j < NUM_EDGES - 4; j++)
            {
                if (indexes[i] > indexes[j])
                    count++;
            }
            position_permutation_list[i] = count;
        }
        break;
    }
    }
}

int lehmer_data::rank()
{
    int index = 0;

    for (int i = 0; i < num_considered; i++)
    {
        index += factorial_list[i] * position_permutation_list[i];
    }

    return index;
}

int lehmer_data::lehmer_index()
{
    switch (pdb)
    {
    case CORNERS_PDB:
        // i.e. Find the ordering of the corners and then rank them * (3^7) + corner orientation rank
        find_position_permutation_list(); // Finds inversion vector for 8 corners
        // Finds the lexiographic rank of the 8 corners, shifted by the 3^7 possible corner orientations, then adding
        // the rank of the corner orientations (considers 7/8 corners)
        return (rank() * TOTAL_NUM_CORNER_ORIENTATIONS) + rank_orientations();
    case EDGES_CASE_1_PDB:
    case EDGES_CASE_2_PDB:
        // i.e. Find the partial permutation of the first 6 edges, find the position permutation rank assosiated with
        // that partial permutation, and also rank the orierntation of the first 6 edges
        find_considered_elements_index_list(); // Finds the indexes of the considered edges of the partial permutation
        find_position_permutation_list();      // Finds the inversion vector for 6/12 edges
        // Finds the lexiographic rank of the partial permutation of 6/12 of the edges, shifted by the 2^6 possible edge
        // orientations they could have, then adding the rank of those 6 edge orientations
        return (rank() * TOTAL_NUM_EDGE_ORIENTATION_6_EDGES) + rank_orientations();
    case CORNER_SLICE_PDB:
        // i.e. Finds the combinatorial rank for the positions of the middle 4 slice edges (i.e. how many are in the
        // middle slice), shifted by the 3^7 possible corner orientations, then adding the rank of the corner
        // orientations (considering 7/8 corners)
        find_considered_elements_index_list(); // Finds the indexes of the middle 4 elements
        return (find_middle_slice() * TOTAL_NUM_CORNER_ORIENTATIONS) + rank_orientations();
    case EDGE_SLICE_PDB:
        // i.e. Finds the combinatorial rank for the positions of the middle 4 slice edges (i.e. how many are in the
        // middle slice), shifted by the 2^11 possible edge orientations, then adding the rank of the edge
        // orientations (considering 11/12 edges)
        find_considered_elements_index_list(); // Finds the indexes of the middle 4 elements
        return (find_middle_slice() * TOTAL_NUM_EDGE_ORIENTENTATIONS) + rank_orientations();
    case CORNER_SLICE_PERM_PDB:
        // i.e. Finds the position permutation of the 8 corners, and their rank, shifts it by 4P4, then adds the
        // permutation rank of the four middle slice edges
        find_position_permutation_list();
        return (rank() * NUM_SLICE_PERMS) + find_middle_slice_permutation_rank();
    case EDGE_SLICE_PERM_PDB:
        // i.e. Finds the position permutation of the 8 edges (not in the middle slice), and their rank, shifts it by
        // 4P4, then adds the permutation rank of the four middle slice edges
        find_position_permutation_list();
        return (rank() * NUM_SLICE_PERMS) + find_middle_slice_permutation_rank();
    }
}