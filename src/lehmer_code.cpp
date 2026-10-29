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

// Corner Pattern Databases: Korf's Algorithm

// Korf: Corner Perm of 8 Corners
template <> void lehmer_data<8, 8, rubicks_2x2>::find_position_permutation_list()
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
}

// Kociemba: Corner Perm of 8 Corners
template <> void lehmer_data<8, 8, rubicks_3x3>::find_position_permutation_list()
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
}

// Korf: Rank orientation of 7/8 corners
template <int num_elements, int num_considered, typename cube_type>
int lehmer_data<num_elements, num_considered, cube_type>::rank_orientations()
{
    int total = 0;
    uint64_t remaining_corners = cube.corners;

    // This converts the remaining corners into a sum, by treating each corner as a base 3 number
    // and iterating through each element, followed by shifting up by 3
    // Only corners [8,2] are considered as the eighth corner is deterministic if 7 are known
    for (int i = NUM_CORNERS - 1; i >= 1; i--)
    {
        total = (total * NUM_CORNER_ORIENTATIONS) +
                ((remaining_corners & CORNER_ORIENTATION_MASK) >> CORNER_ORIENTATION_SHIFT);
        remaining_corners >>= CORNER_WIDTH;
    }

    return total;
}

// Edge Pattern Databases: Korf's Algorithm

template <int num_elements, int num_considered, typename cube_type>
void lehmer_data<num_elements, num_considered, cube_type>::find_considered_elements_index_list()
{
    // AI was used for thie condition, in order to specify the cube_type, while keeping the template values
    if constexpr (std::is_same_v<cube_type, rubicks_3x3>)
    {
        uint64_t remaining_edges = cube.edges;
        int index = 0;

        // Iterates through the edges and based on the cube_variation to determine the considers elements
        switch (cube_variation)
        {
        case CASE_ONE_3X3:
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
        case CASE_TWO_3X3:
            for (int j = 0; j < num_elements; j++)
            {
                uint8_t current_edge = (uint8_t)((remaining_edges >> (j * EDGE_WIDTH)) & EDGE_PIECE_MASK);
                if (current_edge >= num_considered)
                {
                    considered_element_index_list[current_edge - num_considered] = j;
                }
            }
            break;
        case CASE_MIDDLE_SLICE_3X3:
            for (int j = 0; index < 4; j++)
            {
                uint8_t current_edge = (uint8_t)((remaining_edges >> (j * EDGE_WIDTH)) & EDGE_PIECE_MASK);
                std::cout << current_edge << "\n";
                if (current_edge >= 4 && current_edge <= 7)
                {
                    considered_element_index_list[index] = j;
                    index++;
                }
            }
            break;

        case CASE_THREE_3X3:
            for (int j = 0; j < num_elements; j++)
            {
                uint8_t current_edge = (uint8_t)((remaining_edges >> (j * EDGE_WIDTH)) & EDGE_PIECE_MASK);
                if (current_edge < 4 || (current_edge >= 8 and current_edge < 10))
                {
                    considered_element_index_list[index] = j;
                    index++;
                }
            }

        case STANDARD_CUBE:
            std::cerr << "Invalid cube case for lehmer code\n";
            break;
        }
    }
    else
        return;
}

/**
 * Calculates the position_permutation_list for the sub-set of 6 edges, as a partial permutation
 */
template <int num_elements, int num_considered, typename cube_type>
void lehmer_data<num_elements, num_considered, cube_type>::find_position_permutation_list()
{
    bool elements_seen[num_elements] = {};

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
}

template <> int lehmer_data<12, 6, rubicks_3x3>::rank_orientations()
{
    const int NUM_CONSIDERED = 6;
    int total = 0;
    uint64_t remaining_edges = cube.edges;

    for (int i = NUM_CONSIDERED - 1; i >= 0; i--)
    {
        total = (total * NUM_EDGE_ORIENTATIONS) +
                (((remaining_edges >> (EDGE_WIDTH * considered_element_index_list[i])) & EDGE_ORIENTATION_MASK) >>
                 EDGE_ORIENTATION_SHIFT);
    }

    return total;
}

// PHASE 1 PATTERN DATABASES: Kociemba's Algorithm

// Corner Orientations and Slice Positions Pattern Databases: Kociemba's Algorithm

template <int num_elements, int num_considered, typename cube_type>
int lehmer_data<num_elements, num_considered, cube_type>::find_middle_slice()
{
    int total = 0;
    for (int i = 0; i < 4; i++)
    {
        total += choose(considered_element_index_list[i], i + 1);
    }

    return total;
}

template <int num_elements, int num_considered, typename cube_type>
int lehmer_data<num_elements, num_considered, cube_type>::find_middle_slice_permutation_rank()
{
    // AI was used for thie condition, in order to specify the cube_type, while keeping the template values
    if constexpr (std::is_same_v<cube_type, rubicks_3x3>)
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

    return -1;
}

