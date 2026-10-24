#include "../include/generate_scramble.h"
#include "../include/rubicks_2x2.h"
#include <cstdlib>

bool prune(moves new_move, moves previous_moves)
{
    switch (new_move)
    {
    case U:
        return previous_moves == U || previous_moves == U_PRIME || previous_moves == U2;
    case U_PRIME:
        return previous_moves == U || previous_moves == U_PRIME || previous_moves == U2;
    case U2:
        return previous_moves == U || previous_moves == U_PRIME || previous_moves == U2;
    case R:
        return previous_moves == R || previous_moves == R_PRIME || previous_moves == R2;
    case R_PRIME:
        return previous_moves == R || previous_moves == R_PRIME || previous_moves == R2;
    case R2:
        return previous_moves == R || previous_moves == R_PRIME || previous_moves == R2;
    case F:
        return previous_moves == F || previous_moves == F_PRIME || previous_moves == F2;
    case F_PRIME:
        return previous_moves == F || previous_moves == F_PRIME || previous_moves == F2;
    case F2:
        return previous_moves == F || previous_moves == F_PRIME || previous_moves == F2;
    case L:
        return previous_moves == L || previous_moves == L_PRIME || previous_moves == L2;
    case L_PRIME:
        return previous_moves == L || previous_moves == L_PRIME || previous_moves == L2;
    case L2:
        return previous_moves == L || previous_moves == L_PRIME || previous_moves == L2;
    case B:
        return previous_moves == B || previous_moves == B_PRIME || previous_moves == B2;
    case B_PRIME:
        return previous_moves == B || previous_moves == B_PRIME || previous_moves == B2;
    case B2:
        return previous_moves == B || previous_moves == B_PRIME || previous_moves == B2;
    case D:
        return previous_moves == D || previous_moves == D_PRIME || previous_moves == D2;
    case D_PRIME:
        return previous_moves == D || previous_moves == D_PRIME || previous_moves == D2;
    case D2:
        return previous_moves == D || previous_moves == D_PRIME || previous_moves == D2;
    case INVALID_MOVE:
        return false;
    }
}

std::string generate_scramble(int num_moves)
{
    moves scramble[num_moves];
    std::string scramble_string = "";

    for (int i = 0; i < num_moves; i++)
    {
        moves new_move = moves(int(std::rand() % NUM_MOVES));

        if (i == 0 || !prune(new_move, scramble[i - 1]))
        {
            scramble[i] = new_move;
        }
        else
        {
            i--;
        }
    }

    for (moves move : scramble)
    {
        scramble_string += print_move(move) + " ";
    }
    return scramble_string;
}