#include "../include/rubicks_2x2.h"
#include <iostream>
#include <bitset>
#include <cstring>
#include <sstream> 

std::string get_corner(corner c, orientation o)
{
	int x = (int)o;
	std::string corner_colours[8] = {"WRG", "WGO", "WOB", "WBR", "YGR", "YOG", "YBO", "YRB"};

	std::string current_corner = corner_colours[uint8_t(c)];
	return current_corner.substr(x, 1) + current_corner.substr((x + 1) % 3, 1) + current_corner.substr((x + 2) % 3, 1);
};

moves get_move(std::string move)
{
	if (move.substr(0, 1) == "U")
	{
		if (move.size() == 2)
		{
			if (move.substr(1) == "2")
				return U2;
			else return U_PRIME;
		}
		else return U;
	}
	if (move.substr(0, 1) == "F")
	{
		if (move.size() == 2)
		{
			if (move.substr(1) == "2")
				return F2;
			else return F_PRIME;
		}
		else return F;
	}
	if (move.substr(0, 1) == "R")
	{
		if (move.size() == 2)
		{
			if (move.substr(1) == "2")
				return R2;
			else return R_PRIME;
		}
		else return R;
	}
	if (move.substr(0, 1) == "L")
	{
		if (move.size() == 2)
		{
			if (move.substr(1) == "2")
				return L2;
			else return L_PRIME;
		}
		else return L;
	}
	if (move.substr(0, 1) == "B")
	{
		if (move.size() == 2)
		{
			if (move.substr(1) == "2")
				return B2;
			else return B_PRIME;
		}
		else return B;
	}
	if (move.substr(0, 1) == "D")
	{
		if (move.size() == 2)
		{
			if (move.substr(1) == "2")
				return D2;
			else return D_PRIME;
		}
		else return D;
	}
	else
	{
		return INVALID_MOVE;
	}
}

bool Rubicks_2x2::is_solved()
{
	return corners == solved_corners;
}

void Rubicks_2x2::update_corners()
{
	uint64_t _corners = corners;
	const uint64_t corner_mask = 0b111;
	const uint64_t orientation_mask = 0b11000;

	for (int i = 7; i >= 0; i--)
	{
		corners_list[i] = get_corner(static_cast<corner>((uint8_t)(corner_mask & _corners)), static_cast<orientation>((uint8_t)((orientation_mask & _corners) >> 3)));
		_corners >>= 6;
	}
}

