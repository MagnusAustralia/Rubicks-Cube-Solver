#include "../include/rubicks_3x3.h"
#include "../include/rubicks_2x2.h"
#include <bitset>
#include <cstring>
#include <iostream>
#include <sstream>

bool test_rotations(const cube_case variation = STANDARD_CUBE)
{
    rubicks_3x3 cube(variation);

    uint64_t expected_edges;

    switch (variation)
    {
    case STANDARD_CUBE:
        expected_edges = SOLVED_EDGES;
        break;
    case CASE_ONE_3X3:
        expected_edges = EDGE_CASE_ONE_3X3;
        break;
    case CASE_TWO_3X3:
        expected_edges = EDGE_CASE_TWO_3X3;
        break;
    case CASE_MIDDLE_SLICE_3X3:
        expected_edges = EDGE_CASE_MIDDLE_SLICE_3X3;
        break;
    }

    bool is_valid = true;
    for (int i = 0; i < 6; i++)
    {
        face f = (face)i;
        for (int j = 0; j < 3; j++)
        {
            direction d = (direction)j;
            cube.rotate(f, d);
            cube.rotate(f, d);
            cube.rotate(f, d);
            cube.rotate(f, d);

            if (!cube.is_solved(variation))
            {
                is_valid = false;
                std::cout << (int)f << "\n";
                std::cout << "Expected: " << std::bitset<64>(SOLVED_CORNERS) << "\n";
                std::cout << "Actual:   " << std::bitset<64>(cube.corners) << "\n";
                std::cout << "Expected: " << std::bitset<64>(expected_edges) << "\n";
                std::cout << "Actual:   " << std::bitset<64>(cube.edges) << "\n";
            }
        }
    }

    if (is_valid)
    {
        std::cout << "All tests passed!!\n";
    }

    return is_valid;
}

std::string get_edge(edge e, orientation o)
{
    int x = (int)o;
    const std::string EDGE_COLOURS[13] = {"WG", "WO", "WB", "WR", "BO", "BR", "GO", "GR", "YG", "YO", "YB", "YR", "  "};

    std::string current_edge = EDGE_COLOURS[uint8_t(e)];
    return current_edge.substr(x, 1) + current_edge.substr((x + 1) % 2, 1);
};

bool rubicks_3x3::is_solved(const cube_case variation) const
{
    uint64_t expected_edges;

    switch (variation)
    {
    case STANDARD_CUBE:
        expected_edges = SOLVED_EDGES;
        break;
    case CASE_ONE_3X3:
        expected_edges = EDGE_CASE_ONE_3X3;
        break;
    case CASE_TWO_3X3:
        expected_edges = EDGE_CASE_TWO_3X3;
        break;
    case CASE_MIDDLE_SLICE_3X3:
        expected_edges = EDGE_CASE_MIDDLE_SLICE_3X3;
        break;
    }

    return corners == SOLVED_CORNERS and edges == expected_edges;
}

void rubicks_3x3::update_corners()
{
    uint64_t remaining_corners = corners;

    for (int i = NUM_CORNERS - 1; i >= 0; i--)
    {
        corners_list[i] = get_corner(static_cast<corner>((uint8_t)(CORNER_PIECE_MASK & remaining_corners)),
                                     static_cast<orientation>((uint8_t)((CORNER_ORIENTATION_MASK & remaining_corners) >>
                                                                        CORNER_ORIENTATION_SHIFT)));
        remaining_corners >>= CORNER_WIDTH;
    }
}

void rubicks_3x3::update_edges()
{
    uint64_t remaining_edges = edges;

    for (int i = NUM_EDGES - 1; i >= 0; i--)
    {
        edges_list[i] = get_edge(
            static_cast<edge>((uint8_t)(EDGE_PIECE_MASK & remaining_edges)),
            static_cast<orientation>((uint8_t)((EDGE_ORIENTATION_MASK & remaining_edges) >> EDGE_ORIENTATION_SHIFT)));
        remaining_edges >>= EDGE_WIDTH;
    }
}

