#ifndef rubicks_3x3_H
#define rubicks_3x3_H

#include "rubicks_2x2.h"

// Edge Orientation Masks
constexpr uint64_t FRONT_EDGE_ORIENTATION_ADJUSTMENT = 0b000000000010000000001000010000000000000000000000001000000000;
constexpr uint64_t BACK_EDGE_ORIENTATION_ADJUSTMENT = 0b100000000000000000000000000000100001000010000000000000000000;

// Face Edge Masks
constexpr uint64_t UP_EDGE_MASK = 0b111111111111111111110000000000000000000000000000000000000000;
constexpr uint64_t RIGHT_EDGE_MASK = 0b000001111100000000001111100000111110000000000111110000000000;
constexpr uint64_t LEFT_EDGE_MASK = 0b000000000000000111110000011111000001111100000000000000011111;
constexpr uint64_t FRONT_EDGE_MASK = 0b000000000011111000001111111111000000000000000000001111100000;
constexpr uint64_t BACK_EDGE_MASK = 0b111110000000000000000000000000111111111111111000000000000000;
constexpr uint64_t DOWN_EDGE_MASK = 0b000000000000000000000000000000000000000011111111111111111111;

// Individual Edge Maks
constexpr uint64_t UB_EDGE_MASK = 0b111110000000000000000000000000000000000000000000000000000000;
constexpr uint64_t UR_EDGE_MASK = 0b000001111100000000000000000000000000000000000000000000000000;
constexpr uint64_t UF_EDGE_MASK = 0b000000000011111000000000000000000000000000000000000000000000;
constexpr uint64_t UL_EDGE_MASK = 0b000000000000000111110000000000000000000000000000000000000000;
constexpr uint64_t FR_EDGE_MASK = 0b000000000000000000001111100000000000000000000000000000000000;
constexpr uint64_t FL_EDGE_MASK = 0b000000000000000000000000011111000000000000000000000000000000;
constexpr uint64_t BR_EDGE_MASK = 0b000000000000000000000000000000111110000000000000000000000000;
constexpr uint64_t BL_EDGE_MASK = 0b000000000000000000000000000000000001111100000000000000000000;
constexpr uint64_t DB_EDGE_MASK = 0b000000000000000000000000000000000000000011111000000000000000;
constexpr uint64_t DR_EDGE_MASK = 0b000000000000000000000000000000000000000000000111110000000000;
constexpr uint64_t DF_EDGE_MASK = 0b000000000000000000000000000000000000000000000000001111100000;
constexpr uint64_t DL_EDGE_MASK = 0b000000000000000000000000000000000000000000000000000000011111;

// Edge States
constexpr uint64_t SOLVED_EDGES = 0b000000000100010000110010000101001100011101000010010101001011;
constexpr uint64_t EDGE_CASE_ONE_3X3 = 0b000000000100010000110010000101011000110001100011000110001100;
constexpr uint64_t EDGE_CASE_TWO_3X3 = 0b011000110001100011000110001100001100011101000010010101001011;
constexpr uint64_t EDGE_CASE_MIDDLE_SLICE_3X3 = 0b011000110001100011000010000101001100011101100011000110001100;

// Edge Bitwise Values
constexpr uint64_t EDGE_MASK = 0b11111;
constexpr uint64_t EDGE_PIECE_MASK = 0b01111;
constexpr uint64_t EDGE_ORIENTATION_MASK = 0b10000;
constexpr uint64_t EDGE_ADJUSTMENT = 0b10000; // Used to undo the orientation of blank edges
constexpr int EDGE_ORIENTATION_SHIFT = 4;
constexpr int EDGE_WIDTH = 5;
const int NUM_EDGES = 12;
const int NUM_EDGE_ORIENTATIONS = 2;

/**
 * The list of cube cases.
 */
enum cube_case
{
    STANDARD_CUBE,         // The standard cube case.
    CASE_ONE_3X3,          // The cube case with only the first 6 edges.
    CASE_TWO_3X3,          // The cube case with only the last 6 edges.
    CASE_MIDDLE_SLICE_3X3, // The cube case with only the middle 4 edges.
    CASE_THREE_3X3         // The cube case with only the first 6 edges, excluding the middle slice
};

/**
 * The mapping of the edges in a Rubicks Cube
 */
enum edge : uint8_t
{
    UB = 0b0000, // The WG edge piece
    UR = 0b0001, // The WO edge piece
    UF = 0b0010, // The WB edge piece
    UL = 0b0011, // The WR edge piece
    FR = 0b0100, // The BO edge piece
    FL = 0b0101, // The BR edge piece
    BR = 0b0110, // The GO edge piece
    BL = 0b0111, // The GR edge piece
    DB = 0b1000, // The YG edge piece
    DR = 0b1001, // The YO edge piece
    DF = 0b1010, // The YB edge piece
    DL = 0b1011, // The YR edge piece
    NN = 0b1100  // The blank edge piece
};

/**
 * Returns the colour orientation of a given edge
 *
 * @param e The edge of which to use
 * @param o The orientation of the given corner
 *
 * @return A string of the edge's colours given the orientation
 */
std::string get_edge(edge c, orientation o = DEFAULT);

/**
 * A struct which represents a 3x3 Rubick's Cube, with member functions to handle it's operation
 *
 * @field edges A number storing the state of the edges of the Rubick's Cube
 * @field corners A number storing the state of the corners of the Rubick's Cube
 * @field corners_list[NUM_CORNERS] An array of strings, storing the colour order of the corners
 * @field edges_list[NUM_EDGES] An array of strings, storing the colour order of the edges
 */
struct rubicks_3x3
{
    // ["WG","WO","WB","WR","BO","BR","GO","GR","YG","YO","YB","YR"]  # Positions = [UB, UR, UF, UL,
    // FL, FR, BR, BL DB, DR, DF, DL]
    uint64_t edges;
    uint64_t corners;
    std::string corners_list[NUM_CORNERS];
    std::string edges_list[NUM_EDGES];
    bool keep_blank_edge_orientations;

    rubicks_3x3(const cube_case case_3x3 = STANDARD_CUBE, bool blank_edge_orientations = false)
    {
        corners = SOLVED_CORNERS;
        keep_blank_edge_orientations = blank_edge_orientations;

        switch (case_3x3)
        {
        case STANDARD_CUBE:
            edges = SOLVED_EDGES;
            break;
        case CASE_ONE_3X3:
            edges = EDGE_CASE_ONE_3X3;
            break;
        case CASE_TWO_3X3:
            edges = EDGE_CASE_TWO_3X3;
            break;
        case CASE_MIDDLE_SLICE_3X3:
            edges = EDGE_CASE_MIDDLE_SLICE_3X3;
            break;
        case CASE_THREE_3X3:
            edges = SOLVED_EDGES;
            break;
        }
    };

    /**
     * Returns the boolean result of if the given cube is solved
     *
     * @param variation The cube variation to check against
     *
     * @return True if the Rubick's Cube is solved, otherwise False
     */
    bool is_solved(const cube_case variation = STANDARD_CUBE) const;

    /**
     * Uses the current corner state to update corners_list with the colours of the corners in the
     * current order
     */
    void update_corners();

    /**
     * Uses the current edge state to update edges_list with the colours of the edges in the current
     * order
     */
    void update_edges();

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