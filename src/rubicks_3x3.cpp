#include "../include/rubicks_3x3.h"
#include "../include/rubicks_2x2.h"
#include <bitset>
#include <cstring>
#include <iostream>
#include <sstream>

std::string get_edge(edge e, orientation o)
{
    int x = (int)o;
    std::string edge_colours[12] = {"WG", "WO", "WB", "WR", "BO", "BR", "GO", "GR", "YG", "YO", "YB", "YR"};

    std::string current_edge = edge_colours[uint8_t(e)];
    return current_edge.substr(x, 1) + current_edge.substr((x + 1) % 2, 1);
};

bool rubicks_3x3::is_solved()
{
    return corners == SOLVED_CORNERS and edges == SOLVED_EDGES;
}

void rubicks_3x3::update_corners()
{
    uint64_t remaining_corners = corners;
    const uint64_t CORNER_MASK = 0b111;
    const uint64_t CORNER_ORIENTATION_MASK = 0b11000;

    for (int i = 7; i >= 0; i--)
    {
        corners_list[i] =
            get_corner(static_cast<corner>((uint8_t)(CORNER_MASK & remaining_corners)),
                       static_cast<orientation>((uint8_t)((CORNER_ORIENTATION_MASK & remaining_corners) >> 3)));
        remaining_corners >>= 6;
    }
}

void rubicks_3x3::update_edges()
{
    uint64_t remaining_edges = edges;
    const uint64_t EDGE_MASK = 0b1111;
    const uint64_t EDGE_ORIENTATION_MASK = 0b10000;

    for (int i = 11; i >= 0; i--)
    {
        edges_list[i] = get_edge(static_cast<edge>((uint8_t)(EDGE_MASK & remaining_edges)),
                                 static_cast<orientation>((uint8_t)((EDGE_ORIENTATION_MASK & remaining_edges) >> 4)));
        remaining_edges >>= 5;
    }
}

