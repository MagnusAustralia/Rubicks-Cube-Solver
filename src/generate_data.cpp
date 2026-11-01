#include "../include/generate_data.h"
#include "../include/lehmer_code.h"
#include "../include/rubicks_2x2.h"
#include "../include/rubicks_3x3.h"
#include <cstdlib>
#include <iostream>
#include <queue>
#include <stdio.h>
#include <unordered_map>

void process_moves(const cube_state_data &INITIAL_CUBE_STATE, std::queue<rubicks_3x3> &states,
                   std::unordered_map<uint32_t, uint8_t> &known_states, const uint8_t depth, int &num_states,
                   const pdb_type PDB)
{
    switch (PDB)
    {
    case CORNERS_PDB:
    {
        rubicks_3x3 cube;
        lehmer_data data(CORNERS_PDB);
        for (int i = 0; i < NUM_MOVES; i++)
        {
            data.cube.corners = INITIAL_CUBE_STATE.corners;
            moves current_move = (moves)i;
            data.cube.input_move(current_move);
            uint32_t index = data.lehmer_index();
            if (known_states.find(index) == known_states.end())
            {
                known_states[index] = depth;
                cube.corners = data.cube.corners;
                states.push(cube);
                num_states++;
            }
        }
        break;
    }
    case EDGES_CASE_1_PDB:
    case EDGES_CASE_2_PDB:
    case CORNER_SLICE_PDB:
    case EDGE_SLICE_PDB:
    {
        lehmer_data data(PDB);
        for (int i = 0; i < NUM_MOVES; i++)
        {
            data.cube.edges = INITIAL_CUBE_STATE.edges;
            moves current_move = (moves)i;
            data.cube.input_move(current_move);
            uint32_t index = data.lehmer_index();
            if (known_states.find(index) == known_states.end())
            {
                known_states[index] = depth;
                states.push(data.cube);
                num_states++;
            }
        }
        break;
    }
    case CORNER_SLICE_PERM_PDB:
    case EDGE_SLICE_PERM_PDB:
    {
        lehmer_data data(PDB);
        for (int i = 0; i < NUM_MOVES; i++)
        {
            data.cube.corners = INITIAL_CUBE_STATE.corners;
            data.cube.edges = INITIAL_CUBE_STATE.edges;
            moves current_move = (moves)i;
            // Filter out illegal moves
            if (current_move != R && current_move != R_PRIME && current_move != L && current_move != L_PRIME &&
                current_move != F && current_move != F_PRIME && current_move != B && current_move != B_PRIME)
            {
                data.cube.input_move(current_move);
                uint32_t index = data.lehmer_index();
                if (known_states.find(index) == known_states.end())
                {
                    known_states[index] = depth;
                    states.push(data.cube);
                    num_states++;
                }
            }
        }
        break;
    }
    }
}

void bfs(uint8_t &depth, std::queue<rubicks_3x3> &current_depth, int &num_states_processd,
         std::unordered_map<uint32_t, uint8_t> &known_states, const pdb_type PDB)
{
    while (!current_depth.empty() || depth == 0)
    {
        // num_states_processd = 0;
        std::queue<rubicks_3x3> new_depth;
        while (!current_depth.empty())
        {
            rubicks_3x3 cube_state = current_depth.front();
            current_depth.pop();
            process_moves(cube_state_data{cube_state.edges, cube_state.corners}, new_depth, known_states, depth,
                          num_states_processd, PDB);
        }

        std::cout << "Depth: " << static_cast<int>(depth) << ", New States: " << num_states_processd << "\n";
        current_depth = std::move(new_depth);
        depth++;
    }
}

