#include "../include/generate_scramble.h"
#include "../include/rubicks_2x2.h"
#include <cstdlib>
#include <string>

bool prune(moves new_move, moves previous_move)
{
    // Adding the rule that moves must always follow the pattern of U -> D, R -> L, F -> B, and never the opposite
    switch (new_move)
    {
    case U:
        return previous_move == U || previous_move == U_PRIME || previous_move == U2 || previous_move == D ||
               previous_move == D_PRIME || previous_move == D2;
    case U_PRIME:
        return previous_move == U || previous_move == U_PRIME || previous_move == U2 || previous_move == D ||
               previous_move == D_PRIME || previous_move == D2;
    case U2:
        return previous_move == U || previous_move == U_PRIME || previous_move == U2 || previous_move == D ||
               previous_move == D_PRIME || previous_move == D2;
    case R:
        return previous_move == R || previous_move == R_PRIME || previous_move == R2 || previous_move == L ||
               previous_move == L_PRIME || previous_move == L2;
    case R_PRIME:
        return previous_move == R || previous_move == R_PRIME || previous_move == R2 || previous_move == L ||
               previous_move == L_PRIME || previous_move == L2;
    case R2:
        return previous_move == R || previous_move == R_PRIME || previous_move == R2 || previous_move == L ||
               previous_move == L_PRIME || previous_move == L2;
    case F:
        return previous_move == F || previous_move == F_PRIME || previous_move == F2 || previous_move == B ||
               previous_move == B_PRIME || previous_move == B2;
    case F_PRIME:
        return previous_move == F || previous_move == F_PRIME || previous_move == F2 || previous_move == B ||
               previous_move == B_PRIME || previous_move == B2;
    case F2:
        return previous_move == F || previous_move == F_PRIME || previous_move == F2 || previous_move == B ||
               previous_move == B_PRIME || previous_move == B2;
    case L:
        return previous_move == L || previous_move == L_PRIME || previous_move == L2;
    case L_PRIME:
        return previous_move == L || previous_move == L_PRIME || previous_move == L2;
    case L2:
        return previous_move == L || previous_move == L_PRIME || previous_move == L2;
    case B:
        return previous_move == B || previous_move == B_PRIME || previous_move == B2;
    case B_PRIME:
        return previous_move == B || previous_move == B_PRIME || previous_move == B2;
    case B2:
        return previous_move == B || previous_move == B_PRIME || previous_move == B2;
    case D:
        return previous_move == D || previous_move == D_PRIME || previous_move == D2;
    case D_PRIME:
        return previous_move == D || previous_move == D_PRIME || previous_move == D2;
    case D2:
        return previous_move == D || previous_move == D_PRIME || previous_move == D2;
    case INVALID_MOVE:
        return false;
    }
}

std::string generate_scramble(const int NUM_MOVES_TO_GENERATE)
{
    if (NUM_MOVES_TO_GENERATE <= 0)
    {
        return "";
    }

    moves scramble[NUM_MOVES_TO_GENERATE];
    std::string scramble_string = "";

    for (int i = 0; i < NUM_MOVES_TO_GENERATE; i++)
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