void rubicks_3x3::adjust_orientations(uint64_t adjustment)
{
    for (int i = 0; i < NUM_CORNERS; i++)
    {
        if (adjustment & CORNER_ORIENTATION_MASK)
        {
            int initial_orientation =
                ((corners >> (CORNER_WIDTH * i)) & CORNER_ORIENTATION_MASK) >> CORNER_ORIENTATION_SHIFT;

            int new_orientation = (adjustment & CORNER_ORIENTATION_MASK) >> CORNER_ORIENTATION_SHIFT;

            new_orientation = (initial_orientation + new_orientation) % NUM_CORNER_ORIENTATIONS;

            corners = (corners & ~(CORNER_ORIENTATION_MASK << (6 * i))) |
                      ((uint64_t)new_orientation << (CORNER_ORIENTATION_SHIFT + CORNER_WIDTH * i));
        }
        adjustment >>= CORNER_WIDTH;
    }
}

void rubicks_3x3::rotate(face f, direction d)
{
    uint64_t masked_corners;
    uint64_t masked_edges;

    switch (f)
    {
    case UP:
        masked_corners = corners & UP_CORNER_MASK;
        masked_edges = edges & UP_EDGE_MASK;
        switch (d)
        {
        case CLOCKWISE:
            corners = (((masked_corners >> CORNER_WIDTH) | ((masked_corners << (3 * CORNER_WIDTH)))) & UP_CORNER_MASK) |
                      (corners & ~UP_CORNER_MASK);

            edges = (((masked_edges >> EDGE_WIDTH) | (masked_edges << (3 * EDGE_WIDTH))) & UP_EDGE_MASK) |
                    (edges & ~UP_EDGE_MASK);
            break;
        case COUNTER_CLOCKWISE:
            corners = (((masked_corners << CORNER_WIDTH) | (masked_corners >> (3 * CORNER_WIDTH))) & UP_CORNER_MASK) |
                      (corners & ~UP_CORNER_MASK);

            edges = (((masked_edges << EDGE_WIDTH) | (masked_edges >> (3 * EDGE_WIDTH))) & UP_EDGE_MASK) |
                    (edges & ~UP_EDGE_MASK);
            break;
        case DOUBLE:
            corners =
                (((masked_corners << (2 * CORNER_WIDTH)) | (masked_corners >> (2 * CORNER_WIDTH))) & UP_CORNER_MASK) |
                (corners & ~UP_CORNER_MASK);

            edges = (((masked_edges >> (2 * EDGE_WIDTH)) | (masked_edges << (2 * EDGE_WIDTH))) & UP_EDGE_MASK) |
                    (edges & ~UP_EDGE_MASK);
            break;
        }
        break;
    case RIGHT:
        masked_corners = corners & RIGHT_CORNER_MASK;
        masked_edges = edges & RIGHT_EDGE_MASK;
        switch (d)
        {
        case CLOCKWISE:
            corners = ((((masked_corners & URB_CORNER_MASK) >> (4 * CORNER_WIDTH)) |
                        (((masked_corners & URF_CORNER_MASK) << CORNER_WIDTH) |
                         ((masked_corners & DRB_CORNER_MASK) >> CORNER_WIDTH) |
                         ((masked_corners & DRF_CORNER_MASK) << (4 * CORNER_WIDTH)))) &
                       RIGHT_CORNER_MASK) |
                      (corners & ~RIGHT_CORNER_MASK);

            adjust_orientations(RIGHT_CORNER_ORIENTATION_ADJUSTMENT);

            edges = (((masked_edges & UR_EDGE_MASK) >> (5 * EDGE_WIDTH)) |
                     (((masked_edges & FR_EDGE_MASK) << (3 * EDGE_WIDTH)) |
                      ((masked_edges & BR_EDGE_MASK) >> (3 * EDGE_WIDTH)) |
                      ((masked_edges & DR_EDGE_MASK) << (5 * EDGE_WIDTH)))) |
                    (edges & ~RIGHT_EDGE_MASK);
            break;
        case COUNTER_CLOCKWISE:
            corners = ((((masked_corners & URB_CORNER_MASK) >> CORNER_WIDTH) |
                        (((masked_corners & URF_CORNER_MASK) >> (4 * CORNER_WIDTH)) |
                         ((masked_corners & DRB_CORNER_MASK) << (4 * CORNER_WIDTH)) |
                         ((masked_corners & DRF_CORNER_MASK) << CORNER_WIDTH))) &
                       RIGHT_CORNER_MASK) |
                      (corners & ~RIGHT_CORNER_MASK);

            adjust_orientations(RIGHT_CORNER_ORIENTATION_ADJUSTMENT);

            edges = (((masked_edges & UR_EDGE_MASK) >> (3 * EDGE_WIDTH)) |
                     (((masked_edges & FR_EDGE_MASK) >> (5 * EDGE_WIDTH)) |
                      ((masked_edges & BR_EDGE_MASK) << (5 * EDGE_WIDTH)) |
                      ((masked_edges & DR_EDGE_MASK) << (3 * EDGE_WIDTH)))) |
                    (edges & ~RIGHT_EDGE_MASK);
            break;
        case DOUBLE:
            corners = (((masked_corners & URB_CORNER_MASK) >> (5 * CORNER_WIDTH)) |
                       (((masked_corners & URF_CORNER_MASK) >> (3 * CORNER_WIDTH)) |
                        ((masked_corners & DRB_CORNER_MASK) << (3 * CORNER_WIDTH)) |
                        ((masked_corners & DRF_CORNER_MASK) << (5 * CORNER_WIDTH)))) |
                      (corners & ~RIGHT_CORNER_MASK);

            edges = (((masked_edges & UR_EDGE_MASK) >> (8 * EDGE_WIDTH)) |
                     (((masked_edges & FR_EDGE_MASK) >> (2 * EDGE_WIDTH)) |
                      ((masked_edges & BR_EDGE_MASK) << (2 * EDGE_WIDTH)) |
                      ((masked_edges & DR_EDGE_MASK) << (8 * EDGE_WIDTH)))) |
                    (edges & ~RIGHT_EDGE_MASK);
            break;
        }
        break;
    case LEFT:
        masked_corners = corners & LEFT_CORNER_MASK;
        masked_edges = edges & LEFT_EDGE_MASK;
        switch (d)
        {
        case CLOCKWISE:
            corners = ((((masked_corners & ULB_CORNER_MASK) >> (3 * CORNER_WIDTH)) |
                        (((masked_corners & ULF_CORNER_MASK) >> (4 * CORNER_WIDTH)) |
                         ((masked_corners & DLB_CORNER_MASK) << (4 * CORNER_WIDTH)) |
                         ((masked_corners & DLF_CORNER_MASK) << (3 * CORNER_WIDTH)))) &
                       LEFT_CORNER_MASK) |
                      (corners & ~LEFT_CORNER_MASK);

            adjust_orientations(LEFT_CORNER_ORIENTATION_ADJUSTMENT);

            edges = (((masked_edges & UL_EDGE_MASK) >> (2 * EDGE_WIDTH)) |
                     (((masked_edges & FL_EDGE_MASK) >> (6 * EDGE_WIDTH)) |
                      ((masked_edges & BL_EDGE_MASK) << (4 * EDGE_WIDTH)) |
                      ((masked_edges & DL_EDGE_MASK) << (4 * EDGE_WIDTH)))) |
                    (edges & ~LEFT_EDGE_MASK);
            break;
        case COUNTER_CLOCKWISE:
            corners = ((((masked_corners & ULB_CORNER_MASK) >> (4 * CORNER_WIDTH)) |
                        (((masked_corners & ULF_CORNER_MASK) << (3 * CORNER_WIDTH)) |
                         ((masked_corners & DLB_CORNER_MASK) >> (3 * CORNER_WIDTH)) |
                         ((masked_corners & DLF_CORNER_MASK) << (4 * CORNER_WIDTH)))) &
                       LEFT_CORNER_MASK) |
                      (corners & ~LEFT_CORNER_MASK);

            adjust_orientations(LEFT_CORNER_ORIENTATION_ADJUSTMENT);

            edges = (((masked_edges & UL_EDGE_MASK) >> (4 * EDGE_WIDTH)) |
                     (((masked_edges & FL_EDGE_MASK) << (2 * EDGE_WIDTH)) |
                      ((masked_edges & BL_EDGE_MASK) >> (4 * EDGE_WIDTH)) |
                      ((masked_edges & DL_EDGE_MASK) << (6 * EDGE_WIDTH)))) |
                    (edges & ~LEFT_EDGE_MASK);
            break;
        case DOUBLE:
            corners = (((masked_corners & ULB_CORNER_MASK) >> (7 * CORNER_WIDTH)) |
                       (((masked_corners & ULF_CORNER_MASK) >> CORNER_WIDTH) |
                        ((masked_corners & DLB_CORNER_MASK) << CORNER_WIDTH) |
                        ((masked_corners & DLF_CORNER_MASK) << (7 * CORNER_WIDTH)))) |
                      (corners & ~LEFT_CORNER_MASK);

            edges = (((masked_edges & UL_EDGE_MASK) >> (8 * EDGE_WIDTH)) |
                     (((masked_edges & FL_EDGE_MASK) >> (2 * EDGE_WIDTH)) |
                      ((masked_edges & BL_EDGE_MASK) << (2 * EDGE_WIDTH)) |
                      ((masked_edges & DL_EDGE_MASK) << (8 * EDGE_WIDTH)))) |
                    (edges & ~LEFT_EDGE_MASK);
            break;
        }
        break;
    case FRONT:
        masked_corners = corners & FRONT_CORNER_MASK;
        masked_edges = edges & FRONT_EDGE_MASK;
        switch (d)
        {
        case CLOCKWISE:
            corners = ((((masked_corners & URF_CORNER_MASK) >> (4 * CORNER_WIDTH)) |
                        (((masked_corners & ULF_CORNER_MASK) << CORNER_WIDTH) |
                         ((masked_corners & DRF_CORNER_MASK) >> CORNER_WIDTH) |
                         ((masked_corners & DLF_CORNER_MASK) << (4 * CORNER_WIDTH)))) &
                       FRONT_CORNER_MASK) |
                      (corners & ~FRONT_CORNER_MASK);

            adjust_orientations(FRONT_CORNER_ORIENTATION_ADJUSTMENT);

            edges = (((masked_edges & UF_EDGE_MASK) >> (2 * EDGE_WIDTH)) |
                     (((masked_edges & FR_EDGE_MASK) >> (6 * EDGE_WIDTH)) |
                      ((masked_edges & FL_EDGE_MASK) << (3 * EDGE_WIDTH)) |
                      ((masked_edges & DF_EDGE_MASK) << (5 * EDGE_WIDTH)))) |
                    (edges & ~FRONT_EDGE_MASK);

            edges ^= FRONT_EDGE_ORIENTATION_ADJUSTMENT;
            break;
        case COUNTER_CLOCKWISE:
            corners = ((((masked_corners & URF_CORNER_MASK) >> CORNER_WIDTH) |
                        (((masked_corners & ULF_CORNER_MASK) >> (4 * CORNER_WIDTH)) |
                         ((masked_corners & DRF_CORNER_MASK) << (4 * CORNER_WIDTH)) |
                         ((masked_corners & DLF_CORNER_MASK) << CORNER_WIDTH))) &
                       FRONT_CORNER_MASK) |
                      (corners & ~FRONT_CORNER_MASK);

            adjust_orientations(FRONT_CORNER_ORIENTATION_ADJUSTMENT);

            edges = (((masked_edges & UF_EDGE_MASK) >> (3 * EDGE_WIDTH)) |
                     (((masked_edges & FR_EDGE_MASK) << (2 * EDGE_WIDTH)) |
                      ((masked_edges & FL_EDGE_MASK) >> (5 * EDGE_WIDTH)) |
                      ((masked_edges & DF_EDGE_MASK) << (6 * EDGE_WIDTH)))) |
                    (edges & ~FRONT_EDGE_MASK);

            edges ^= FRONT_EDGE_ORIENTATION_ADJUSTMENT;
            break;
        case DOUBLE:
            corners = (((masked_corners & URF_CORNER_MASK) >> (5 * CORNER_WIDTH)) |
                       (((masked_corners & ULF_CORNER_MASK) >> (3 * CORNER_WIDTH)) |
                        ((masked_corners & DRF_CORNER_MASK) << (3 * CORNER_WIDTH)) |
                        ((masked_corners & DLF_CORNER_MASK) << (5 * CORNER_WIDTH)))) |
                      (corners & ~FRONT_CORNER_MASK);

            edges = (((masked_edges & UF_EDGE_MASK) >> (8 * EDGE_WIDTH)) |
                     (((masked_edges & FR_EDGE_MASK) >> EDGE_WIDTH) | ((masked_edges & FL_EDGE_MASK) << EDGE_WIDTH) |
                      ((masked_edges & DF_EDGE_MASK) << (8 * EDGE_WIDTH)))) |
                    (edges & ~FRONT_EDGE_MASK);
            break;
        }
        break;
    case BACK:
        masked_corners = corners & BACK_CORNER_MASK;
        masked_edges = edges & BACK_EDGE_MASK;
        switch (d)
        {
        case CLOCKWISE:
            corners = ((((masked_corners & ULB_CORNER_MASK) >> (4 * CORNER_WIDTH)) |
                        (((masked_corners & URB_CORNER_MASK) << CORNER_WIDTH) |
                         ((masked_corners & DLB_CORNER_MASK) >> CORNER_WIDTH) |
                         ((masked_corners & DRB_CORNER_MASK) << (4 * CORNER_WIDTH)))) &
                       BACK_CORNER_MASK) |
                      (corners & ~BACK_CORNER_MASK);

            adjust_orientations(BACK_CORNER_ORIENTATION_ADJUSTMENT);

            edges =
                (((masked_edges & UB_EDGE_MASK) >> (7 * EDGE_WIDTH)) |
                 (((masked_edges & BR_EDGE_MASK) << (6 * EDGE_WIDTH)) | ((masked_edges & BL_EDGE_MASK) >> EDGE_WIDTH) |
                  ((masked_edges & DB_EDGE_MASK) << (2 * EDGE_WIDTH)))) |
                (edges & ~BACK_EDGE_MASK);

            edges ^= BACK_EDGE_ORIENTATION_ADJUSTMENT;
            break;
        case COUNTER_CLOCKWISE:
            corners = ((((masked_corners & ULB_CORNER_MASK) >> CORNER_WIDTH) |
                        (((masked_corners & URB_CORNER_MASK) >> (4 * CORNER_WIDTH)) |
                         ((masked_corners & DLB_CORNER_MASK) << (4 * CORNER_WIDTH)) |
                         ((masked_corners & DRB_CORNER_MASK) << CORNER_WIDTH))) &
                       BACK_CORNER_MASK) |
                      (corners & ~BACK_CORNER_MASK);

            adjust_orientations(BACK_CORNER_ORIENTATION_ADJUSTMENT);

            edges = (((masked_edges & UB_EDGE_MASK) >> (6 * EDGE_WIDTH)) |
                     (((masked_edges & BR_EDGE_MASK) >> (2 * EDGE_WIDTH)) |
                      ((masked_edges & BL_EDGE_MASK) << (7 * EDGE_WIDTH)) |
                      ((masked_edges & DB_EDGE_MASK) << EDGE_WIDTH))) |
                    (edges & ~BACK_EDGE_MASK);

            edges ^= BACK_EDGE_ORIENTATION_ADJUSTMENT;
            break;
        case DOUBLE:
            corners = (((masked_corners & ULB_CORNER_MASK) >> (5 * CORNER_WIDTH)) |
                       (((masked_corners & URB_CORNER_MASK) >> (3 * CORNER_WIDTH)) |
                        ((masked_corners & DLB_CORNER_MASK) << (3 * CORNER_WIDTH)) |
                        ((masked_corners & DRB_CORNER_MASK) << (5 * CORNER_WIDTH)))) |
                      (corners & ~BACK_CORNER_MASK);

            edges = (((masked_edges & UB_EDGE_MASK) >> (8 * EDGE_WIDTH)) |
                     (((masked_edges & BR_EDGE_MASK) >> EDGE_WIDTH) | ((masked_edges & BL_EDGE_MASK) << EDGE_WIDTH) |
                      ((masked_edges & DB_EDGE_MASK) << (8 * EDGE_WIDTH)))) |
                    (edges & ~BACK_EDGE_MASK);
            break;
        }
        break;
    case DOWN:
        masked_corners = corners & DOWN_CORNER_MASK;
        masked_edges = edges & DOWN_EDGE_MASK;
        switch (d)
        {
        case CLOCKWISE:
            corners = (((masked_corners << CORNER_WIDTH) & DOWN_CORNER_MASK) | (masked_corners >> (3 * CORNER_WIDTH))) |
                      (corners & ~DOWN_CORNER_MASK);

            edges = (((masked_edges << EDGE_WIDTH) | (masked_edges >> (3 * EDGE_WIDTH))) & DOWN_EDGE_MASK) |
                    (edges & ~DOWN_EDGE_MASK);
            break;
        case COUNTER_CLOCKWISE:
            corners = ((masked_corners >> CORNER_WIDTH) | ((masked_corners << (3 * CORNER_WIDTH)) & DOWN_CORNER_MASK)) |
                      (corners & ~DOWN_CORNER_MASK);

            edges = (((masked_edges >> EDGE_WIDTH) | (masked_edges << (3 * EDGE_WIDTH))) & DOWN_EDGE_MASK) |
                    (edges & ~DOWN_EDGE_MASK);
            break;
        case DOUBLE:
            corners =
                (((masked_corners << (2 * CORNER_WIDTH)) | (masked_corners >> (2 * CORNER_WIDTH))) & DOWN_CORNER_MASK) |
                (corners & ~DOWN_CORNER_MASK);

            edges = (((masked_edges << (2 * EDGE_WIDTH)) | (masked_edges >> (2 * EDGE_WIDTH))) & DOWN_EDGE_MASK) |
                    (edges & ~DOWN_EDGE_MASK);
            break;
        }
        break;
    }

    if (!keep_blank_edge_orientations)
    {
        for (int i = 0; i < NUM_EDGES; i++)
        {
            if (((edges >> (EDGE_WIDTH * i)) & EDGE_MASK) == (NN | EDGE_ADJUSTMENT))
            {
                edges ^= (EDGE_ADJUSTMENT << (EDGE_WIDTH * i));
            }
        }
    }
}