void Rubicks_2x2::rotate(face f, direction d)
{
	uint64_t masked_corners;

	switch (f) {
		case UP:
			masked_corners = corners & up_corner_mask;
			switch (d)
			{
			case CLOCKWISE:
				corners = (((masked_corners >> 6) | ((masked_corners << 18)))  & up_corner_mask) | (corners & ~up_corner_mask);
				break;
			case COUNTER_CLOCKWISE:
				corners = ((masked_corners << 6) | ((masked_corners >> 18) & up_corner_mask)) | (corners & ~up_corner_mask);
				break;
			case DOUBLE:
				corners = ((masked_corners << 12) | ((masked_corners >> 12) & up_corner_mask)) | (corners & ~up_corner_mask);
				break;
			}
			break;
		case RIGHT:
			masked_corners = corners & right_corner_mask;
			switch (d)
			{
			case CLOCKWISE:
				corners = ((((masked_corners & URB_corner_mask) >> 24) | (((masked_corners & URF_corner_mask) << 6) | ((masked_corners & DRB_corner_mask) >> 6) | ((masked_corners & DRF_corner_mask) << 24)) + right_corner_orientation_adjustment) & right_corner_mask) | (corners & ~right_corner_mask);
				break;
			case COUNTER_CLOCKWISE:
				corners = ((((masked_corners & URB_corner_mask) >> 6) | (((masked_corners & URF_corner_mask) >> 24) | ((masked_corners & DRB_corner_mask) << 24) | ((masked_corners & DRF_corner_mask) << 6)) + right_corner_orientation_adjustment) & right_corner_mask) | (corners & ~right_corner_mask);
				break;
			case DOUBLE:
				corners = (((masked_corners & URB_corner_mask) >> 30) | (((masked_corners & URF_corner_mask) >> 18) | ((masked_corners & DRB_corner_mask) << 18) | ((masked_corners & DRF_corner_mask) << 30))) | (corners & ~right_corner_mask);
				break;
			}
			if ((corners & URB_corner_orientation_mask) == URB_corner_orientation_mask)
				corners -= URB_corner_orientation_mask;
			if ((corners & URF_corner_orientation_mask) == URF_corner_orientation_mask)
				corners -= URF_corner_orientation_mask;
			if ((corners & DRB_corner_orientation_mask) == DRB_corner_orientation_mask)
				corners -= DRB_corner_orientation_mask;
			if ((corners & DRF_corner_orientation_mask) == DRF_corner_orientation_mask)
				corners -= DRF_corner_orientation_mask;
			break;
		case LEFT:
			masked_corners = corners & left_corner_mask;
			switch (d)
			{
			case CLOCKWISE:
				corners = ((((masked_corners & ULB_corner_mask) >> 18) | (((masked_corners & ULF_corner_mask) >> 24) | ((masked_corners & DLB_corner_mask) << 24) | ((masked_corners & DLF_corner_mask) << 18)) + left_corner_orientation_adjustment) & left_corner_mask) | (corners & ~left_corner_mask);
				break;
			case COUNTER_CLOCKWISE:
				corners = ((((masked_corners & ULB_corner_mask) >> 24) | (((masked_corners & ULF_corner_mask) << 18) | ((masked_corners & DLB_corner_mask) >> 18) | ((masked_corners & DLF_corner_mask) << 24)) + left_corner_orientation_adjustment) & left_corner_mask) | (corners & ~left_corner_mask);
				break;
			case DOUBLE:
				corners = (((masked_corners & ULB_corner_mask) >> 42) | (((masked_corners & ULF_corner_mask) >> 6) | ((masked_corners & DLB_corner_mask) << 6) | ((masked_corners & DLF_corner_mask) << 42))) | (corners & ~left_corner_mask);
				break;
			}
			if ((corners & ULB_corner_orientation_mask) == ULB_corner_orientation_mask)
				corners -= ULB_corner_orientation_mask;
			if ((corners & ULF_corner_orientation_mask) == ULF_corner_orientation_mask)
				corners -= ULF_corner_orientation_mask;
			if ((corners & DLB_corner_orientation_mask) == DLB_corner_orientation_mask)
				corners -= DLB_corner_orientation_mask;
			if ((corners & DLF_corner_orientation_mask) == DLF_corner_orientation_mask)
				corners -= DLF_corner_orientation_mask;
			break;
		case FRONT:
			masked_corners = corners & front_corner_mask;
			switch (d)
			{
			case CLOCKWISE:
				corners = ((((masked_corners & URF_corner_mask) >> 24) | (((masked_corners & ULF_corner_mask) << 6) | ((masked_corners & DRF_corner_mask) >> 6) | ((masked_corners & DLF_corner_mask) << 24)) + front_corner_orientation_adjustment) & front_corner_mask) | (corners & ~front_corner_mask);
				break;
			case COUNTER_CLOCKWISE:
				corners = ((((masked_corners & URF_corner_mask) >> 6) | (((masked_corners & ULF_corner_mask) >> 24) | ((masked_corners & DRF_corner_mask) << 24) | ((masked_corners & DLF_corner_mask) << 6)) + front_corner_orientation_adjustment) & front_corner_mask) | (corners & ~front_corner_mask);
				break;
			case DOUBLE:
				corners = (((masked_corners & URF_corner_mask) >> 30) | (((masked_corners & ULF_corner_mask) >> 18) | ((masked_corners & DRF_corner_mask) << 18) | ((masked_corners & DLF_corner_mask) << 30))) | (corners & ~front_corner_mask);
				break;
			}
			if ((corners & URF_corner_orientation_mask) == URF_corner_orientation_mask)
				corners -= URF_corner_orientation_mask;
			if ((corners & ULF_corner_orientation_mask) == ULF_corner_orientation_mask)
				corners -= ULF_corner_orientation_mask;
			if ((corners & DRF_corner_orientation_mask) == DRF_corner_orientation_mask)
				corners -= DRF_corner_orientation_mask;
			if ((corners & DLF_corner_orientation_mask) == DLF_corner_orientation_mask)
				corners -= DLF_corner_orientation_mask;
			break;
		case BACK:
			masked_corners = corners & back_corner_mask;
			switch (d)
			{
			case CLOCKWISE:
				corners = ((((masked_corners & ULB_corner_mask) >> 24) | (((masked_corners & URB_corner_mask) << 6) | ((masked_corners & DLB_corner_mask) >> 6) | ((masked_corners & DRB_corner_mask) << 24)) + back_corner_orientation_adjustment) & back_corner_mask) | (corners & ~back_corner_mask);
				break;
			case COUNTER_CLOCKWISE:
				corners = ((((masked_corners & ULB_corner_mask) >> 6) | (((masked_corners & URB_corner_mask) >> 24) | ((masked_corners & DLB_corner_mask) << 24) | ((masked_corners & DRB_corner_mask) << 6)) + back_corner_orientation_adjustment) & back_corner_mask) | (corners & ~back_corner_mask);
				break;
			case DOUBLE:
				corners = (((masked_corners & ULB_corner_mask) >> 30) | (((masked_corners & URB_corner_mask) >> 18) | ((masked_corners & DLB_corner_mask) << 18) | ((masked_corners & DRB_corner_mask) << 30))) | (corners & ~back_corner_mask);
				break;
			}
			if ((corners & ULB_corner_orientation_mask) == ULB_corner_orientation_mask)
				corners -= ULB_corner_orientation_mask;
			if ((corners & URB_corner_orientation_mask) == URB_corner_orientation_mask)
				corners -= URB_corner_orientation_mask;
			if ((corners & DLB_corner_orientation_mask) == DLB_corner_orientation_mask)
				corners -= DLB_corner_orientation_mask;
			if ((corners & DRB_corner_orientation_mask) == DRB_corner_orientation_mask)
				corners -= DRB_corner_orientation_mask;
			break;
		case DOWN:
			masked_corners = corners & down_corner_mask;
			switch (d)
			{
			case CLOCKWISE:
				corners = (((masked_corners << 6) & down_corner_mask) | (masked_corners >> 18)) | (corners & ~down_corner_mask);
				break;
			case COUNTER_CLOCKWISE:
				corners = ((masked_corners >> 6) | ((masked_corners << 18) & down_corner_mask)) | (corners & ~down_corner_mask);
				break;
			case DOUBLE:
				corners = (((masked_corners << 12) | (masked_corners >> 12)) & down_corner_mask) | (corners & ~down_corner_mask);
				break;
			}
			break;
	}
	corners &= corner_orientation_readjustment_mask;
}

