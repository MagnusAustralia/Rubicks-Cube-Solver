#include "../include/rubicks_3x3.h"
#include "../include/rubicks_2x2.h"
#include <iostream>
#include <bitset>
#include <cstring>
#include <sstream> 

std::string get_edge(edge e, orientation o)
{
	int x = (int)o;
	std::string edge_colours[12] = {"WG", "WO", "WB", "WR", "BO", "BR", "GO", "GR", "YG", "YO", "YB", "YR"};

	std::string current_edge = edge_colours[uint8_t(e)];
	return current_edge.substr(x, 1) + current_edge.substr((x + 1) % 2, 1);
};

bool Rubicks_3x3::is_solved()
{
	return corners == solved_corners and edges == solved_edges;
}

void Rubicks_3x3::update_corners()
{
	uint64_t _corners = corners;
	const uint64_t corner_mask = 0b111;
	const uint64_t corner_orientation_mask = 0b11000;

	for (int i = 7; i >= 0; i--)
	{
		corners_list[i] = get_corner(static_cast<corner>((uint8_t)(corner_mask & _corners)), static_cast<orientation>((uint8_t)((corner_orientation_mask & _corners) >> 3)));
		_corners >>= 6;
	}
}

void Rubicks_3x3::update_edges()
{
	uint64_t _edges = edges;
	const uint64_t edge_mask = 0b1111;
	const uint64_t edge_orientation_mask = 0b10000;

	for (int i = 11; i >= 0; i--)
	{
		edges_list[i] = get_edge(static_cast<edge>((uint8_t)(edge_mask & _edges)), static_cast<orientation>((uint8_t)((edge_orientation_mask & _edges) >> 4)));
		_edges >>= 5;
	}
	
}

