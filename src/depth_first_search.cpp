#include "../include/depth_first_search.h"
#include "../include/generate_data.h"
#include "../include/generate_scramble.h"
#include "../include/lehmer_code.h"
#include "../include/rubicks_2x2.h"
#include <fcntl.h>
#include <iostream>
#include <stack>
#include <sys/mman.h>
#include <unistd.h>

#include <chrono>
#include <cstdint>
#include <iomanip>

using std::chrono::duration;
using std::chrono::steady_clock;

void print_iteration(int bound, steady_clock::time_point start, const char *label = "")
{
    double secs = duration<double>(steady_clock::now() - start).count();
    double ratio = g_stats.prev_expanded ? double(g_stats.expanded) / g_stats.prev_expanded : 0.0;
    std::cout << "  iter " << g_stats.iteration << " bound=" << bound << " expanded=" << g_stats.expanded
              << " generated=" << g_stats.generated << " kept=" << g_stats.kept << " ratio=" << std::fixed
              << std::setprecision(2) << ratio << " time=" << secs << "s " << label << "\n";

    g_stats.total_expanded += g_stats.expanded;
    g_stats.total_generated += g_stats.generated;
    g_stats.total_rem1 += g_stats.rem1;
    g_stats.total_rem2 += g_stats.rem2;
    g_stats.prev_expanded = g_stats.expanded;
}

unsigned char *open_file(const std::string FILENAME, const long NUM_POSSIBLE_STATES)
{
    int fd = open(FILENAME.c_str(), O_RDONLY);
    if (fd == -1)
    {
        std::cerr << "open failed\n";
    }
    void *mapping = mmap(nullptr, NUM_POSSIBLE_STATES / 2, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapping == MAP_FAILED)
    {
        std::cerr << "mmap failed";
    }
    close(fd);

    return static_cast<unsigned char *>(mapping);
}

void open_data_files(pattern_dbs &db)
{
    db.corner_depths = open_file(CORNERS_DB_FILENAME, POSSIBLE_CORNER_STATES);
    db.edge_case_1_depths = open_file(EDGES_1_DB_FILENAME, POSSIBLE_EDGE_STATES);
    db.edge_case_2_depths = open_file(EDGES_2_DB_FILENAME, POSSIBLE_EDGE_STATES);
    db.corner_slice_depths = open_file(CORNERS_SLICE_DB_FILENAME, POSSIBLE_CORNER_SLICE_STATES);
    db.edge_slice_depths = open_file(EDGES_SLICE_DB_FILENAME, POSSIBLE_EDGE_SLICE_STATES);
}

void close_data_files(pattern_dbs &db)
{
    munmap(static_cast<void *>(db.corner_depths), POSSIBLE_CORNER_STATES / 2);
    munmap(static_cast<void *>(db.edge_case_1_depths), POSSIBLE_EDGE_STATES / 2);
    munmap(static_cast<void *>(db.edge_case_2_depths), POSSIBLE_EDGE_STATES / 2);
    munmap(static_cast<void *>(db.corner_slice_depths), POSSIBLE_CORNER_SLICE_STATES / 2);
    munmap(static_cast<void *>(db.edge_slice_depths), POSSIBLE_EDGE_SLICE_STATES / 2);
}

int moves_till_solved(const rubicks_2x2 &cube, const pattern_dbs &db)
{

    int distance_from_solved = 0;

    lehmer_data<8, 8, rubicks_2x2> indexer;
    indexer.cube.corners = cube.corners;
    int index = indexer.lehmer_index();

    const unsigned char BYTE = db.corner_depths[index >> 1];

    if (index & 1)
    {
        distance_from_solved = (BYTE >> 4) & 0b1111;
    }
    else
    {
        distance_from_solved = BYTE & 0b1111;
    }

    return distance_from_solved;
}

