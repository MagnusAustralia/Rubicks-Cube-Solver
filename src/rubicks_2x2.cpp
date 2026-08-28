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
	return corners == 0b000000000001000010000011000100000101000110000111;
}

void Rubicks_2x2::update_corners()
{
	uint64_t _corners = corners;
	uint64_t corner_mask = 0b111;
	uint64_t orientation_mask = 0b11000;

	for (int i = 7; i >= 0; i--)
	{
		corners_list[i] = get_corner(static_cast<corner>((uint8_t)(corner_mask & _corners)), static_cast<orientation>((uint8_t)((orientation_mask & _corners) >> 3)));
		_corners >>= 6;
	}
}

void Rubicks_2x2::rotate(face f, direction d)
{
	uint64_t mask;
	uint64_t temp;
	uint64_t mask_a;
	uint64_t mask_b;
	uint64_t mask_c;
	uint64_t mask_d;

	const uint64_t right_orientation_adjustment = 0b000000010000001000000000000000001000010000000000;
	const uint64_t left_orientation_adjustment = 0b001000000000000000010000010000000000000000001000;
	const uint64_t front_orientation_adjustment = 0b000000000000010000001000000000000000001000010000;
	const uint64_t back_orientation_adjustment = 0b010000001000000000000000001000010000000000000000;

	switch (f) {
		case UP:
			mask = 0b111111111111111111111111000000000000000000000000;
			temp = corners & mask;
			switch (d)
			{
			case CLOCKWISE:
				corners = (((temp >> 6) | ((temp << 18)))  & mask) | (corners & ~mask);
				break;
			case COUNTER_CLOCKWISE:
				corners = ((temp << 6) | ((temp >> 18) & mask)) | (corners & ~mask);
				break;
			case DOUBLE:
				corners = ((temp << 12) | ((temp >> 12) & mask)) | (corners & ~mask);
				break;
			}
			break;
		case RIGHT:
			mask = 0b000000111111111111000000000000111111111111000000; // URB URF DRB DRF
			mask_a = 0b000000111111000000000000000000000000000000000000;
			mask_b = 0b000000000000111111000000000000000000000000000000;
			mask_c = 0b000000000000000000000000000000111111000000000000;
			mask_d = 0b000000000000000000000000000000000000111111000000;
			temp = corners & mask;
			switch (d)
			{
			case CLOCKWISE:
				corners = ((((temp & mask_a) >> 24) | (((temp & mask_b) << 6) | ((temp & mask_c) >> 6) | ((temp & mask_d) << 24)) + right_orientation_adjustment) & mask) | (corners & ~mask);
				break;
			case COUNTER_CLOCKWISE:
				corners = ((((temp & mask_a) >> 6) | (((temp & mask_b) >> 24) | ((temp & mask_c) << 24) | ((temp & mask_d) << 6)) + right_orientation_adjustment) & mask) | (corners & ~mask);
				break;
			case DOUBLE:
				corners = (((temp & mask_a) >> 30) | (((temp & mask_b) >> 18) | ((temp & mask_c) << 18) | ((temp & mask_d) << 30))) | (corners & ~mask);
				break;
			}
			if ((corners & 0b000000000000000000000000000000000000011000000000) == 1536)
				corners -= 0b000000000000000000000000000000000000011000000000;
			if ((corners & 0b000000000000000000000000000000011000000000000000) == 98304)
				corners -= 0b000000000000000000000000000000011000000000000000;
			if ((corners & 0b000000000000011000000000000000000000000000000000) == 25769803776L)
				corners -= 0b000000000000011000000000000000000000000000000000;
			if ((corners & 0b000000011000000000000000000000000000000000000000) == 1649267441664L)
				corners -= 0b000000011000000000000000000000000000000000000000;
			break;
		case LEFT:
			mask = 0b111111000000000000111111111111000000000000111111; // ULB ULF DLB DLF
			mask_a = 0b111111000000000000000000000000000000000000000000;
			mask_b = 0b000000000000000000111111000000000000000000000000;
			mask_c = 0b000000000000000000000000111111000000000000000000;
			mask_d = 0b000000000000000000000000000000000000000000111111;
			temp = corners & mask;
			switch (d)
			{
			case CLOCKWISE:
				corners = ((((temp & mask_a) >> 18) | (((temp & mask_b) >> 24) | ((temp & mask_c) << 24) | ((temp & mask_d) << 18)) + left_orientation_adjustment) & mask) | (corners & ~mask);
				break;
			case COUNTER_CLOCKWISE:
				corners = ((((temp & mask_a) >> 24) | (((temp & mask_b) << 18) | ((temp & mask_c) >> 18) | ((temp & mask_d) << 24)) + left_orientation_adjustment) & mask) | (corners & ~mask);
				break;
			case DOUBLE:
				corners = (((temp & mask_a) >> 42) | (((temp & mask_b) >> 6) | ((temp & mask_c) << 6) | ((temp & mask_d) << 42))) | (corners & ~mask);
				break;
			}
			if ((corners & 0b000000000000000000000000000000000000000000011000) == 24)
				corners -= 0b000000000000000000000000000000000000000000011000;
			if ((corners & 0b000000000000000000000000011000000000000000000000) == 6291456)
				corners -= 0b000000000000000000000000011000000000000000000000;
			if ((corners & 0b000000000000000000011000000000000000000000000000) == 402653184)
				corners -= 0b000000000000000000011000000000000000000000000000;
			if ((corners & 0b011000000000000000000000000000000000000000000000) == 105553116266496L)
				corners -= 0b011000000000000000000000000000000000000000000000;
			break;
		case FRONT:
			mask = 0b000000000000111111111111000000000000111111111111; // URF ULF DRF DLF
			mask_a = 0b000000000000111111000000000000000000000000000000;
			mask_b = 0b000000000000000000111111000000000000000000000000;
			mask_c = 0b000000000000000000000000000000000000111111000000;
			mask_d = 0b000000000000000000000000000000000000000000111111;
			temp = corners & mask;
			switch (d)
			{
			case CLOCKWISE:
				corners = ((((temp & mask_a) >> 24) | (((temp & mask_b) << 6) | ((temp & mask_c) >> 6) | ((temp & mask_d) << 24)) + front_orientation_adjustment) & mask) | (corners & ~mask);
				break;
			case COUNTER_CLOCKWISE:
				corners = ((((temp & mask_a) >> 6) | (((temp & mask_b) >> 24) | ((temp & mask_c) << 24) | ((temp & mask_d) << 6)) + front_orientation_adjustment) & mask) | (corners & ~mask);
				break;
			case DOUBLE:
				corners = (((temp & mask_a) >> 30) | (((temp & mask_b) >> 18) | ((temp & mask_c) << 18) | ((temp & mask_d) << 30))) | (corners & ~mask);
				break;
			}
			if ((corners & 0b000000000000000000000000000000000000000000011000) == 24)
				corners -= 0b000000000000000000000000000000000000000000011000;
			if ((corners & 0b000000000000000000000000000000000000011000000000) == 1536)
				corners -= 0b000000000000000000000000000000000000011000000000;
			if ((corners & 0b000000000000000000011000000000000000000000000000) == 402653184)
				corners -= 0b000000000000000000011000000000000000000000000000;
			if ((corners & 0b000000000000011000000000000000000000000000000000) == 25769803776L)
				corners -= 0b000000000000011000000000000000000000000000000000;
			break;
		case BACK:
			mask = 0b111111111111000000000000111111111111000000000000; // ULB URB DLB DRB
			mask_a = 0b111111000000000000000000000000000000000000000000;
			mask_b = 0b000000111111000000000000000000000000000000000000;
			mask_c = 0b000000000000000000000000111111000000000000000000;
			mask_d = 0b000000000000000000000000000000111111000000000000;
			temp = corners & mask;
			switch (d)
			{
			case CLOCKWISE:
				corners = ((((temp & mask_a) >> 24) | (((temp & mask_b) << 6) | ((temp & mask_c) >> 6) | ((temp & mask_d) << 24)) + back_orientation_adjustment) & mask) | (corners & ~mask);
				break;
			case COUNTER_CLOCKWISE:
				corners = ((((temp & mask_a) >> 6) | (((temp & mask_b) >> 24) | ((temp & mask_c) << 24) | ((temp & mask_d) << 6)) + back_orientation_adjustment) & mask) | (corners & ~mask);
				break;
			case DOUBLE:
				corners = (((temp & mask_a) >> 30) | (((temp & mask_b) >> 18) | ((temp & mask_c) << 18) | ((temp & mask_d) << 30))) | (corners & ~mask);
				break;
			}
			if ((corners & 0b000000000000000000000000000000011000000000000000) == 98304)
				corners -= 0b000000000000000000000000000000011000000000000000;
			if ((corners & 0b000000000000000000000000011000000000000000000000) == 6291456)
				corners -= 0b000000000000000000000000011000000000000000000000;
			if ((corners & 0b000000011000000000000000000000000000000000000000) == 1649267441664L)
				corners -= 0b000000011000000000000000000000000000000000000000;
			if ((corners & 0b011000000000000000000000000000000000000000000000) == 105553116266496L)
				corners -= 0b011000000000000000000000000000000000000000000000;
			break;
		case DOWN:
			mask = 0b111111111111111111111111;
			temp = corners & mask;
			switch (d)
			{
			case CLOCKWISE:
				corners = (((temp << 6) & mask) | (temp >> 18)) | (corners & ~mask);
				break;
			case COUNTER_CLOCKWISE:
				corners = ((temp >> 6) | ((temp << 18) & mask)) | (corners & ~mask);
				break;
			case DOUBLE:
				corners = (((temp << 12) | (temp >> 12)) & mask) | (corners & ~mask);
				break;
			}
			break;
	}
	corners &= 0b011111011111011111011111011111011111011111011111;
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


int main()
{
	Rubicks_2x2 cube;
	std::string scramble;
	std::cout << "Enter your scramble: ";
	std::getline(std::cin, scramble);
	cube.input_scramble(scramble);
	cube.print_cube();
	return 0;
}
