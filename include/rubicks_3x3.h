#ifndef rubicks_3x3_H
#define rubicks_3x3_H

#include "rubicks_2x2.h"

constexpr uint64_t SOLVED_EDGES = 0b000000000100010000110010000101001100011101000010010101001011;
constexpr uint64_t FRONT_EDGE_ORIENTATION_ADJUSTMENT = 0b000000000010000000001000010000000000000000000000001000000000;
constexpr uint64_t BACK_EDGE_ORIENTATION_ADJUSTMENT = 0b100000000000000000000000000000100001000010000000000000000000;
constexpr uint64_t UP_EDGE_MASK = 0b111111111111111111110000000000000000000000000000000000000000;
constexpr uint64_t RIGHT_EDGE_MASK = 0b000001111100000000001111100000111110000000000111110000000000;
constexpr uint64_t LEFT_EDGE_MASK = 0b000000000000000111110000011111000001111100000000000000011111;
constexpr uint64_t FRONT_EDGE_MASK = 0b000000000011111000001111111111000000000000000000001111100000;
constexpr uint64_t BACK_EDGE_MASK = 0b111110000000000000000000000000111111111111111000000000000000;
constexpr uint64_t DOWN_EDGE_MASK = 0b000000000000000000000000000000000000000011111111111111111111;

//
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

/**
 * The mapping of the edges in a Rubicks Cube
 */
enum edge : uint8_t
{
    UB = 0b0000, // WG
    UR = 0b0001, // WO
    UF = 0b0010, // WB
    UL = 0b0011, // WR
    FR = 0b0100, // BO
    FL = 0b0101, // BR
    BR = 0b0110, // GO
    BL = 0b0111, // GR
    DB = 0b1000, // YG
    DR = 0b1001, // YO
    DF = 0b1010, // YB
    DL = 0b1011  // YR
};

/**
 * Returns the colour orientation of a given edge
 *
 * @param e The edge of which to use
 * @param o The orientation of the given corner
 *
 * @return A string of the edge's colours given the orientation
 */
std::string get_edge(edge c, orientation o);

/**
 *
 */
struct rubicks_3x3
{
    // ["WG","WO","WB","WR","BO","BR","GO","GR","YG","YO","YB","YR"]  # Positions = [UB, UR, UF, UL,
    // FL, FR, BR, BL DB, DR, DF, DL]
    uint64_t edges;
    uint64_t corners;
    std::string corners_list[8];
    std::string edges_list[12];

    rubicks_3x3()
    {
        edges = SOLVED_EDGES;
        corners = SOLVED_CORNERS;
    };

    /**
     * Returns the boolean result of if the given cube is solved
     */
    bool is_solved();

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
     *
     */
    void adjust_orientations(uint64_t &corners, uint64_t adjustment);

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