int moves_till_solved(const rubicks_3x3 &cube, const pattern_dbs &db, const cube_case variation, const pdb_type PBD)
{
    std::string filename;

    int distance_from_solved = 0;
    int index = 0;
    unsigned char byte;

    if (variation == CASE_MIDDLE_SLICE_3X3 && PBD == CORNER_SLICE_PDB)
    {
        lehmer_data<12, 4, rubicks_3x3> indexer(variation);
        indexer.cube = cube;
        index = indexer.lehmer_index();
        byte = db.corner_slice_depths[index >> 1];
    }
    else if (variation == CASE_MIDDLE_SLICE_3X3 && PBD == EDGE_SLICE_PDB)
    {
        // lehmer_data<12, 12, rubicks_3x3> indexer(variation); TEMP FIX
        lehmer_data<12, 4, rubicks_3x3> indexer(variation);
        indexer.cube = cube;
        index = indexer.lehmer_index();
        byte = db.edge_slice_depths[index >> 1];
    }
    else
    {
        lehmer_data<12, 6, rubicks_3x3> indexer(variation);
        indexer.cube.edges = cube.edges;
        index = indexer.lehmer_index();
        byte = (variation == CASE_ONE_3X3) ? db.edge_case_1_depths[index >> 1] : db.edge_case_2_depths[index >> 1];
    }

    if (index & 1)
    {
        distance_from_solved = (byte >> 4) & 0b1111;
    }
    else
    {
        distance_from_solved = byte & 0b1111;
    }

    return distance_from_solved;
}

int heuristic_korf(const rubicks_3x3 &cube, const pattern_dbs &db, const int MAX_HEURISTIC)
{
    constexpr uint64_t EDGES_MASK = 0b11111ULL;
    uint64_t edges_case_1 = cube.edges;
    uint64_t edges_case_2 = cube.edges;
    rubicks_2x2 corners_cube(cube.corners);
    rubicks_3x3 edges_cube;

    for (int i = 0; i < NUM_EDGES; i++)
    {
        int current_edge = (cube.edges >> (EDGE_WIDTH * i)) & EDGE_PIECE_MASK;
        if (current_edge >= 6)
        {
            edges_case_1 = (edges_case_1 & ~(EDGES_MASK << (EDGE_WIDTH * i))) | (uint64_t(NN) << (EDGE_WIDTH * i));
        }
        else
        {
            edges_case_2 = (edges_case_2 & ~(EDGES_MASK << (EDGE_WIDTH * i))) | (uint64_t(NN) << (EDGE_WIDTH * i));
        }
    }

    edges_cube.edges = edges_case_1;
    edges_cube.corners = cube.corners;

    int h1 = moves_till_solved(corners_cube, db);
    if (h1 > MAX_HEURISTIC)
    {
        g_stats.exit_corner++;
        return h1;
    }

    // std::cout << "H1: " << h1 << "\n";
    int h2 = moves_till_solved(edges_cube, db, CASE_ONE_3X3, EDGES_CASE_1_PDB);
    if (h2 > MAX_HEURISTIC)
    {
        g_stats.exit_edge1++;
        return h2;
    }

    // std::cout << "H2: " << h2 << "\n";
    edges_cube.edges = edges_case_2;
    int h3 = moves_till_solved(edges_cube, db, CASE_TWO_3X3, EDGES_CASE_2_PDB);
    // std::cout << "H3: " << h3 << "\n";

    if (h1 > h2 && h1 > h3)
    {
        g_stats.win_corner++;
        return h1;
    }
    if (h2 > h3)
    {
        g_stats.win_edge1++;
        return h2;
    }
    g_stats.win_edge2++;
    return h3;
}

