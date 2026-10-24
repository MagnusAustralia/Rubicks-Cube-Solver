#ifndef LEHMER_CODE_H
#define LEHMER_CODE_H

#include "../include/rubicks_2x2.h"
#include "../include/rubicks_3x3.h"

/**
 * The Lehmer code is a method for efficiently categorising the order of states
 * i.e. Used as a useful indexer to lookup the moves data.
 */

/**
 * Given an input n, this function returns it's factorial
 *
 * @param n The input number
 * @return The factorial
 */
int factorial(int n);

/**
 * Given an input n and k, this function returns it's perumataion (i.e. nPk)
 *
 * @param n The total number of elements
 * @param k The number of elements considered
 * @return The number of permutations of n and k
 */
int permutate(int n, int k);

/**
 *
 */
template <int num_elements, typename cube_type> struct lehmer_data
{
    int elements_considered;
    int factorial_list[num_elements];
    int position_permutation_list[num_elements];
    int orientation_permutation_list[num_elements];
    cube_type cube;
    uint64_t *elements;

    lehmer_data(cube_type cube_choice, int num_considered = num_elements);

    /**
     * Iterates through the
     */
    void find_permuatation_list(int (&permutation_list)[num_elements], const uint64_t mask, const int element_length);

    /**
     *
     */
    void find_position_permutation_list();

    /**
     * This handles
     */
    int rank_orientations(const int states, const uint64_t ELEMENT_MASK, const int ELEMENT_LENGTH,
                          const int SHIFT_WIDTH, const int type = 0);

    /**
     *
     */
    int rank_corner_orientations();

    /**
     *
     */
    int rank_edge_orientations(const int states);

    /**
     *
     */
    int rank(int (&permutation_list)[num_elements]);

    /**
     *
     */
    int lehmer_index(const int type = 0);
};

template <> inline lehmer_data<8, rubicks_2x2>::lehmer_data(rubicks_2x2 cube_choice, int num_considered)
{
    cube = rubicks_2x2();
    elements = &cube.corners;
    elements_considered = num_considered;

    for (int i = 0; i < num_considered; i++)
    {
        factorial_list[i] = permutate(num_considered - 1 - i, num_considered - 1 - i);
    }
}

template <> inline lehmer_data<12, rubicks_3x3>::lehmer_data(rubicks_3x3 cube_choice, int num_considered)
{
    cube = rubicks_3x3();
    elements = &cube.edges;
    elements_considered = num_considered;

    for (int i = 0; i < num_considered; i++)
    {
        factorial_list[i] = permutate(num_considered - 1 - i, num_considered - 1 - i);
    }
}

#endif