void Rubicks_3x3::rotate(face f, direction d)
{
	uint64_t masked_corners;
	uint64_t masked_edges;

	switch (f) {
		case UP:
			masked_corners = corners & up_corner_mask;
			masked_edges = edges & up_edge_mask;
			switch (d)
			{
			case CLOCKWISE:
				corners = (((masked_corners >> 6) | (masked_corners << 18))  & up_corner_mask) | (corners & ~up_corner_mask);
				edges = (((masked_edges >> 5) | (masked_edges << 15)) & up_edge_mask) | (edges & ~up_edge_mask);
				break;
			case COUNTER_CLOCKWISE:
				corners = ((masked_corners << 6) | ((masked_corners >> 18) & up_corner_mask)) | (corners & ~up_corner_mask);
				edges = (((masked_edges << 5) | (masked_edges >> 15)) & up_edge_mask) | (edges & ~up_edge_mask);
				break;
			case DOUBLE:
				corners = ((masked_corners << 12) | ((masked_corners >> 12) & up_corner_mask)) | (corners & ~up_corner_mask);
				edges = (((masked_edges >> 10) | (masked_edges << 10)) & up_edge_mask) | (edges & ~up_edge_mask);
				break;
			}
			break;
		case RIGHT:
			masked_corners = corners & right_corner_mask;
			masked_edges = edges & right_edge_mask;
			switch (d)
			{
			case CLOCKWISE:
				corners = ((((masked_corners & URB_corner_mask) >> 24) | (((masked_corners & URF_corner_mask) << 6) | ((masked_corners & DRB_corner_mask) >> 6) | ((masked_corners & DRF_corner_mask) << 24)) + right_corner_orientation_adjustment) & right_corner_mask) | (corners & ~right_corner_mask);
				edges = (((masked_edges & UR_edge_mask) >> 25) | (((masked_edges & FR_edge_mask) << 15) | ((masked_edges & BR_edge_mask) >> 15) | ((masked_edges & DR_edge_mask) << 25))) | (edges & ~right_edge_mask);
				break;
			case COUNTER_CLOCKWISE:
				corners = ((((masked_corners & URB_corner_mask) >> 6) | (((masked_corners & URF_corner_mask) >> 24) | ((masked_corners & DRB_corner_mask) << 24) | ((masked_corners & DRF_corner_mask) << 6)) + right_corner_orientation_adjustment) & right_corner_mask) | (corners & ~right_corner_mask);
				edges = (((masked_edges & UR_edge_mask) >> 15) | (((masked_edges & FR_edge_mask) >> 25) | ((masked_edges & BR_edge_mask) << 25) | ((masked_edges & DR_edge_mask) << 15))) | (edges & ~right_edge_mask);
				break;
			case DOUBLE:
				corners = (((masked_corners & URB_corner_mask) >> 30) | (((masked_corners & URF_corner_mask) >> 18) | ((masked_corners & DRB_corner_mask) << 18) | ((masked_corners & DRF_corner_mask) << 30))) | (corners & ~right_corner_mask);
				edges = (((masked_edges & UR_edge_mask) >> 40) | (((masked_edges & FR_edge_mask) >> 10) | ((masked_edges & BR_edge_mask) << 10) | ((masked_edges & DR_edge_mask) << 40))) | (edges & ~right_edge_mask);
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
			masked_edges = edges & left_edge_mask;
			switch (d)
			{
			case CLOCKWISE:
				corners = ((((masked_corners & ULB_corner_mask) >> 18) | (((masked_corners & ULF_corner_mask) >> 24) | ((masked_corners & DLB_corner_mask) << 24) | ((masked_corners & DLF_corner_mask) << 18)) + left_corner_orientation_adjustment) & left_corner_mask) | (corners & ~left_corner_mask);
				edges = (((masked_edges & UL_edge_mask) >> 10) | (((masked_edges & FL_edge_mask) >> 30) | ((masked_edges & BL_edge_mask) << 20) | ((masked_edges & DL_edge_mask) << 20))) | (edges & ~left_edge_mask);
				break;
			case COUNTER_CLOCKWISE:
				corners = ((((masked_corners & ULB_corner_mask) >> 24) | (((masked_corners & ULF_corner_mask) << 18) | ((masked_corners & DLB_corner_mask) >> 18) | ((masked_corners & DLF_corner_mask) << 24)) + left_corner_orientation_adjustment) & left_corner_mask) | (corners & ~left_corner_mask);
				edges = (((masked_edges & UL_edge_mask) >> 20) | (((masked_edges & FL_edge_mask) << 10) | ((masked_edges & BL_edge_mask) >> 20) | ((masked_edges & DL_edge_mask) << 30))) | (edges & ~left_edge_mask);
				break;
			case DOUBLE:
				corners = (((masked_corners & ULB_corner_mask) >> 42) | (((masked_corners & ULF_corner_mask) >> 6) | ((masked_corners & DLB_corner_mask) << 6) | ((masked_corners & DLF_corner_mask) << 42))) | (corners & ~left_corner_mask);
				edges = (((masked_edges & UL_edge_mask) >> 40) | (((masked_edges & FL_edge_mask) >> 10) | ((masked_edges & BL_edge_mask) << 10) | ((masked_edges & DL_edge_mask) << 40))) | (edges & ~left_edge_mask);
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
			masked_edges = edges & front_edge_mask;
			switch (d)
			{
			case CLOCKWISE:
				corners = ((((masked_corners & URF_corner_mask) >> 24) | (((masked_corners & ULF_corner_mask) << 6) | ((masked_corners & DRF_corner_mask) >> 6) | ((masked_corners & DLF_corner_mask) << 24)) + front_corner_orientation_adjustment) & front_corner_mask) | (corners & ~front_corner_mask);
				edges = (((masked_edges & UF_edge_mask) >> 10) | (((masked_edges & FR_edge_mask) >> 30) | ((masked_edges & FL_edge_mask) << 15) | ((masked_edges & DF_edge_mask) << 25))) | (edges & ~front_edge_mask);
				edges ^= front_edge_orientation_adjustment;
				break;
			case COUNTER_CLOCKWISE:
				corners = ((((masked_corners & URF_corner_mask) >> 6) | (((masked_corners & ULF_corner_mask) >> 24) | ((masked_corners & DRF_corner_mask) << 24) | ((masked_corners & DLF_corner_mask) << 6)) + front_corner_orientation_adjustment) & front_corner_mask) | (corners & ~front_corner_mask);
				edges = (((masked_edges & UF_edge_mask) >> 15) | (((masked_edges & FR_edge_mask) << 10) | ((masked_edges & FL_edge_mask) >> 25) | ((masked_edges & DF_edge_mask) << 30))) | (edges & ~front_edge_mask);
				edges ^= front_edge_orientation_adjustment;
				break;
			case DOUBLE:
				corners = (((masked_corners & URF_corner_mask) >> 30) | (((masked_corners & ULF_corner_mask) >> 18) | ((masked_corners & DRF_corner_mask) << 18) | ((masked_corners & DLF_corner_mask) << 30))) | (corners & ~front_corner_mask);
				edges = (((masked_edges & UF_edge_mask) >> 40) | (((masked_edges & FR_edge_mask) >> 5) | ((masked_edges & FL_edge_mask) << 5) | ((masked_edges & DF_edge_mask) << 40))) | (edges & ~front_edge_mask);
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
			masked_edges = edges & back_edge_mask;
			switch (d)
			{
			case CLOCKWISE:
				corners = ((((masked_corners & ULB_corner_mask) >> 24) | (((masked_corners & URB_corner_mask) << 6) | ((masked_corners & DLB_corner_mask) >> 6) | ((masked_corners & DRB_corner_mask) << 24)) + back_corner_orientation_adjustment) & back_corner_mask) | (corners & ~back_corner_mask);
				edges = (((masked_edges & UB_edge_mask) >> 35) | (((masked_edges & BR_edge_mask) << 30) | ((masked_edges & BL_edge_mask) >> 5) | ((masked_edges & DB_edge_mask) << 10))) | (edges & ~back_edge_mask);
				edges ^= back_edge_orientation_adjustment;
				break;
			case COUNTER_CLOCKWISE:
				corners = ((((masked_corners & ULB_corner_mask) >> 6) | (((masked_corners & URB_corner_mask) >> 24) | ((masked_corners & DLB_corner_mask) << 24) | ((masked_corners & DRB_corner_mask) << 6)) + back_corner_orientation_adjustment) & back_corner_mask) | (corners & ~back_corner_mask);
				edges = (((masked_edges & UB_edge_mask) >> 30) | (((masked_edges & BR_edge_mask) >> 10) | ((masked_edges & BL_edge_mask) << 35) | ((masked_edges & DB_edge_mask) << 5))) | (edges & ~back_edge_mask);
				edges ^= back_edge_orientation_adjustment;
				break;
			case DOUBLE:
				corners = (((masked_corners & ULB_corner_mask) >> 30) | (((masked_corners & URB_corner_mask) >> 18) | ((masked_corners & DLB_corner_mask) << 18) | ((masked_corners & DRB_corner_mask) << 30))) | (corners & ~back_corner_mask);
				edges = (((masked_edges & UB_edge_mask) >> 40) | (((masked_edges & BR_edge_mask) >> 5) | ((masked_edges & BL_edge_mask) << 5) | ((masked_edges & DB_edge_mask) << 40))) | (edges & ~back_edge_mask);
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
			masked_edges = edges & down_edge_mask;
			switch (d)
			{
			case CLOCKWISE:
				corners = (((masked_corners << 6) & down_corner_mask) | (masked_corners >> 18)) | (corners & ~down_corner_mask);
				edges = (((masked_edges << 5) | (masked_edges >> 15)) & down_edge_mask) | (edges & ~down_edge_mask);
				break;
			case COUNTER_CLOCKWISE:
				corners = ((masked_corners >> 6) | ((masked_corners << 18) & down_corner_mask)) | (corners & ~down_corner_mask);
				edges = (((masked_edges >> 5) | (masked_edges << 15)) & down_edge_mask) | (edges & ~down_edge_mask);
				break;
			case DOUBLE:
				corners = (((masked_corners << 12) | (masked_corners >> 12)) & down_corner_mask) | (corners & ~down_corner_mask);
				edges = (((masked_edges << 10) | (masked_edges >> 10)) & down_edge_mask) | (edges & ~down_edge_mask);
				break;
			}
			break;
	}
	corners &= corner_orientation_readjustment_mask;
}

void Rubicks_3x3::print_cube()
{
	update_corners();
	update_edges();

	std::cout << "   " << corners_list[0][0] << edges_list[0][0] << corners_list[1][0] << "\n";
	std::cout << "   " << edges_list[3][0] << "W" << edges_list[1][0] << "\n";
	std::cout << "   " << corners_list[3][0] << edges_list[2][0] << corners_list[2][0] << "\n";
	std::cout << corners_list[0][1] << edges_list[3][1] << corners_list[3][2] << corners_list[3][1] << edges_list[2][1] << corners_list[2][2] << corners_list[2][1] << edges_list[1][1] << corners_list[1][2] << corners_list[1][1] << edges_list[0][1] << corners_list[0][2] << "\n";
	std::cout << edges_list[7][1] << "R" << edges_list[5][1] << edges_list[5][0] << "B" << edges_list[4][0] << edges_list[4][1] << "O" << edges_list[6][1] << edges_list[6][0] << "G" << edges_list[7][0] << "\n";
	std::cout << corners_list[4][2] << edges_list[11][1] << corners_list[7][1] << corners_list[7][2] << edges_list[10][1] << corners_list[6][1] << corners_list[6][2] << edges_list[9][1] << corners_list[5][1] << corners_list[5][2] << edges_list[8][1] << corners_list[4][1] << "\n";
	std::cout << "   " << corners_list[7][0] << edges_list[10][0] << corners_list[6][0] << "\n";
	std::cout << "   " << edges_list[11][0] << "Y" << edges_list[9][0] << "\n";
	std::cout << "   " << corners_list[4][0] << edges_list[8][0] << corners_list[5][0] << "\n";
}

void Rubicks_3x3::print_cube(int i)
{
	update_corners();

	std::cout << "  " << corners_list[0][0] << corners_list[1][0] << "\n";
	std::cout << "  " << corners_list[3][0] << corners_list[2][0] << "\n";
	std::cout << corners_list[0][1] << corners_list[3][2] << corners_list[3][1] << corners_list[2][2] << corners_list[2][1] << corners_list[1][2] << corners_list[1][1] << corners_list[0][2] << "\n";
	std::cout << corners_list[4][2] << corners_list[7][1] << corners_list[7][2] << corners_list[6][1] << corners_list[6][2] << corners_list[5][1] << corners_list[5][2] << corners_list[4][1] << "\n";
	std::cout << "  " << corners_list[7][0] << corners_list[6][0] << "\n";
	std::cout << "  " << corners_list[4][0] << corners_list[5][0] << "\n";
}

void Rubicks_3x3::input_scramble(std::string scramble)
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
	Rubicks_3x3 cube;
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
	cube.print_cube();
	std::cout << "\n";
	cube.rotate(BACK, COUNTER_CLOCKWISE);
	cube.print_cube();
	
	return 0;
}