void rubicks_3x3::print_cube()
{
    update_corners();
    update_edges();

    std::cout << "   " << corners_list[0][0] << edges_list[0][0] << corners_list[1][0] << "\n";
    std::cout << "   " << edges_list[3][0] << "W" << edges_list[1][0] << "\n";
    std::cout << "   " << corners_list[3][0] << edges_list[2][0] << corners_list[2][0] << "\n";
    std::cout << corners_list[0][1] << edges_list[3][1] << corners_list[3][2] << corners_list[3][1] << edges_list[2][1]
              << corners_list[2][2] << corners_list[2][1] << edges_list[1][1] << corners_list[1][2]
              << corners_list[1][1] << edges_list[0][1] << corners_list[0][2] << "\n";
    std::cout << edges_list[7][1] << "R" << edges_list[5][1] << edges_list[5][0] << "B" << edges_list[4][0]
              << edges_list[4][1] << "O" << edges_list[6][1] << edges_list[6][0] << "G" << edges_list[7][0] << "\n";
    std::cout << corners_list[4][2] << edges_list[11][1] << corners_list[7][1] << corners_list[7][2]
              << edges_list[10][1] << corners_list[6][1] << corners_list[6][2] << edges_list[9][1] << corners_list[5][1]
              << corners_list[5][2] << edges_list[8][1] << corners_list[4][1] << "\n";
    std::cout << "   " << corners_list[7][0] << edges_list[10][0] << corners_list[6][0] << "\n";
    std::cout << "   " << edges_list[11][0] << "Y" << edges_list[9][0] << "\n";
    std::cout << "   " << corners_list[4][0] << edges_list[8][0] << corners_list[5][0] << "\n";
}