void generate_db(const pdb_type PDB)
{
    uint8_t *moves_till_completion;

    std::string filename;
    std::queue<rubicks_3x3> current_depth;
    std::unordered_map<uint32_t, uint8_t> known_states;
    uint8_t depth = 1;
    int num_states_processed = 1;
    cube_case variation = STANDARD_CUBE;
    unsigned int num_possible_states;

    switch (PDB)
    {
    case CORNERS_PDB:
    {
        num_possible_states = POSSIBLE_CORNER_STATES;
        filename = CORNERS_DB_FILENAME;
        break;
    }
    case EDGES_CASE_1_PDB:
    {
        num_possible_states = POSSIBLE_EDGE_STATES;
        filename = EDGES_1_DB_FILENAME;
        variation = CASE_ONE_3X3;
        break;
    }
    case EDGES_CASE_2_PDB:
    {
        num_possible_states = POSSIBLE_EDGE_STATES;
        filename = EDGES_2_DB_FILENAME;
        variation = CASE_TWO_3X3;
        break;
    }
    case CORNER_SLICE_PDB:
    {
        num_possible_states = POSSIBLE_CORNER_SLICE_STATES;
        filename = CORNERS_SLICE_DB_FILENAME;
        variation = CASE_MIDDLE_SLICE_3X3;
        break;
    }
    case EDGE_SLICE_PDB:
    {
        num_possible_states = POSSIBLE_EDGE_SLICE_STATES;
        filename = EDGES_SLICE_DB_FILENAME;
        break;
    }
    case CORNER_SLICE_PERM_PDB:
    {
        num_possible_states = POSSIBLE_CORNER_SLICE_PERM_STATES;
        filename = CORNERS_SLICE_PERM_DB_FILENAME;
        break;
    }
    case EDGE_SLICE_PERM_PDB:
    {
        num_possible_states = POSSIBLE_EDGE_SLICE_PERM_STATES;
        filename = EDGES_SLICE_PERM_DB_FILENAME;
        break;
    }
    }

    lehmer_data indexer(PDB);
    known_states[indexer.lehmer_index()] = 0;

    moves_till_completion = (uint8_t *)calloc(num_possible_states / 2, sizeof(*moves_till_completion));

    rubicks_3x3 cube = rubicks_3x3(variation);

    current_depth.push(cube);

    bfs(depth, current_depth, num_states_processed, known_states, PDB);

    std::cout << "Onto Keys\n";

    for (const auto &[INDEX, VALUE] : known_states)
    {
        if (INDEX < num_possible_states)
        {
            if (INDEX & 1)
            {
                moves_till_completion[INDEX >> 1] |= ((VALUE & 0b1111) << 4);
            }
            else
            {
                moves_till_completion[INDEX >> 1] |= (VALUE & 0b1111);
            }
        }
    }

    FILE *file = fopen(filename.c_str(), "wb");
    if (!file)
    {
        free(moves_till_completion);
        std::cerr << "Failed to open output file\n";
        return;
    }

    size_t r1 = fwrite(moves_till_completion, 1, num_possible_states / 2, file);
    printf("Wrote %zu bytes out of %d requested\n", r1, num_possible_states / 2);
    fclose(file);
    free(moves_till_completion);
}

int main(int argc, char *argv[])
{
    if (std::string(argv[1]) == "corners")
    {
        std::cout << "Generating corners database: \n\n";
        generate_db(CORNERS_PDB);
    }
    else if (std::string(argv[1]) == "edges-1")
    {
        std::cout << "Generating edges-case-1 database: \n\n";
        generate_db(EDGES_CASE_1_PDB);
    }
    else if (std::string(argv[1]) == "edges-2")
    {
        std::cout << "Generating edges-case-2 database: \n\n";
        generate_db(EDGES_CASE_2_PDB);
    }
    else if (std::string(argv[1]) == "corners-slice")
    {
        std::cout << "Generating corners-slice database: \n\n";
        generate_db(CORNER_SLICE_PDB);
    }
    else if (std::string(argv[1]) == "edges-slice")
    {
        std::cout << "Generating edges-slice database: \n\n";
        generate_db(EDGE_SLICE_PDB);
    }
    else if (std::string(argv[1]) == "corners-slices-perm")
    {
        std::cout << "Generating corners-slice-perm database: \n\n";
        generate_db(CORNER_SLICE_PERM_PDB);
    }
    else if (std::string(argv[1]) == "edges-slices-perm")
    {
        std::cout << "Generating edges-slice-perm database: \n\n";
        generate_db(EDGE_SLICE_PERM_PDB);
    }
    else
    {
        std::cerr << "Invalid arguments to generate_data.cpp\n";
    }

    return 0;
}