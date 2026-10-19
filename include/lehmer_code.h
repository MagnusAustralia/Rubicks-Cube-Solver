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

    lehmer_data(rubicks_2x2 cube_choice, int num_considered = num_elements)
    {
        cube = rubicks_2x2();
        elements = &cube.corners;
        elements_considered = num_considered;

        for (int i = 0; i < num_elements; i++)
        {
            factorial_list[i] = permutate(num_elements - 1 - i, num_considered - 1 - i);
        }
    }

    lehmer_data(rubicks_3x3 cube_choice, int num_considered = num_elements)
    {
        cube = rubicks_3x3();
        elements = &cube.corners;
        elements_considered = num_considered;

        for (int i = 0; i < num_elements; i++)
        {
            factorial_list[i] = permutate(num_elements - 1 - i, num_considered - 1 - i);
        }
    }

    /**
     *
     */
    int get_num_elements();

    /**
     *
     */
    void find_permuatation_list(int (&permutation_list)[num_elements], const uint64_t mask, const int element_length);

    /**
     *
     */
    void find_position_permutation_list();

    /**
     *
     */
    void find_orientation_permutation_list();

    /**
     *
     */
    int rank(int (&permutation_list)[num_elements]);

    /**
     *
     */
    int lehmer_index();
};

#endif