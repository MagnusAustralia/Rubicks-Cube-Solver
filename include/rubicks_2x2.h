#ifndef RUBICKS_2X2_H
#define RUBICKS_2X2_H

#include <bitset>

constexpr uint64_t solved_corners = 0b000000000001000010000011000100000101000110000111;
constexpr uint64_t right_corner_orientation_adjustment = 0b000000010000001000000000000000001000010000000000;
constexpr uint64_t left_corner_orientation_adjustment = 0b001000000000000000010000010000000000000000001000;
constexpr uint64_t front_corner_orientation_adjustment = 0b000000000000010000001000000000000000001000010000;
constexpr uint64_t back_corner_orientation_adjustment = 0b010000001000000000000000001000010000000000000000;
constexpr uint64_t up_corner_mask = 0b111111111111111111111111000000000000000000000000;
constexpr uint64_t right_corner_mask = 0b000000111111111111000000000000111111111111000000;
constexpr uint64_t left_corner_mask = 0b111111000000000000111111111111000000000000111111;
constexpr uint64_t front_corner_mask = 0b000000000000111111111111000000000000111111111111;
constexpr uint64_t back_corner_mask = 0b111111111111000000000000111111111111000000000000;
constexpr uint64_t down_corner_mask = 0b111111111111111111111111;

constexpr uint64_t ULB_corner_mask = 0b111111000000000000000000000000000000000000000000;
constexpr uint64_t URB_corner_mask = 0b000000111111000000000000000000000000000000000000;
constexpr uint64_t URF_corner_mask = 0b000000000000111111000000000000000000000000000000;
constexpr uint64_t ULF_corner_mask = 0b000000000000000000111111000000000000000000000000;
constexpr uint64_t DLB_corner_mask = 0b000000000000000000000000111111000000000000000000;
constexpr uint64_t DRB_corner_mask = 0b000000000000000000000000000000111111000000000000;
constexpr uint64_t DRF_corner_mask = 0b000000000000000000000000000000000000111111000000;
constexpr uint64_t DLF_corner_mask = 0b000000000000000000000000000000000000000000111111;

constexpr uint64_t ULB_corner_orientation_mask = 0b011000000000000000000000000000000000000000000000;
constexpr uint64_t URB_corner_orientation_mask = 0b000000011000000000000000000000000000000000000000;
constexpr uint64_t URF_corner_orientation_mask = 0b000000000000011000000000000000000000000000000000;
constexpr uint64_t ULF_corner_orientation_mask = 0b000000000000000000011000000000000000000000000000;
constexpr uint64_t DLB_corner_orientation_mask = 0b000000000000000000000000011000000000000000000000;
constexpr uint64_t DRB_corner_orientation_mask = 0b000000000000000000000000000000011000000000000000;
constexpr uint64_t DRF_corner_orientation_mask = 0b000000000000000000000000000000000000011000000000;
constexpr uint64_t DLF_corner_orientation_mask = 0b000000000000000000000000000000000000000000011000;

constexpr uint64_t corner_orientation_readjustment_mask = 0b011111011111011111011111011111011111011111011111;

/**
 * The face on the cube, which should be rotated
 */
enum face
{
	UP,
	RIGHT,
	LEFT,
	FRONT,
	BACK,
	DOWN
};

/**
 * The orientation of a corner piece
 */
enum orientation : uint8_t
{
	DEFAULT = 0,
	SINGLE_SHIFT = 1,
	DOUBLE_SHIFT = 2
};

/**
 * The direction of rotation
 */
enum direction
{
	CLOCKWISE,
	COUNTER_CLOCKWISE,
	DOUBLE
};

/**
 * The mapping of the corners in a Rubicks Cube
 */
enum corner : uint8_t
{
	ULB = 0b000, // WRG
	URB = 0b001, // WGO
	URF = 0b010, // WOB
	ULF = 0b011, // WBR
	DLB = 0b100, // YGR
	DRB = 0b101, // YOG
	DRF = 0b110, // YBO
	DLF = 0b111  // YRB
};

/**
 * The possible moves on the cube
 */
enum moves
{
	U,
	U_PRIME,
	U2,
	R,
	R_PRIME,
	R2,
	L,
	L_PRIME,
	L2,
	F,
	F_PRIME,
	F2,
	B,
	B_PRIME,
	B2,
	D,
	D_PRIME,
	D2,
	INVALID_MOVE
};

/**
 * Returns the move type of the input move
 * 
 * @param move The move to convert
 */
moves get_move(std::string move);

/**
 * Returns the colour orientation of a given corner
 * 
 * @param c The corner of which to use
 * @param o The orientation of the given corner
 * 
 * @return A string of the corner's colours given the orientation
 */
std::string get_corner(corner c, orientation o);

/**
 * 
 */
struct Rubicks_2x2
{
	// ["WRG","WGO","WOB","WBR","YGR","YOG","YBO","YRB"]  # Positions = [ULB, URB, URF, ULF, DLB, DRB, DRF, DLF]
	uint64_t corners;
	std::string corners_list[8];

	Rubicks_2x2()
	{
		corners = solved_corners;
	};

	/**
	 * Returns the boolean result of if the given cube is solved
	 */
	bool is_solved();

	/**
	 * Uses the current corner state to update corners_list with the colours of the corners in the current order
	 */
	void update_corners();

	/**
	 * Rotates the cube in the given face and direction
	 * 
	 * @param f The face of which to rotate
	 * @param d The direction of which to rotate the face
	 */
	void rotate(face f, direction d);

	/**
	 * Outputs a graphical display of the current cube state to the console
	 */
	void print_cube();

	/**
	 * Preforms the given moves on the cube.
	 * 
	 * @param scramble A string containing the cube scramble
	 */
	void input_scramble(std::string scramble);
};

#endif