template <> int lehmer_data<8, 8, rubicks_3x3>::rank_orientations()
{
    int total = 0;
    uint64_t remaining_edges = cube.edges;

    for (int i = NUM_EDGES - 2; i >= 0; i--)
    {
        total = (total * NUM_EDGE_ORIENTATIONS) + ((remaining_edges & EDGE_ORIENTATION_MASK) >> EDGE_ORIENTATION_SHIFT);
        remaining_edges >>= EDGE_WIDTH;
    }

    return total;
}

template <int num_elements, int num_considered, typename cube_type>
int lehmer_data<num_elements, num_considered, cube_type>::lehmer_index()
{
    return 0;
}

template <> int lehmer_data<8, 8, rubicks_2x2>::lehmer_index()
{
    find_position_permutation_list();
    return (rank() * NUM_CORNER_ORIENTS) + rank_orientations();
}

template <> int lehmer_data<12, 6, rubicks_3x3>::lehmer_index()
{
    find_considered_elements_index_list();
    find_position_permutation_list();
    return (rank() * 64) + rank_orientations();
}

template <> int lehmer_data<12, 4, rubicks_3x3>::lehmer_index()
{
    // Size = 3^7 * 12C4 -> rank(position_permutation_list) = 0-455
    // rank_orientations now maps correctly
    find_considered_elements_index_list();
    return (find_middle_slice() * NUM_CORNER_ORIENTS) + rank_orientations();
}

// Edge Orientations and Slice Positions Pattern Databases: Kociemba's Algorithm
template <> int lehmer_data<8, 8, rubicks_3x3>::lehmer_index()
{
    find_position_permutation_list();
    return (rank() * NUM_SLICE_PERMS) + find_middle_slice_permutation_rank();
}

// edges first
template <> int lehmer_data<8, 6, rubicks_3x3>::lehmer_index()
{
    find_considered_elements_index_list();
    find_position_permutation_list();
    return (rank() * NUM_SLICE_PERMS) + find_middle_slice_permutation_rank(); // Also need to add parity
}

// Genertic function to calculate the rank of a position_permutation_list
template <int num_elements, int num_considered, typename cube_type>
int lehmer_data<num_elements, num_considered, cube_type>::rank()
{
    int index = 0;

    for (int i = 0; i < num_considered; i++)
    {
        index += factorial_list[i] * position_permutation_list[i];
    }

    return index;
}

int main()
{
    lehmer_data<8, 8, rubicks_3x3> data;

    data.cube.input_scramble("B2");

    std::cout << "Rank: " << data.lehmer_index() << "\n";

    return 0;
}

template struct lehmer_data<8, 8, rubicks_2x2>;
template struct lehmer_data<12, 6, rubicks_3x3>;
template struct lehmer_data<12, 4, rubicks_3x3>;
template struct lehmer_data<8, 8, rubicks_3x3>;
template struct lehmer_data<8, 6, rubicks_3x3>;
template struct lehmer_data<8, 4, rubicks_3x3>;

// TODO: Used for co-ordinate mappings for Korf's Algorithm
// void generate_move_tables(int (&corner_perm_table)[NUM_CORNER_PERMS][NUM_MOVES])
// {
//     lehmer_data<8, 8, rubicks_2x2> data;
//     uint64_t selected_corner_perm_state = 0;

//     for (unsigned int i = 0; i < NUM_CORNER_PERMS; i++)
//     {
//         data.find_position_permutation_from_rank(i, selected_corner_perm_state);
//         for (int j = 0; j < NUM_MOVES; j++)
//         {
//             data.cube.corners = selected_corner_perm_state;
//             data.cube.input_move(moves(j));
//             data.find_position_permutation_list();
//             corner_perm_table[i][j] = data.rank(data.position_permutation_list);
//         }
//     }
// }

// {
// TODO: Used for co-ordinate mappings for Korf's Algorithm
// template <>
// void lehmer_data<8, 8, rubicks_2x2>::find_position_permutation_from_rank(unsigned int perm_state,
//                                                                          uint64_t &corner_perm_state)
//     {
//         const uint8_t ELEMENT_LENGTH = 6;
//         const int NUM_ELEMENTS = 8;

//         /**
//          * 1. De-rank the rank to generate the permutation list
//          * 2. Iterate through the permutation list, to generate the states in reverse
//          */

//         for (int i = 0; i < NUM_ELEMENTS; i++)
//         {
//             position_permutation_list[i] = perm_state / factorial_list[i];
//             perm_state -= factorial_list[i] * position_permutation_list[i];
//         }

//         int indexes[NUM_ELEMENTS];

//         for (int i = 0; i < NUM_ELEMENTS; i++)
//         {
//             int count = position_permutation_list[i];
//             for (int j = i + 1; j < NUM_ELEMENTS; j++)
//             {
//                 if (indexes[i] > indexes[j])
//                     count++;
//             }
//         }

//         for (int i = 0; i < NUM_ELEMENTS; i++)
//         {
//             corner_perm_state = (corner_perm_state << ELEMENT_LENGTH) | (uint8_t)((corner)(indexes[i]));
//         }
//     }
// }