void rubicks_3x3::input_move(moves move)
{
    switch (move)
    {
    case U:
        rotate(UP, CLOCKWISE);
        break;
    case U_PRIME:
        rotate(UP, COUNTER_CLOCKWISE);
        break;
    case U2:
        rotate(UP, DOUBLE);
        break;
    case R:
        rotate(RIGHT, CLOCKWISE);
        break;
    case R_PRIME:
        rotate(RIGHT, COUNTER_CLOCKWISE);
        break;
    case R2:
        rotate(RIGHT, DOUBLE);
        break;
    case L:
        rotate(LEFT, CLOCKWISE);
        break;
    case L_PRIME:
        rotate(LEFT, COUNTER_CLOCKWISE);
        break;
    case L2:
        rotate(LEFT, DOUBLE);
        break;
    case F:
        rotate(FRONT, CLOCKWISE);
        break;
    case F_PRIME:
        rotate(FRONT, COUNTER_CLOCKWISE);
        break;
    case F2:
        rotate(FRONT, DOUBLE);
        break;
    case B:
        rotate(BACK, CLOCKWISE);
        break;
    case B_PRIME:
        rotate(BACK, COUNTER_CLOCKWISE);
        break;
    case B2:
        rotate(BACK, DOUBLE);
        break;
    case D:
        rotate(DOWN, CLOCKWISE);
        break;
    case D_PRIME:
        rotate(DOWN, COUNTER_CLOCKWISE);
        break;
    case D2:
        rotate(DOWN, DOUBLE);
        break;
    case INVALID_MOVE:
        std::cerr << "Invalid Move\n";
        break;
    }
}