int heuristic_kociemba(const rubicks_3x3 &cube, const pattern_dbs &db, const int MAX_HEURISTIC)
{
    constexpr uint64_t EDGES_MASK = 0b11111ULL;
    uint64_t slice_edges = cube.edges;
    rubicks_3x3 corner_slice_cube;
    rubicks_3x3 edge_slice_cube = cube;

    for (int i = 0; i < NUM_EDGES; i++)
    {
        int current_edge = (cube.edges >> (EDGE_WIDTH * i)) & EDGE_PIECE_MASK;
        if (current_edge < 4 || current_edge >= 8)
        {
            slice_edges = (slice_edges & ~(EDGES_MASK << (EDGE_WIDTH * i))) | (uint64_t(NN) << (EDGE_WIDTH * i));
        }
    }

    corner_slice_cube.edges = slice_edges;
    corner_slice_cube.corners = cube.corners;

    int h1 = moves_till_solved(corner_slice_cube, db, CASE_MIDDLE_SLICE_3X3, CORNER_SLICE_PDB);
    if (h1 > MAX_HEURISTIC)
    {
        return h1;
    }

    int h2 = moves_till_solved(edge_slice_cube, db, CASE_MIDDLE_SLICE_3X3, EDGE_SLICE_PDB);

    if (h1 > h2)
        return h1;

    return h2;
}