void rubicks_3x3::adjust_orientations(uint64_t &corners, uint64_t adjustment)
{
    const uint64_t ORIENTATION_MASK = 0b11000;

    for (int i = 0; i < 8; i++)
    {
        if (adjustment & ORIENTATION_MASK)
        {
            int initial_orientation = ((corners >> (6 * i)) & ORIENTATION_MASK) >> 3;

            int new_orientation = (adjustment & ORIENTATION_MASK) >> 3;

            new_orientation = (initial_orientation + new_orientation) % 3;

            corners = (corners & ~(ORIENTATION_MASK << (6 * i))) | ((uint64_t)new_orientation << (3 + 6 * i));
        }
        adjustment >>= 6;
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
            corners =
                (((masked_corners >> 6) | ((masked_corners << 18))) & UP_CORNER_MASK) | (corners & ~UP_CORNER_MASK);
            edges = (((masked_edges >> 5) | (masked_edges << 15)) & UP_EDGE_MASK) | (edges & ~UP_EDGE_MASK);
            break;
        case COUNTER_CLOCKWISE:
            corners = (((masked_corners << 6) | (masked_corners >> 18)) & UP_CORNER_MASK) | (corners & ~UP_CORNER_MASK);
            edges = (((masked_edges << 5) | (masked_edges >> 15)) & UP_EDGE_MASK) | (edges & ~UP_EDGE_MASK);
            break;
        case DOUBLE:
            corners =
                (((masked_corners << 12) | (masked_corners >> 12)) & UP_CORNER_MASK) | (corners & ~UP_CORNER_MASK);
            edges = (((masked_edges >> 10) | (masked_edges << 10)) & UP_EDGE_MASK) | (edges & ~UP_EDGE_MASK);
            break;
        }
        break;
    case RIGHT:
        masked_corners = corners & RIGHT_CORNER_MASK;
        masked_edges = edges & RIGHT_EDGE_MASK;
        switch (d)
        {
        case CLOCKWISE:
            corners = ((((masked_corners & URB_CORNER_MASK) >> 24) |
                        (((masked_corners & URF_CORNER_MASK) << 6) | ((masked_corners & DRB_CORNER_MASK) >> 6) |
                         ((masked_corners & DRF_CORNER_MASK) << 24))) &
                       RIGHT_CORNER_MASK) |
                      (corners & ~RIGHT_CORNER_MASK);
            adjust_orientations(corners, RIGHT_CORNER_ORIENTATION_ADJUSTMENT);
            edges = (((masked_edges & UR_EDGE_MASK) >> 25) |
                     (((masked_edges & FR_EDGE_MASK) << 15) | ((masked_edges & BR_EDGE_MASK) >> 15) |
                      ((masked_edges & DR_EDGE_MASK) << 25))) |
                    (edges & ~RIGHT_EDGE_MASK);
            break;
        case COUNTER_CLOCKWISE:
            corners = ((((masked_corners & URB_CORNER_MASK) >> 6) |
                        (((masked_corners & URF_CORNER_MASK) >> 24) | ((masked_corners & DRB_CORNER_MASK) << 24) |
                         ((masked_corners & DRF_CORNER_MASK) << 6))) &
                       RIGHT_CORNER_MASK) |
                      (corners & ~RIGHT_CORNER_MASK);
            adjust_orientations(corners, RIGHT_CORNER_ORIENTATION_ADJUSTMENT);
            edges = (((masked_edges & UR_EDGE_MASK) >> 15) |
                     (((masked_edges & FR_EDGE_MASK) >> 25) | ((masked_edges & BR_EDGE_MASK) << 25) |
                      ((masked_edges & DR_EDGE_MASK) << 15))) |
                    (edges & ~RIGHT_EDGE_MASK);
            break;
        case DOUBLE:
            corners = (((masked_corners & URB_CORNER_MASK) >> 30) |
                       (((masked_corners & URF_CORNER_MASK) >> 18) | ((masked_corners & DRB_CORNER_MASK) << 18) |
                        ((masked_corners & DRF_CORNER_MASK) << 30))) |
                      (corners & ~RIGHT_CORNER_MASK);
            edges = (((masked_edges & UR_EDGE_MASK) >> 40) |
                     (((masked_edges & FR_EDGE_MASK) >> 10) | ((masked_edges & BR_EDGE_MASK) << 10) |
                      ((masked_edges & DR_EDGE_MASK) << 40))) |
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
            corners = ((((masked_corners & ULB_CORNER_MASK) >> 18) |
                        (((masked_corners & ULF_CORNER_MASK) >> 24) | ((masked_corners & DLB_CORNER_MASK) << 24) |
                         ((masked_corners & DLF_CORNER_MASK) << 18))) &
                       LEFT_CORNER_MASK) |
                      (corners & ~LEFT_CORNER_MASK);
            adjust_orientations(corners, LEFT_CORNER_ORIENTATION_ADJUSTMENT);
            edges = (((masked_edges & UL_EDGE_MASK) >> 10) |
                     (((masked_edges & FL_EDGE_MASK) >> 30) | ((masked_edges & BL_EDGE_MASK) << 20) |
                      ((masked_edges & DL_EDGE_MASK) << 20))) |
                    (edges & ~LEFT_EDGE_MASK);
            break;
        case COUNTER_CLOCKWISE:
            corners = ((((masked_corners & ULB_CORNER_MASK) >> 24) |
                        (((masked_corners & ULF_CORNER_MASK) << 18) | ((masked_corners & DLB_CORNER_MASK) >> 18) |
                         ((masked_corners & DLF_CORNER_MASK) << 24))) &
                       LEFT_CORNER_MASK) |
                      (corners & ~LEFT_CORNER_MASK);
            adjust_orientations(corners, LEFT_CORNER_ORIENTATION_ADJUSTMENT);
            edges = (((masked_edges & UL_EDGE_MASK) >> 20) |
                     (((masked_edges & FL_EDGE_MASK) << 10) | ((masked_edges & BL_EDGE_MASK) >> 20) |
                      ((masked_edges & DL_EDGE_MASK) << 30))) |
                    (edges & ~LEFT_EDGE_MASK);
            break;
        case DOUBLE:
            corners = (((masked_corners & ULB_CORNER_MASK) >> 42) |
                       (((masked_corners & ULF_CORNER_MASK) >> 6) | ((masked_corners & DLB_CORNER_MASK) << 6) |
                        ((masked_corners & DLF_CORNER_MASK) << 42))) |
                      (corners & ~LEFT_CORNER_MASK);
            edges = (((masked_edges & UL_EDGE_MASK) >> 40) |
                     (((masked_edges & FL_EDGE_MASK) >> 10) | ((masked_edges & BL_EDGE_MASK) << 10) |
                      ((masked_edges & DL_EDGE_MASK) << 40))) |
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
            corners = ((((masked_corners & URF_CORNER_MASK) >> 24) |
                        (((masked_corners & ULF_CORNER_MASK) << 6) | ((masked_corners & DRF_CORNER_MASK) >> 6) |
                         ((masked_corners & DLF_CORNER_MASK) << 24))) &
                       FRONT_CORNER_MASK) |
                      (corners & ~FRONT_CORNER_MASK);
            adjust_orientations(corners, FRONT_CORNER_ORIENTATION_ADJUSTMENT);
            edges = (((masked_edges & UF_EDGE_MASK) >> 10) |
                     (((masked_edges & FR_EDGE_MASK) >> 30) | ((masked_edges & FL_EDGE_MASK) << 15) |
                      ((masked_edges & DF_EDGE_MASK) << 25))) |
                    (edges & ~FRONT_EDGE_MASK);
            edges ^= FRONT_EDGE_ORIENTATION_ADJUSTMENT;
            break;
        case COUNTER_CLOCKWISE:
            corners = ((((masked_corners & URF_CORNER_MASK) >> 6) |
                        (((masked_corners & ULF_CORNER_MASK) >> 24) | ((masked_corners & DRF_CORNER_MASK) << 24) |
                         ((masked_corners & DLF_CORNER_MASK) << 6))) &
                       FRONT_CORNER_MASK) |
                      (corners & ~FRONT_CORNER_MASK);
            adjust_orientations(corners, FRONT_CORNER_ORIENTATION_ADJUSTMENT);
            edges = (((masked_edges & UF_EDGE_MASK) >> 15) |
                     (((masked_edges & FR_EDGE_MASK) << 10) | ((masked_edges & FL_EDGE_MASK) >> 25) |
                      ((masked_edges & DF_EDGE_MASK) << 30))) |
                    (edges & ~FRONT_EDGE_MASK);
            edges ^= FRONT_EDGE_ORIENTATION_ADJUSTMENT;
            break;
        case DOUBLE:
            corners = (((masked_corners & URF_CORNER_MASK) >> 30) |
                       (((masked_corners & ULF_CORNER_MASK) >> 18) | ((masked_corners & DRF_CORNER_MASK) << 18) |
                        ((masked_corners & DLF_CORNER_MASK) << 30))) |
                      (corners & ~FRONT_CORNER_MASK);
            edges = (((masked_edges & UF_EDGE_MASK) >> 40) |
                     (((masked_edges & FR_EDGE_MASK) >> 5) | ((masked_edges & FL_EDGE_MASK) << 5) |
                      ((masked_edges & DF_EDGE_MASK) << 40))) |
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
            corners = ((((masked_corners & ULB_CORNER_MASK) >> 24) |
                        (((masked_corners & URB_CORNER_MASK) << 6) | ((masked_corners & DLB_CORNER_MASK) >> 6) |
                         ((masked_corners & DRB_CORNER_MASK) << 24))) &
                       BACK_CORNER_MASK) |
                      (corners & ~BACK_CORNER_MASK);
            adjust_orientations(corners, BACK_CORNER_ORIENTATION_ADJUSTMENT);
            edges = (((masked_edges & UB_EDGE_MASK) >> 35) |
                     (((masked_edges & BR_EDGE_MASK) << 30) | ((masked_edges & BL_EDGE_MASK) >> 5) |
                      ((masked_edges & DB_EDGE_MASK) << 10))) |
                    (edges & ~BACK_EDGE_MASK);
            edges ^= BACK_EDGE_ORIENTATION_ADJUSTMENT;
            break;
        case COUNTER_CLOCKWISE:
            corners = ((((masked_corners & ULB_CORNER_MASK) >> 6) |
                        (((masked_corners & URB_CORNER_MASK) >> 24) | ((masked_corners & DLB_CORNER_MASK) << 24) |
                         ((masked_corners & DRB_CORNER_MASK) << 6))) &
                       BACK_CORNER_MASK) |
                      (corners & ~BACK_CORNER_MASK);
            adjust_orientations(corners, BACK_CORNER_ORIENTATION_ADJUSTMENT);
            edges = (((masked_edges & UB_EDGE_MASK) >> 30) |
                     (((masked_edges & BR_EDGE_MASK) >> 10) | ((masked_edges & BL_EDGE_MASK) << 35) |
                      ((masked_edges & DB_EDGE_MASK) << 5))) |
                    (edges & ~BACK_EDGE_MASK);
            edges ^= BACK_EDGE_ORIENTATION_ADJUSTMENT;
            break;
        case DOUBLE:
            corners = (((masked_corners & ULB_CORNER_MASK) >> 30) |
                       (((masked_corners & URB_CORNER_MASK) >> 18) | ((masked_corners & DLB_CORNER_MASK) << 18) |
                        ((masked_corners & DRB_CORNER_MASK) << 30))) |
                      (corners & ~BACK_CORNER_MASK);
            edges = (((masked_edges & UB_EDGE_MASK) >> 40) |
                     (((masked_edges & BR_EDGE_MASK) >> 5) | ((masked_edges & BL_EDGE_MASK) << 5) |
                      ((masked_edges & DB_EDGE_MASK) << 40))) |
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
            corners =
                (((masked_corners << 6) & DOWN_CORNER_MASK) | (masked_corners >> 18)) | (corners & ~DOWN_CORNER_MASK);
            edges = (((masked_edges << 5) | (masked_edges >> 15)) & DOWN_EDGE_MASK) | (edges & ~DOWN_EDGE_MASK);
            break;
        case COUNTER_CLOCKWISE:
            corners =
                ((masked_corners >> 6) | ((masked_corners << 18) & DOWN_CORNER_MASK)) | (corners & ~DOWN_CORNER_MASK);
            edges = (((masked_edges >> 5) | (masked_edges << 15)) & DOWN_EDGE_MASK) | (edges & ~DOWN_EDGE_MASK);
            break;
        case DOUBLE:
            corners =
                (((masked_corners << 12) | (masked_corners >> 12)) & DOWN_CORNER_MASK) | (corners & ~DOWN_CORNER_MASK);
            edges = (((masked_edges << 10) | (masked_edges >> 10)) & DOWN_EDGE_MASK) | (edges & ~DOWN_EDGE_MASK);
            break;
        }
        break;
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

int main()
{
    rubicks_3x3 cube;
    std::string scramble;
    while (true)
    {
        std::cout << "Enter your scramble: ";
        std::getline(std::cin, scramble);
        if (scramble == "q")
            break;
        cube.input_scramble(scramble);
        cube.print_cube();
    }

    return 0;
}
