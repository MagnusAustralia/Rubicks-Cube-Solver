#include "../include/generate_data.h"
#include "../include/lehmer_code.h"
#include "../include/rubicks_2x2.h"
#include "../include/rubicks_3x3.h"
#include <cstdlib>
#include <iostream>
#include <queue>
#include <stdio.h>
#include <unordered_map>

template <typename T>
void process_moves(T &cube, std::queue<T> &states, std::unordered_map<uint64_t, uint8_t> &known_states, uint8_t depth,
                   int &num_states)
{
    const uint64_t INITIAL_CUBE_STATE = cube.corners;
    for (int i = 0; i < NUM_MOVES; i++)
    {
        cube.input_move((moves)i);
        if (known_states.find(cube.corners) == known_states.end())
        {
            known_states[cube.corners] = depth;
            states.push(cube);
            num_states++;
        }
        cube.corners = INITIAL_CUBE_STATE;
    }
}

template <typename T>
void bfs(uint8_t &depth, std::queue<T> &current_depth, int &num_states_processd,
         std::unordered_map<uint64_t, uint8_t> &known_states)
{
    while (!current_depth.empty())
    {
        std::queue<T> new_depth;
        while (!current_depth.empty())
        {
            T cube_state = current_depth.front();
            current_depth.pop();
            process_moves<T>(cube_state, new_depth, known_states, depth, num_states_processd);
        }

        std::cout << "Depth: " << static_cast<int>(depth) << ", States: " << num_states_processd << "\n";
        current_depth = std::move(new_depth);
        depth++;
    }
}

void generate_corner_db()
{
    uint8_t *moves_till_completion;
    moves_till_completion = (uint8_t *)calloc(POSSIBLE_2X2_STATES / 2, sizeof(*moves_till_completion));

    std::queue<rubicks_2x2> current_depth;
    uint8_t depth = 1;
    int num_states_processed = 1;
    rubicks_2x2 cube;

    std::unordered_map<uint64_t, uint8_t> known_states;
    known_states[cube.corners] = 0;
    current_depth.push(cube);

    bfs<rubicks_2x2>(depth, current_depth, num_states_processed, known_states);

    std::cout << "Onto Keys\n";

    for (const auto &[KEY, VALUE] : known_states)
    {
        lehmer_data<8, rubicks_2x2> indexer{cube};
        indexer.cube.corners = KEY;
        int index = indexer.lehmer_index();
        if (index & 1)
        {
            moves_till_completion[index >> 1] |= ((VALUE & 0b1111) << 4);
        }
        else
        {
            moves_till_completion[index >> 1] |= (VALUE & 0b1111);
        }
    }

    FILE *file = fopen("./data/2x2.bin", "wb");
    if (!file)
    {
        free(moves_till_completion);
        std::cerr << "Failed to open output file\n";
        return;
    }

    size_t r1 = fwrite(moves_till_completion, 1, POSSIBLE_2X2_STATES / 2, file);
    printf("Wrote %zu bytes out of %d requested\n", r1, POSSIBLE_2X2_STATES / 2);
    fclose(file);
    free(moves_till_completion);
}

void generate_edges_db()
{
    uint8_t *moves_till_completion;
    moves_till_completion = (uint8_t *)calloc(POSSIBLE_3X3_STATES / 2, sizeof(*moves_till_completion));

    std::queue<rubicks_3x3> current_depth;
    uint8_t depth = 1;
    int num_states_processed = 1;
    rubicks_3x3 cube;

    std::unordered_map<uint64_t, uint8_t> known_states;
    known_states[cube.corners] = 0;
    current_depth.push(cube);

    bfs<rubicks_3x3>(depth, current_depth, num_states_processed, known_states);

    std::cout << "Onto Keys\n";

    for (const auto &[KEY, VALUE] : known_states)
    {
        lehmer_data<8, rubicks_3x3> indexer{cube};
        indexer.cube.corners = KEY;
        int index = indexer.lehmer_index();
        if (index & 1)
        {
            moves_till_completion[index >> 1] |= ((VALUE & 0b1111) << 4);
        }
        else
        {
            moves_till_completion[index >> 1] |= (VALUE & 0b1111);
        }
    }

    FILE *file = fopen("./data/edges.bin", "wb");
    if (!file)
    {
        free(moves_till_completion);
        std::cerr << "Failed to open output file\n";
        return;
    }

    size_t r1 = fwrite(moves_till_completion, 1, POSSIBLE_3X3_STATES / 2, file);
    printf("Wrote %zu bytes out of %d requested\n", r1, POSSIBLE_3X3_STATES / 2);
    fclose(file);
    free(moves_till_completion);
}

int main(int argc, char *argv[])
{
    if (std::string(argv[1]) == "edges")
    {
        std::cout << "Generating edges database: \n";
        generate_edges_db();
    }
    else
    {
        std::cout << "Generating corners database: \n";
        generate_corner_db();
    }

    return 0;
}