void korfs_algorithm(rubicks_3x3 cube, const pattern_dbs &db)
{
    std::stack<node> node_stack;
    moves moves_list[21] = {};
    bool solved = false;
    int next_bound = heuristic_korf(cube, db, INFINITE_DEPTH);
    int bound = 0;
    int estimated_moves = 0;
    node current_node;
    const int ROOT_H = next_bound;
    g_stats = stats{}; // reset AFTER the root heuristic call
    auto scramble_start = steady_clock::now();
    auto iter_start = scramble_start;

    while (!solved)
    {
        if (node_stack.empty())
        {
            if (g_stats.iteration > 0)
                print_iteration(bound, iter_start);
            g_stats.iteration++;
            g_stats.expanded = g_stats.generated = g_stats.kept = g_stats.rem1 = g_stats.rem2 = 0;
            iter_start = steady_clock::now();
            node_stack.push(node{cube, INVALID_MOVE, 0});
            bound = next_bound;
            next_bound = INFINITE_DEPTH; // i.e. inf
        }

        current_node = node_stack.top();
        node_stack.pop();

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

                // std::cout << "\nMoves Used: ";

                // for (int i = 0; moves_list[i] != INVALID_MOVE; i++)
                // {
                //     std::cout << print_move(moves_list[i]) << " ";
                // }
                // std::cout << "\n\n";

                print_iteration(bound, iter_start, "(partial: stopped at goal)");
                double total_secs = duration<double>(steady_clock::now() - scramble_start).count();
                std::cout << "  ROOT_H=" << ROOT_H << " solution_len=" << current_node.depth
                          << " gap=" << (current_node.depth - ROOT_H) << "\n"
                          << "  total_expanded=" << g_stats.total_expanded
                          << " total_generated=" << g_stats.total_generated
                          << " nodes/s=" << uint64_t(g_stats.total_expanded / total_secs) << "\n"
                          << "  frontier share: rem1=" << 100.0 * g_stats.total_rem1 / g_stats.total_expanded
                          << "% rem2=" << 100.0 * g_stats.total_rem2 / g_stats.total_expanded << "%\n"
                          << "  heuristic: exit_corner=" << g_stats.exit_corner << " exit_edge1=" << g_stats.exit_edge1
                          << " full=" << g_stats.full_eval << " wins c/e1/e2=" << g_stats.win_corner << "/"
                          << g_stats.win_edge1 << "/" << g_stats.win_edge2 << "\n";

                // current_node.cube.print_cube();
                return;
            }
        }
        else
        {
            g_stats.expanded++;
            int remaining = bound - current_node.depth;
            if (remaining == 1)
                g_stats.rem1++;
            else if (remaining == 2)
                g_stats.rem2++;
            std::stack<node> successors;
            for (int i = 0; i < NUM_MOVES; i++)
            {
                if (current_node.depth == 0 || !prune(moves(i), current_node.move))
                {
                    g_stats.generated++;
                    rubicks_3x3 new_cube;
                    new_cube.edges = current_node.cube.edges;
                    new_cube.corners = current_node.cube.corners;
                    new_cube.input_move(moves(i));
                    estimated_moves =
                        current_node.depth + 1 + heuristic_korf(new_cube, db, bound - current_node.depth - 1);
                    if (estimated_moves <= bound)
                    {
                        g_stats.kept++;
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

void kociembas_algorithm(rubicks_3x3 cube, const pattern_dbs &db)
{
    std::stack<node> node_stack;
    moves moves_list[21] = {};
    bool solved = false;
    int next_bound = heuristic_kociemba(cube, db, INFINITE_DEPTH);
    int bound = 0;
    int estimated_moves = 0;
    node current_node;

    while (!solved)
    {
        if (node_stack.empty())
        {
            node_stack.push(node{cube, INVALID_MOVE, 0});
            bound = next_bound;
            next_bound = INFINITE_DEPTH; // i.e. inf
        }

        current_node = node_stack.top();
        node_stack.pop();

        if (current_node.depth > 0)
        {
            moves_list[current_node.depth - 1] = current_node.move;
        }
        moves_list[current_node.depth] = INVALID_MOVE;

        if (current_node.depth == bound)
        {
            if (current_node.cube.is_solved() || heuristic_kociemba(current_node.cube, db, INFINITE_DEPTH) == 0)
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
            std::stack<node> successors;
            for (int i = 0; i < NUM_MOVES; i++)
            {
                if (current_node.depth == 0 || !prune(moves(i), current_node.move))
                {
                    rubicks_3x3 new_cube;
                    new_cube.edges = current_node.cube.edges;
                    new_cube.corners = current_node.cube.corners;
                    new_cube.input_move(moves(i));
                    estimated_moves =
                        current_node.depth + 1 + heuristic_kociemba(new_cube, db, bound - current_node.depth - 1);
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

    // const std::string SCRAMBLES[15] = {
    //     "U2 R' L' U2 R L F U' F' L2 F2 U'",  "F' L' F2 L U R L2 U F R2 L U",     "F L F2 U2 L F' R2 F L2 F U R2",
    //     "F2 R2 F' L2 U R U2 F' R L2 F' L",   "R L U L' U2 R2 L2 U2 R2 U2 F2 U",  "U' F' U' F2 L U2 F2 R2 F R U2 F'",
    //     "R F R2 U2 R U2 L2 F' R' F' R L'",   "R2 F R2 U F L U2 F U R' F2 L'",    "L' U' R' F2 R' F U L F R2 L U'",
    //     "R' U2 F' R' F' R2 L U2 R2 L F L",   "F' U' L F L' F2 U R2 L2 F' L F'",  "L2 U' R2 U' F' U' L U' F2 U R2 F'",
    //     "F2 U2 L U2 F' L F2 U2 R' L2 U' R'", "R2 U F' L2 U2 R U' L' F U' F2 U2", "U' R2 L U2 R' F' R2 L2 F2 R2 L2
    //     F2"};

    pattern_dbs db;

    open_data_files(db);

    // std::string scramble = generate_scramble();
    std::string scramble = "B R L U R2 L U' F' U' L2 B U F2 L B'";

    rubicks_3x3 cube;

    cube.input_scramble(scramble);

    // cube.print_cube();
    std::cout << "Scramble: " << scramble << "\n\n";

    // std::cout << "Moves till solved: " << heuristic_kociemba(cube, db) << "\n";

    kociembas_algorithm(cube, db);

    // uint64_t suite_expanded = 0;
    // auto suite_start = steady_clock::now();

    // // for (std::string scramble : SCRAMBLES)
    // // {
    // std::cout << "Scramble " << scramble << "\n";
    // cube.input_scramble(scramble);
    // auto t1 = steady_clock::now();
    // korfs_algorithm(cube, db);
    // auto t2 = steady_clock::now();
    // double secs = duration<double>(t2 - t1).count();
    // std::cout << "Time: " << secs << "s\n\n";
    // suite_expanded += g_stats.total_expanded;
    // // }
    // std::cout << "SUITE total_expanded=" << suite_expanded
    //           << " time=" << duration<double>(steady_clock::now() - suite_start).count() << "s\n";

    // cube.print_cube();

    close_data_files(db);
}
