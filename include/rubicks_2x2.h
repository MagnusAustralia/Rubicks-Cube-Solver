#ifndef rubicks_2x2_H
#define rubicks_2x2_H

#include <bitset>

constexpr uint64_t SOLVED_CORNERS = 0b000000000001000010000011000100000101000110000111;

// Face Orientation Adjustment Masks
constexpr uint64_t RIGHT_CORNER_ORIENTATION_ADJUSTMENT = 0b000000010000001000000000000000001000010000000000;
constexpr uint64_t LEFT_CORNER_ORIENTATION_ADJUSTMENT = 0b001000000000000000010000010000000000000000001000;
constexpr uint64_t FRONT_CORNER_ORIENTATION_ADJUSTMENT = 0b000000000000010000001000000000000000001000010000;
constexpr uint64_t BACK_CORNER_ORIENTATION_ADJUSTMENT = 0b010000001000000000000000001000010000000000000000;

// Face Corner Masks
constexpr uint64_t UP_CORNER_MASK = 0b111111111111111111111111000000000000000000000000;
constexpr uint64_t RIGHT_CORNER_MASK = 0b000000111111111111000000000000111111111111000000;
constexpr uint64_t LEFT_CORNER_MASK = 0b111111000000000000111111111111000000000000111111;
constexpr uint64_t FRONT_CORNER_MASK = 0b000000000000111111111111000000000000111111111111;
constexpr uint64_t BACK_CORNER_MASK = 0b111111111111000000000000111111111111000000000000;
constexpr uint64_t DOWN_CORNER_MASK = 0b111111111111111111111111;

// Individual Corner Maks
constexpr uint64_t ULB_CORNER_MASK = 0b111111000000000000000000000000000000000000000000;
constexpr uint64_t URB_CORNER_MASK = 0b000000111111000000000000000000000000000000000000;
constexpr uint64_t URF_CORNER_MASK = 0b000000000000111111000000000000000000000000000000;
constexpr uint64_t ULF_CORNER_MASK = 0b000000000000000000111111000000000000000000000000;
constexpr uint64_t DLB_CORNER_MASK = 0b000000000000000000000000111111000000000000000000;
constexpr uint64_t DRB_CORNER_MASK = 0b000000000000000000000000000000111111000000000000;
constexpr uint64_t DRF_CORNER_MASK = 0b000000000000000000000000000000000000111111000000;
constexpr uint64_t DLF_CORNER_MASK = 0b000000000000000000000000000000000000000000111111;

// Edge Bitwise Values
const uint64_t CORNER_ORIENTATION_MASK = 0b11000;
const uint64_t CORNER_PIECE_MASK = 0b111;
const int NUM_MOVES = 18;
const int CORNER_WIDTH = 6;
const int NUM_CORNERS = 8;
const int CORNER_ORIENTATION_SHIFT = 3;
const int NUM_CORNER_ORIENTATIONS = 3;

/**
 * The face on the cube, which should be rotated
 */
enum face
{
    UP,    // White face
    RIGHT, // Orange face
    LEFT,  // Red face
    FRONT, // Blue face
    BACK,  // Green face
    DOWN   // Yellow face
};

/**
 * The orientation of a corner piece
 */
enum orientation
{
    DEFAULT,
    SINGLE_SHIFT,
    DOUBLE_SHIFT
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
 * The mapping of the corners in a Rubicks Cube, where U = Up,
 * D = Down, R = Right, L = Left, F = Front, B = Back
 */
enum corner : uint8_t
{
    ULB = 0b000, // The WRG corner piece
    URB = 0b001, // The WGO corner piece
    URF = 0b010, // The WOB corner piece
    ULF = 0b011, // The WBR corner piece
    DLB = 0b100, // The YGR corner piece
    DRB = 0b101, // The YOG corner piece
    DRF = 0b110, // The YBO corner piece
    DLF = 0b111  // The YRB corner piece
};

/**
 * The possible moves on the cube standard Rubick's Cube
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
 * Returns the string representing the move passed in
 *
 * @param move The move to convert to a string
 *
 * @return A string of the given move
 */
std::string print_move(moves move);

/**
 * Returns the move type of the input move
 *
 * @param move The move to convert
 *
 * @return The move of the passed in string
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
 * A struct which represents a 2x2 Rubick's Cube, with member functions to handle it's operation
 *
 * @field corners A number storing the state of the Rubick's Cube
 * @field corners_list[NUM_CORNERS] An array of strings, storing the colour order of the corners
 */
struct rubicks_2x2
{
    // ["WRG","WGO","WOB","WBR","YGR","YOG","YBO","YRB"]  # Positions = [ULB, URB, URF, ULF, DLB,
    // DRB, DRF, DLF]
    uint64_t corners;
    std::string corners_list[NUM_CORNERS];

    rubicks_2x2()
    {
        corners = SOLVED_CORNERS;
    };

    rubicks_2x2(uint64_t initial_corners)
    {
        corners = initial_corners;
    };

    /**
     * Returns the boolean result of if the given cube is solved
     *
     * @return True if the Rubick's Cube is solved, otherwise False
     */
    bool is_solved();

    /**
     * Uses the current corner state to update corners_list with the colours of the corners in the
     * current order
     */
    void update_corners();

    /**
     * Given the passed in adjustment, the corners orientations will be updated
     *
     * @param adjustment A number storing the orientation adjustment for a given rotation
     */
    void adjust_orientations(uint64_t adjustment);

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
     * Preforms the given move on the cube.
     *
     * @param move The move to apply
     */
    void input_move(moves move);

    /**
     * Preforms the given moves on the cube.
     *
     * @param scramble A string containing the cube scramble
     */
    void input_scramble(std::string scramble);
};

#endif