void rubicks_3x3::input_scramble(std::string scramble)
{
    std::stringstream ss(scramble);
    std::string move;
    while (ss >> move)
    {
        moves current_move = get_move(move);
        switch (current_move)
        {
        case U:
            rotate(UP, CLOCKWISE);
            break;
        case U_PRIME:
            rotate(UP, COUNTER_CLOCKWISE);
            break;
        case U2:
            rotate(UP, DOUBLE);
            break;
        case R:
            rotate(RIGHT, CLOCKWISE);
            break;
        case R_PRIME:
            rotate(RIGHT, COUNTER_CLOCKWISE);
            break;
        case R2:
            rotate(RIGHT, DOUBLE);
            break;
        case L:
            rotate(LEFT, CLOCKWISE);
            break;
        case L_PRIME:
            rotate(LEFT, COUNTER_CLOCKWISE);
            break;
        case L2:
            rotate(LEFT, DOUBLE);
            break;
        case F:
            rotate(FRONT, CLOCKWISE);
            break;
        case F_PRIME:
            rotate(FRONT, COUNTER_CLOCKWISE);
            break;
        case F2:
            rotate(FRONT, DOUBLE);
            break;
        case B:
            rotate(BACK, CLOCKWISE);
            break;
        case B_PRIME:
            rotate(BACK, COUNTER_CLOCKWISE);
            break;
        case B2:
            rotate(BACK, DOUBLE);
            break;
        case D:
            rotate(DOWN, CLOCKWISE);
            break;
        case D_PRIME:
            rotate(DOWN, COUNTER_CLOCKWISE);
            break;
        case D2:
            rotate(DOWN, DOUBLE);
            break;
        case INVALID_MOVE:
            std::cerr << "Invalid Move\n";
            break;
        }
    }
}

// int main()
// {
//     rubicks_3x3 cube(CASE_MIDDLE_SLICE_3X3, false);
//     cube.input_scramble("F");

//     std::cout << "Expected: " << std::bitset<60>(cube.edges) << "\n";

//     rubicks_3x3 cube2(CASE_MIDDLE_SLICE_3X3, true);
//     cube2.input_scramble("F");

//     std::cout << "Expected: " << std::bitset<60>(cube2.edges) << "\n";

//     // cube.print_cube();
//     // test_rotations();

//     // std::string scramble;
//     // while (true)
//     // {
//     //     std::cout << "Enter your scramble: ";
//     //     std::getline(std::cin, scramble);
//     //     if (scramble == "q")
//     //         break;
//     //     cube.input_scramble(scramble);
//     //     cube.print_cube();
//     // }

//     return 0;
// }
