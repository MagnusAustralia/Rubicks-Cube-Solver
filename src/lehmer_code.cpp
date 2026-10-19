#include "../include/lehmer_code.h"
#include "../include/rubicks_2x2.h"
#include "../include/rubicks_3x3.h"
#include <iostream>

int factorial(int n)
{
    if (n <= 1)
        return 1;
    return n * factorial(n - 1);
}

int permutate(int n, int k)
{
    return factorial(n) / (factorial(n - k));
}

template <int num_elements, typename cube_type> int lehmer_data<num_elements, cube_type>::get_num_elements()
{
    return num_elements;
}

template <int num_elements, typename cube_type>
void lehmer_data<num_elements, cube_type>::find_permuatation_list(int (&permutation_list)[num_elements],
                                                                  const uint64_t mask, const int ELEMENT_LENGTH)
{
    int indexes[num_elements];

    uint64_t remaining_elements = *elements;

    for (int i = num_elements - 1; i >= 0; i--)
    {
        indexes[i] = (uint8_t)(remaining_elements & mask);
        remaining_elements >>= ELEMENT_LENGTH;
    }

    for (int i = 0; i < num_elements; i++)
    {
        int count = 0;
        for (int j = i + 1; j < num_elements; j++)
        {
            if (indexes[i] > indexes[j])
                count++;
        }
        permutation_list[i] = count;
    }
}

template <int num_elements, typename cube_type>
void lehmer_data<num_elements, cube_type>::find_position_permutation_list()
{
    const uint64_t ELEMENT_MASK = 0b111;
    const uint8_t ELEMENT_LENGTH = 6;

    find_permuatation_list(position_permutation_list, ELEMENT_MASK, ELEMENT_LENGTH);
}

template <int num_elements, typename cube_type>
void lehmer_data<num_elements, cube_type>::find_orientation_permutation_list()
{
    const uint64_t ELEMENT_MASK = 0b011000;
    const uint8_t ELEMENT_LENGTH = 6;

    find_permuatation_list(orientation_permutation_list, ELEMENT_MASK, ELEMENT_LENGTH);
}

template <int num_elements, typename cube_type>
int lehmer_data<num_elements, cube_type>::rank(int (&permutation_list)[num_elements])
{
    int index = 0;

    for (int i = 0; i < num_elements - 1; i++)
    {
        index += factorial_list[i] * permutation_list[i];
    }

    return index;
}

template <int num_elements, typename cube_type> int lehmer_data<num_elements, cube_type>::lehmer_index()
{
    find_position_permutation_list();
    find_orientation_permutation_list();

    return (rank(position_permutation_list) * 2187) + rank(orientation_permutation_list);
}

// int overall_index(int n)
// {
// 	// For corners:
// 	// 		return (rank(corner_permutation_list) * 3**7) + rank(orientation_permutation_list)
// 	// For Edges:
// 	// 		return (rank(edge_permutation_list) * 2**6) + rank(orientation permutation_list)
// }

int main()
{
    lehmer_data<8, rubicks_2x2> data{rubicks_2x2()};

    data.cube.input_scramble("U");

    std::cout << data.lehmer_index() << "\n";

    return 0;
}

template int lehmer_data<8, rubicks_2x2>::lehmer_index();
template int lehmer_data<8, rubicks_3x3>::lehmer_index();