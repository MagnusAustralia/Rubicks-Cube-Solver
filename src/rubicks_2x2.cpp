#include "../include/rubicks_2x2.h"
#include <bitset>
#include <cstring>
#include <iostream>
#include <sstream>

bool test_rotations()
{
    rubicks_2x2 cube;
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

            if (!cube.is_solved())
            {
                is_valid = false;
                std::cout << (int)f << "\n";
                std::cout << "Expected: " << std::bitset<64>(SOLVED_CORNERS) << "\n";
                std::cout << "Actual:   " << std::bitset<64>(cube.corners) << "\n";
            }
        }
    }

    if (is_valid)
    {
        std::cout << "All tests passed!!\n";
    }

    return is_valid;
}

std::string get_corner(corner c, orientation o)
{
    int x = (int)o;
    std::string corner_colours[8] = {"WRG", "WGO", "WOB", "WBR", "YGR", "YOG", "YBO", "YRB"};

    std::string current_corner = corner_colours[uint8_t(c)];
    return current_corner.substr(x, 1) + current_corner.substr((x + 1) % 3, 1) + current_corner.substr((x + 2) % 3, 1);
};

std::string print_move(moves move)
{
    switch (move)
    {
    case U:
        return "U";
    case U_PRIME:
        return "U'";
    case U2:
        return "U2";
    case R:
        return "R";
    case R_PRIME:
        return "R'";
    case R2:
        return "R2";
    case L:
        return "L";
    case L_PRIME:
        return "L'";
    case L2:
        return "L2";
    case F:
        return "F";
    case F_PRIME:
        return "F'";
    case F2:
        return "F2";
    case B:
        return "B";
    case B_PRIME:
        return "B'";
    case B2:
        return "B2";
    case D:
        return "D";
    case D_PRIME:
        return "D'";
    case D2:
        return "D2";
    case INVALID_MOVE:
        return "None";
    }
}

moves get_move(std::string move)
{
    if (move.substr(0, 1) == "U")
    {
        if (move.size() == 2)
        {
            if (move.substr(1) == "2")
                return U2;
            else
                return U_PRIME;
        }
        else
            return U;
    }
    if (move.substr(0, 1) == "F")
    {
        if (move.size() == 2)
        {
            if (move.substr(1) == "2")
                return F2;
            else
                return F_PRIME;
        }
        else
            return F;
    }
    if (move.substr(0, 1) == "R")
    {
        if (move.size() == 2)
        {
            if (move.substr(1) == "2")
                return R2;
            else
                return R_PRIME;
        }
        else
            return R;
    }
    if (move.substr(0, 1) == "L")
    {
        if (move.size() == 2)
        {
            if (move.substr(1) == "2")
                return L2;
            else
                return L_PRIME;
        }
        else
            return L;
    }
    if (move.substr(0, 1) == "B")
    {
        if (move.size() == 2)
        {
            if (move.substr(1) == "2")
                return B2;
            else
                return B_PRIME;
        }
        else
            return B;
    }
    if (move.substr(0, 1) == "D")
    {
        if (move.size() == 2)
        {
            if (move.substr(1) == "2")
                return D2;
            else
                return D_PRIME;
        }
        else
            return D;
    }
    else
    {
        return INVALID_MOVE;
    }
}

bool rubicks_2x2::is_solved() const
{
    return corners == SOLVED_CORNERS;
}

void rubicks_2x2::update_corners()
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

void rubicks_2x2::adjust_orientations(uint64_t adjustment)
{
    for (int i = 0; i < NUM_CORNERS; i++)
    {
        if (adjustment & CORNER_ORIENTATION_MASK)
        {
            int initial_orientation =
                ((corners >> (CORNER_WIDTH * i)) & CORNER_ORIENTATION_MASK) >> CORNER_ORIENTATION_SHIFT;

            int new_orientation = (adjustment & CORNER_ORIENTATION_MASK) >> CORNER_ORIENTATION_SHIFT;

            new_orientation = (initial_orientation + new_orientation) % NUM_CORNER_ORIENTATIONS;

            corners = (corners & ~(CORNER_ORIENTATION_MASK << (CORNER_WIDTH * i))) |
                      ((uint64_t)new_orientation << (CORNER_ORIENTATION_SHIFT + CORNER_WIDTH * i));
        }
        adjustment >>= CORNER_WIDTH;
    }
}