void Rubicks_2x2::print_cube()
{
	update_corners();

	std::cout << "  " << corners_list[0][0] << corners_list[1][0] << "\n";
	std::cout << "  " << corners_list[3][0] << corners_list[2][0] << "\n";
	std::cout << corners_list[0][1] << corners_list[3][2] << corners_list[3][1] << corners_list[2][2] << corners_list[2][1] << corners_list[1][2] << corners_list[1][1] << corners_list[0][2] << "\n";
	std::cout << corners_list[4][2] << corners_list[7][1] << corners_list[7][2] << corners_list[6][1] << corners_list[6][2] << corners_list[5][1] << corners_list[5][2] << corners_list[4][1] << "\n";
	std::cout << "  " << corners_list[7][0] << corners_list[6][0] << "\n";
	std::cout << "  " << corners_list[4][0] << corners_list[5][0] << "\n";
}

void Rubicks_2x2::input_scramble(std::string scramble)
{
	std::stringstream ss(scramble);
	std::string move;
	while (ss >> move)
	{
		moves current_move = get_move(move);
		switch (current_move) {
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
// 	Rubicks_2x2 cube;
// 	std::string scramble;
// 	while (true)
// 	{
// 		std::cout << "Enter your scramble: ";
// 		std::getline(std::cin, scramble);
// 		if (scramble == "q")
// 			break;
// 		cube.input_scramble(scramble);
// 		cube.print_cube();
// 	}
	
// 	return 0;
// }
