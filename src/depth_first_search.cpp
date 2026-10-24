#include "../include/depth_first_search.h"
#include "../include/generate_data.h"
#include "../include/generate_scramble.h"
#include "../include/lehmer_code.h"
#include "../include/rubicks_2x2.h"
#include <fcntl.h>
#include <iostream>
#include <queue>
#include <stack>
#include <sys/mman.h>
#include <unistd.h>

int moves_till_solved(const rubicks_2x2 &cube)
{
    int fd = open(CORNERS_DB_FILENAME.c_str(), O_RDONLY);
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

    // std::cout << "The index is: " << index << "\n";

    const unsigned char BYTE = corner_depths[index >> 1];

    if (index & 1)
    {
        distance_from_solved = (BYTE >> 4) & 0b1111;
    }
    else
    {
        distance_from_solved = BYTE & 0b1111;
    }
    // std::cout << std::bitset<8>(BYTE) << "\n";

    munmap(mapping, POSSIBLE_2X2_STATES / 2);
    close(fd);
    return distance_from_solved;
}

int heuristic(const rubicks_2x2 cube)
{
    return moves_till_solved(cube);
}

void iterative_depthening_a_star(rubicks_2x2 cube)
{
    std::stack<node> node_stack;
    moves moves_list[21] = {};
    bool solved = false;
    int next_bound = heuristic(cube);
    int bound = 0;
    int estimated_moves = 0;
    node current_node;

    while (!solved)
    {
        if (node_stack.empty())
        {
            node_stack.push(node{cube, INVALID_MOVE, 0});
            bound = next_bound;
            next_bound = 100; // i.e. inf
        }

        current_node = node_stack.top();
        node_stack.pop();

        // std::cout << "The current node has Move: " << print_move(current_node.move) << " Depth: " <<
        // current_node.depth
        //           << "\n";

        if (current_node.depth > 0)
        {
            moves_list[current_node.depth - 1] = current_node.move;
        }
        moves_list[current_node.depth] = INVALID_MOVE;

        if (current_node.depth == bound)
        {
            if (current_node.cube.is_solved())
            {
                solved = true;

                std::cout << "\nMoves Used: ";

                for (int i = 0; moves_list[i] != INVALID_MOVE; i++)
                {
                    std::cout << print_move(moves_list[i]) << " ";
                }
                std::cout << "\n\n";

                current_node.cube.print_cube();
                return;
            }
        }
        else
        {
            std::priority_queue<node> successors;
            for (int i = 0; i < NUM_MOVES; i++)
            {
                if (current_node.depth == 0 || !prune(moves(i), current_node.move))
                {
                    rubicks_2x2 new_cube = rubicks_2x2(current_node.cube.corners);
                    new_cube.input_move(moves(i));
                    estimated_moves = current_node.depth + heuristic(new_cube);
                    if (estimated_moves <= bound)
                    {
                        successors.push(node{new_cube, moves(i), estimated_moves});
                    }
                    else if (estimated_moves < next_bound)
                    {
                        next_bound = estimated_moves;
                    }
                }
            }
            while (!successors.empty())
            {
                node successor = successors.top();
                successors.pop();
                node_stack.push(node{successor.cube, successor.move, current_node.depth + 1});
            }
        }
    }
}

int main()
{
    srand(time(0));

    rubicks_2x2 cube;

    std::string scramble = generate_scramble(50);

    std::cout << "Scramble: " << scramble << "\n\n";

    cube.input_scramble(scramble);
    cube.print_cube();

    // int moves_required = moves_till_solved(cube);
    // std::cout << "Cube is " << moves_required << " moves away from being solved\n";
    iterative_depthening_a_star(cube);
}
