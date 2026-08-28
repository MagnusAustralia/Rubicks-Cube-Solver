#ifndef RUBICKS_3X3_H
#define RUBICKS_3X3_H

#include "rubicks_2x2.h"

/**
 * 
 */
enum Edges : uint8_t
{
	UB = 0b0000,
	UR = 0b0001,
	UF = 0b0010,
	UL = 0b0011,
	FL = 0b0100,
	FR = 0b0101,
	BR = 0b0110,
	BL = 0b0111,
	DB = 0b1000,
	DR = 0b1001,
	DF = 0b1010,
	DL = 0b1011
};

/**
 * 
 */
struct Rubicks_3x3 : Rubicks_2x2
{
	// ["WG","WO","WB","WR","BO","BR","GO","GR","YG","YO","YB","YR"]  # Positions = [UB, UR, UF, UL, FL, FR, BR, BL DB, DR, DF, DL]
	uint64_t edges;

	Rubicks_3x3()
	{
		edges = 0b000000000100010000110010000101001100011101000010010101001011;
		corners = 0b0000000001000100001100100001010011000111;
	};

	/**
	 * 
	 */
	void rotate(Rubicks_3x3 &r);

	/**
	 * 
	 */
	void print_cube(Rubicks_3x3 &r);
};

#endif