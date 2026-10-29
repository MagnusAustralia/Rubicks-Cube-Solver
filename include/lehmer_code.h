#ifndef LEHMER_CODE_H
#define LEHMER_CODE_H

#include "../include/rubicks_2x2.h"
#include "../include/rubicks_3x3.h"
#include <iostream>

const int NUM_CORNER_PERMS = 40'320; // 8P8
const int NUM_CORNER_ORIENTS = 2187; // 3^7
const int NUM_EDGE_ORIENTS = 2048;   // 2^11
const int NUM_SLICE_PERMS = 24;      // 4P4

/**
 * Given an input n, this function returns it's factorial
 *
 * @param n The input number
 *
 * @return The factorial
 */
uint32_t factorial(int n);

/**
 * Given an input n and k, this function returns it's combination (i.e. nCk)
 *
 * @param n The total number of elements
 * @param k The number of elements considered
 *
 * @return The number of combinations of n and k
 */
uint32_t choose(int n, int k);

/**
 * Given an input n and k, this function returns it's perumataion (i.e. nPk)
 *
 * @param n The total number of elements
 * @param k The number of elements considered
 *
 * @return The number of permutations of n and k
 */
uint32_t permutate(int n, int k);

/**
 * A struct which is used to find the Lehmer Index of a Rubick's Cube state
 *
 * @field factorial_list[num_considered]                A pre-computed list to calculate the rank
 * @field position_permutation_list[num_considered]     A list storing the calculated position permutation list
 * @field considered_element_index_list[num_considered] A list storing the indexes of elements considered in a partial
 * permutation
 * @field cube_type                                     A store of a Rubicks cube to caclulate based off of
 * @field cube_variation                                The type of the cube (used to handle special cube cases)
 */
template <int num_elements, int num_considered, typename cube_type> struct lehmer_data
{
    uint32_t factorial_list[num_considered];
    int position_permutation_list[num_considered];
    int considered_element_index_list[num_considered];
    cube_type cube;
    cube_case cube_variation;

    lehmer_data(const cube_case variation = STANDARD_CUBE)
    {
        cube = cube_type(variation);
        cube_variation = variation;
        for (int i = 0; i < num_considered; i++)
        {
            factorial_list[i] = permutate(num_elements - 1 - i, num_considered - 1 - i);
        }
    }

    /**
     * Given the cube variation of the lehner_data, the indexes of the considered elements are found, and the
     * consider_elements_index_list is updated accordingly.
     */
    void find_considered_elements_index_list();

    /**
     * Calculates the position_permutation_list
     */
    void find_position_permutation_list();

    /**
     * Given the orientations of the cube state, calculates the rank of that orientation
     *
     * @return The orientation rank
     */
    int rank_orientations();

    /**
     * Given the position_permutation_list and the precomputed factorials list, calculates the rank of the position
     * permutation
     *
     * @return The position permutation rank
     */
    int rank();

    /**
     * Finds and calculates the lehmer index of the cube state
     *
     * @return The lehmer index
     */
    int lehmer_index();

    /**
     * Finds the partial permutation of the four edges: FL, FR, BL and BR
     *
     * @return A permutation rank for the middle slice
     */
    int find_middle_slice();

    /**
     *
     */
    int find_middle_slice_permutation_rank();

    /**
     *
     */
    // void find_position_permutation_from_rank(unsigned int perm_state, uint64_t &corner_perm_state);
};

template <> int lehmer_data<8, 8, rubicks_2x2>::lehmer_index();   // Korf's Algorithm: Corners PDB
template <> int lehmer_data<12, 6, rubicks_3x3>::lehmer_index();  // Korf's Algorithm: Edges PDB
template <> int lehmer_data<8, 8, rubicks_3x3>::lehmer_index();   //
template <> int lehmer_data<12, 4, rubicks_3x3>::lehmer_index();  // Kociemba's Algorithm: Corner-Slice PDB
template <> int lehmer_data<8, 6, rubicks_3x3>::lehmer_index();   // Kociemba's Algorithm: Corner-Slice-Perm PDB
template <> int lehmer_data<12, 12, rubicks_3x3>::lehmer_index(); // Kociemba's Algorithm: Corner-Slice-Perm PDB

#endif