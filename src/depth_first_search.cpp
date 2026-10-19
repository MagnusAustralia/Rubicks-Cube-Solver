#include "../include/depth_first_search.h"
#include "../include/generate_data.h"
#include "../include/lehmer_code.h"
#include "../include/rubicks_2x2.h"
#include <fcntl.h>
#include <iostream>
#include <stack>
#include <sys/mman.h>
#include <unistd.h>

int moves_till_solved(const rubicks_2x2 &cube)
{
    int fd = open("./data/2x2.bin", O_RDONLY);
    if (fd == -1)
    {
        std::cerr << "open failed\n";
        return -1;
    }
    void *mapping = mmap(nullptr, POSSIBLE_2X2_STATES / 2, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapping == MAP_FAILED)
    {
        std::cerr << "mmap failed";
    }
    const unsigned char *corner_depths = static_cast<const unsigned char *>(mapping);

    int distance_from_solved = 0;

    lehmer_data<8, rubicks_2x2> indexer{rubicks_2x2()};
    indexer.cube.corners = cube.corners;
    int index = indexer.lehmer_index();

    std::cout << "The index is: " << index << "\n";

    const unsigned char BYTE = corner_depths[index >> 1];

    if (index & 1)
    {
    	distance_from_solved = (BYTE >> 4) & 0b1111;
    }
    else
    {
    	distance_from_solved = BYTE & 0b1111;
    }
    std::cout << std::bitset<8>(BYTE) << "\n";

    munmap(mapping, POSSIBLE_2X2_STATES / 2);
    close(fd);
    return distance_from_solved;
}

// int heuristic(const rubicks_2x2 cube)
// {
//     return 0;
// }

// void iterative_depthening_a_star(rubicks_2x2 cube)
// {
//     std::stack<node> node_stack;
//     moves moves_list[21];
//     bool solved = false;
//     int next_bound = heuristic(cube);
// }

int main()
{
    rubicks_2x2 cube;
    // cube.input_scramble("L");
    cube.print_cube();
    int moves_required = moves_till_solved(cube);
    std::cout << "Cube is " << moves_required << " moves away from being solved\n";
}