void rubicks_2x2::rotate(face f, direction d)
{
    uint64_t masked_corners;

    switch (f)
    {
    case UP:
        masked_corners = corners & UP_CORNER_MASK;
        switch (d)
        {
        case CLOCKWISE:
            // std::cout << "Up Clockwise Turn" << "\n";
            corners = (((masked_corners >> CORNER_WIDTH) | ((masked_corners << (3 * CORNER_WIDTH)))) & UP_CORNER_MASK) |
                      (corners & ~UP_CORNER_MASK);
            break;
        case COUNTER_CLOCKWISE:
            // std::cout << "Up Counter-Clockwise Turn" << "\n";
            corners = (((masked_corners << CORNER_WIDTH) | (masked_corners >> (3 * CORNER_WIDTH))) & UP_CORNER_MASK) |
                      (corners & ~UP_CORNER_MASK);
            break;
        case DOUBLE:
            // std::cout << "Up Double Turn" << "\n";
            corners =
                (((masked_corners << (2 * CORNER_WIDTH)) | (masked_corners >> (2 * CORNER_WIDTH))) & UP_CORNER_MASK) |
                (corners & ~UP_CORNER_MASK);
            break;
        }
        break;
    case RIGHT:
        masked_corners = corners & RIGHT_CORNER_MASK;
        switch (d)
        {
        case CLOCKWISE:
            // std::cout << "Right Clockwise Turn" << "\n";
            corners = ((((masked_corners & URB_CORNER_MASK) >> (4 * CORNER_WIDTH)) |
                        (((masked_corners & URF_CORNER_MASK) << CORNER_WIDTH) |
                         ((masked_corners & DRB_CORNER_MASK) >> CORNER_WIDTH) |
                         ((masked_corners & DRF_CORNER_MASK) << (4 * CORNER_WIDTH)))) &
                       RIGHT_CORNER_MASK) |
                      (corners & ~RIGHT_CORNER_MASK);
            adjust_orientations(RIGHT_CORNER_ORIENTATION_ADJUSTMENT);
            break;
        case COUNTER_CLOCKWISE:
            // std::cout << "Right Counter-Clockwise Turn" << "\n";
            corners = ((((masked_corners & URB_CORNER_MASK) >> CORNER_WIDTH) |
                        (((masked_corners & URF_CORNER_MASK) >> (4 * CORNER_WIDTH)) |
                         ((masked_corners & DRB_CORNER_MASK) << (4 * CORNER_WIDTH)) |
                         ((masked_corners & DRF_CORNER_MASK) << CORNER_WIDTH))) &
                       RIGHT_CORNER_MASK) |
                      (corners & ~RIGHT_CORNER_MASK);
            adjust_orientations(RIGHT_CORNER_ORIENTATION_ADJUSTMENT);
            break;
        case DOUBLE:
            // std::cout << "Right Double Turn" << "\n";
            corners = (((masked_corners & URB_CORNER_MASK) >> (5 * CORNER_WIDTH)) |
                       (((masked_corners & URF_CORNER_MASK) >> (3 * CORNER_WIDTH)) |
                        ((masked_corners & DRB_CORNER_MASK) << (3 * CORNER_WIDTH)) |
                        ((masked_corners & DRF_CORNER_MASK) << (5 * CORNER_WIDTH)))) |
                      (corners & ~RIGHT_CORNER_MASK);
            break;
        }
        break;
    case LEFT:
        masked_corners = corners & LEFT_CORNER_MASK;
        switch (d)
        {
        case CLOCKWISE:
            // std::cout << "Left Clockwise Turn" << "\n";
            corners = ((((masked_corners & ULB_CORNER_MASK) >> (3 * CORNER_WIDTH)) |
                        (((masked_corners & ULF_CORNER_MASK) >> (4 * CORNER_WIDTH)) |
                         ((masked_corners & DLB_CORNER_MASK) << (4 * CORNER_WIDTH)) |
                         ((masked_corners & DLF_CORNER_MASK) << (3 * CORNER_WIDTH)))) &
                       LEFT_CORNER_MASK) |
                      (corners & ~LEFT_CORNER_MASK);
            adjust_orientations(LEFT_CORNER_ORIENTATION_ADJUSTMENT);
            break;
        case COUNTER_CLOCKWISE:
            // std::cout << "Left Counter-Clockwise Turn" << "\n";
            corners = ((((masked_corners & ULB_CORNER_MASK) >> (4 * CORNER_WIDTH)) |
                        (((masked_corners & ULF_CORNER_MASK) << (3 * CORNER_WIDTH)) |
                         ((masked_corners & DLB_CORNER_MASK) >> (3 * CORNER_WIDTH)) |
                         ((masked_corners & DLF_CORNER_MASK) << (4 * CORNER_WIDTH)))) &
                       LEFT_CORNER_MASK) |
                      (corners & ~LEFT_CORNER_MASK);
            adjust_orientations(LEFT_CORNER_ORIENTATION_ADJUSTMENT);
            break;
        case DOUBLE:
            // std::cout << "Left Double Turn" << "\n";
            corners = (((masked_corners & ULB_CORNER_MASK) >> (7 * CORNER_WIDTH)) |
                       (((masked_corners & ULF_CORNER_MASK) >> CORNER_WIDTH) |
                        ((masked_corners & DLB_CORNER_MASK) << CORNER_WIDTH) |
                        ((masked_corners & DLF_CORNER_MASK) << (7 * CORNER_WIDTH)))) |
                      (corners & ~LEFT_CORNER_MASK);
            break;
        }
        break;
    case FRONT:
        masked_corners = corners & FRONT_CORNER_MASK;
        switch (d)
        {
        case CLOCKWISE:
            // std::cout << "Front Clockwise Turn" << "\n";
            corners = ((((masked_corners & URF_CORNER_MASK) >> (4 * CORNER_WIDTH)) |
                        (((masked_corners & ULF_CORNER_MASK) << CORNER_WIDTH) |
                         ((masked_corners & DRF_CORNER_MASK) >> CORNER_WIDTH) |
                         ((masked_corners & DLF_CORNER_MASK) << (4 * CORNER_WIDTH)))) &
                       FRONT_CORNER_MASK) |
                      (corners & ~FRONT_CORNER_MASK);
            adjust_orientations(FRONT_CORNER_ORIENTATION_ADJUSTMENT);
            break;
        case COUNTER_CLOCKWISE:
            // std::cout << "Front Counter-Clockwise Turn" << "\n";
            corners = ((((masked_corners & URF_CORNER_MASK) >> CORNER_WIDTH) |
                        (((masked_corners & ULF_CORNER_MASK) >> (4 * CORNER_WIDTH)) |
                         ((masked_corners & DRF_CORNER_MASK) << (4 * CORNER_WIDTH)) |
                         ((masked_corners & DLF_CORNER_MASK) << CORNER_WIDTH))) &
                       FRONT_CORNER_MASK) |
                      (corners & ~FRONT_CORNER_MASK);
            adjust_orientations(FRONT_CORNER_ORIENTATION_ADJUSTMENT);
            break;
        case DOUBLE:
            // std::cout << "Front Double Turn" << "\n";
            corners = (((masked_corners & URF_CORNER_MASK) >> (5 * CORNER_WIDTH)) |
                       (((masked_corners & ULF_CORNER_MASK) >> (3 * CORNER_WIDTH)) |
                        ((masked_corners & DRF_CORNER_MASK) << (3 * CORNER_WIDTH)) |
                        ((masked_corners & DLF_CORNER_MASK) << (5 * CORNER_WIDTH)))) |
                      (corners & ~FRONT_CORNER_MASK);
            break;
        }
        break;
    case BACK:
        masked_corners = corners & BACK_CORNER_MASK;
        switch (d)
        {
        case CLOCKWISE:
            // std::cout << "Back Clockwise Turn" << "\n";
            corners = ((((masked_corners & ULB_CORNER_MASK) >> (4 * CORNER_WIDTH)) |
                        (((masked_corners & URB_CORNER_MASK) << CORNER_WIDTH) |
                         ((masked_corners & DLB_CORNER_MASK) >> CORNER_WIDTH) |
                         ((masked_corners & DRB_CORNER_MASK) << (4 * CORNER_WIDTH)))) &
                       BACK_CORNER_MASK) |
                      (corners & ~BACK_CORNER_MASK);
            adjust_orientations(BACK_CORNER_ORIENTATION_ADJUSTMENT);
            break;
        case COUNTER_CLOCKWISE:
            // std::cout << "Back Counter-Clockwise Turn" << "\n";
            corners = ((((masked_corners & ULB_CORNER_MASK) >> CORNER_WIDTH) |
                        (((masked_corners & URB_CORNER_MASK) >> (4 * CORNER_WIDTH)) |
                         ((masked_corners & DLB_CORNER_MASK) << (4 * CORNER_WIDTH)) |
                         ((masked_corners & DRB_CORNER_MASK) << CORNER_WIDTH))) &
                       BACK_CORNER_MASK) |
                      (corners & ~BACK_CORNER_MASK);
            adjust_orientations(BACK_CORNER_ORIENTATION_ADJUSTMENT);
            break;
        case DOUBLE:
            // std::cout << "Back Double Turn" << "\n";
            corners = (((masked_corners & ULB_CORNER_MASK) >> (5 * CORNER_WIDTH)) |
                       (((masked_corners & URB_CORNER_MASK) >> (3 * CORNER_WIDTH)) |
                        ((masked_corners & DLB_CORNER_MASK) << (3 * CORNER_WIDTH)) |
                        ((masked_corners & DRB_CORNER_MASK) << (5 * CORNER_WIDTH)))) |
                      (corners & ~BACK_CORNER_MASK);
            break;
        }
        break;
    case DOWN:
        masked_corners = corners & DOWN_CORNER_MASK;
        switch (d)
        {
        case CLOCKWISE:
            // std::cout << "Down Clockwise Turn" << "\n";
            corners = (((masked_corners << CORNER_WIDTH) & DOWN_CORNER_MASK) | (masked_corners >> (3 * CORNER_WIDTH))) |
                      (corners & ~DOWN_CORNER_MASK);
            break;
        case COUNTER_CLOCKWISE:
            // std::cout << "Down Counter-Clockwise Turn" << "\n";
            corners = ((masked_corners >> CORNER_WIDTH) | ((masked_corners << (3 * CORNER_WIDTH)) & DOWN_CORNER_MASK)) |
                      (corners & ~DOWN_CORNER_MASK);
            break;
        case DOUBLE:
            // std::cout << "Down Double Turn" << "\n";
            corners =
                (((masked_corners << (2 * CORNER_WIDTH)) | (masked_corners >> (2 * CORNER_WIDTH))) & DOWN_CORNER_MASK) |
                (corners & ~DOWN_CORNER_MASK);
            break;
        }
        break;
    }
}

void rubicks_2x2::print_cube()
{
    update_corners();

    std::cout << "  " << corners_list[0][0] << corners_list[1][0] << "\n";
    std::cout << "  " << corners_list[3][0] << corners_list[2][0] << "\n";
    std::cout << corners_list[0][1] << corners_list[3][2] << corners_list[3][1] << corners_list[2][2]
              << corners_list[2][1] << corners_list[1][2] << corners_list[1][1] << corners_list[0][2] << "\n";
    std::cout << corners_list[4][2] << corners_list[7][1] << corners_list[7][2] << corners_list[6][1]
              << corners_list[6][2] << corners_list[5][1] << corners_list[5][2] << corners_list[4][1] << "\n";
    std::cout << "  " << corners_list[7][0] << corners_list[6][0] << "\n";
    std::cout << "  " << corners_list[4][0] << corners_list[5][0] << "\n";
}

void rubicks_2x2::input_move(moves move)
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

void rubicks_2x2::input_scramble(std::string scramble)
{
    std::stringstream ss(scramble);
    std::string move;
    while (ss >> move)
    {
        moves current_move = get_move(move);
        input_move(current_move);
    }
}

// int main()
// {
//     // rubicks_2x2 cube;
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
//     test_rotations();

//     return 0;
// }