#include "../include/generate_scramble.h"
#include "../include/kociembas_algorithm.h"
#include "../include/korfs_algorithm.h"
#include "../include/rubicks_2x2.h"
#include "../include/rubicks_3x3.h"
#include "depth_first_search.h"
#include "splashkit.h";
#include <iostream>
#include <string>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const color FILL_COLOUR = COLOR_BLUE;
const color OUTLINE_COLOUR = COLOR_GRAY;
const color BACKGROUND_COLOUR = COLOR_BLACK;
const color HIGHLIGHT_COLOUR = COLOR_LIGHT_BLUE;
const font FONT = load_font("Custom Font", "Audiowide-Regular.ttf");
const int FONT_SIZE = 20;
// const int REFRESH_RATE = 60;
const int BUFFER_X = 4;
const int BUFFER_Y = 4;
const int REFRESH_RATE = 60;
const int DISPLAY_MOVES_ACTIVE_TIME = 3000;

/**
 * The list of enum options for the cube type.
 *
 * @option CUBE_TYPE_2X2 The option to use a 2x2 Rubicks Cube.
 * @option CUBE_TYPE_3X3 The option to use a 3x3 Rubicks Cube.
 */
enum current_cube_type
{
    CUBE_TYPE_2X2,
    CUBE_TYPE_3X3
};

/**
 * This struct stores the cube_data used in the cli, with a store of which cube type is currently being worked with.
 *
 * @field current The current_cube_type being used
 * @field r2 The data for a 2x2 Rubicks Cube when it is selected
 * @field r3 The data for a 3x3 Rubicks Cube when it is selected
 */
struct cube_data
{
    current_cube_type current;
    rubicks_2x2 r2;
    rubicks_3x3 r3;

    cube_data()
    {
        current = CUBE_TYPE_2X2;
        r2 = rubicks_2x2();
    };
};

void visualise_cube(cube_data &data)
{
    clear_screen(BACKGROUND_COLOUR);

    switch (data.current)
    {
    case CUBE_TYPE_2X2:
    {
        double width = (double)SCREEN_WIDTH / 10;
        double height = (double)SCREEN_HEIGHT / 8;

        data.r2.update_corners();
        char colour_map[6][8] = {
            {' ', ' ', data.r2.corners_list[0][0], data.r2.corners_list[1][0], ' ', ' ', ' ', ' '},
            {' ', ' ', data.r2.corners_list[3][0], data.r2.corners_list[2][0], ' ', ' ', ' ', ' '},
            {data.r2.corners_list[0][1], data.r2.corners_list[3][2], data.r2.corners_list[3][1],
             data.r2.corners_list[2][2], data.r2.corners_list[2][1], data.r2.corners_list[1][2],
             data.r2.corners_list[1][1], data.r2.corners_list[0][2]},
            {data.r2.corners_list[4][2], data.r2.corners_list[7][1], data.r2.corners_list[7][2],
             data.r2.corners_list[6][1], data.r2.corners_list[6][2], data.r2.corners_list[5][1],
             data.r2.corners_list[5][2], data.r2.corners_list[4][1]},
            {' ', ' ', data.r2.corners_list[7][0], data.r2.corners_list[6][0], ' ', ' ', ' ', ' '},
            {' ', ' ', data.r2.corners_list[4][0], data.r2.corners_list[5][0], ' ', ' ', ' ', ' '},
        };

        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                color fill_colour;
                switch (colour_map[i][j])
                {
                case ' ':
                    fill_colour = BACKGROUND_COLOUR;
                    break;
                case 'W':
                    fill_colour = COLOR_WHITE;
                    break;
                case 'Y':
                    fill_colour = COLOR_YELLOW;
                    break;
                case 'B':
                    fill_colour = COLOR_BLUE;
                    break;
                case 'G':
                    fill_colour = COLOR_BRIGHT_GREEN;
                    break;
                case 'R':
                    fill_colour = COLOR_RED;
                    break;
                case 'O':
                    fill_colour = COLOR_ORANGE;
                    break;
                default:
                    std::cerr << "Invalid Colour\n";
                }

                fill_rectangle(fill_colour, (1 + j) * width, (1 + i) * height, width, height);
                draw_rectangle(COLOR_BLACK, (1 + j) * width, (1 + i) * height, width, height);
            }
        }

        break;
    }

    case CUBE_TYPE_3X3:
    {
        double width = (double)SCREEN_WIDTH / 14;
        double height = (double)SCREEN_HEIGHT / 11;

        data.r3.update_corners();
        data.r3.update_edges();
        char colour_map[9][12] = {
            {' ', ' ', ' ', data.r3.corners_list[0][0], data.r3.edges_list[0][0], data.r3.corners_list[1][0], ' ', ' ',
             ' ', ' ', ' ', ' '},
            {' ', ' ', ' ', data.r3.edges_list[3][0], 'W', data.r3.edges_list[1][0], ' ', ' ', ' ', ' ', ' ', ' '},
            {' ', ' ', ' ', data.r3.corners_list[3][0], data.r3.edges_list[2][0], data.r3.corners_list[2][0], ' ', ' ',
             ' ', ' ', ' ', ' '},
            {data.r3.corners_list[0][1], data.r3.edges_list[3][1], data.r3.corners_list[3][2],
             data.r3.corners_list[3][1], data.r3.edges_list[2][1], data.r3.corners_list[2][2],
             data.r3.corners_list[2][1], data.r3.edges_list[1][1], data.r3.corners_list[1][2],
             data.r3.corners_list[1][1], data.r3.edges_list[0][1], data.r3.corners_list[0][2]},
            {data.r3.edges_list[7][1], 'R', data.r3.edges_list[5][1], data.r3.edges_list[5][0], 'B',
             data.r3.edges_list[4][0], data.r3.edges_list[4][1], 'O', data.r3.edges_list[6][1],
             data.r3.edges_list[6][0], 'G', data.r3.edges_list[7][0]},
            {data.r3.corners_list[4][2], data.r3.edges_list[11][1], data.r3.corners_list[7][1],
             data.r3.corners_list[7][2], data.r3.edges_list[10][1], data.r3.corners_list[6][1],
             data.r3.corners_list[6][2], data.r3.edges_list[9][1], data.r3.corners_list[5][1],
             data.r3.corners_list[5][2], data.r3.edges_list[8][1], data.r3.corners_list[4][1]},
            {' ', ' ', ' ', data.r3.corners_list[7][0], data.r3.edges_list[10][0], data.r3.corners_list[6][0], ' ', ' ',
             ' ', ' ', ' ', ' '},
            {' ', ' ', ' ', data.r3.edges_list[11][0], 'Y', data.r3.edges_list[9][0], ' ', ' ', ' ', ' ', ' ', ' '},
            {' ', ' ', ' ', data.r3.corners_list[4][0], data.r3.edges_list[8][0], data.r3.corners_list[5][0], ' ', ' ',
             ' ', ' ', ' ', ' '},
        };

        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 12; j++)
            {
                color fill_colour;
                switch (colour_map[i][j])
                {
                case ' ':
                    fill_colour = BACKGROUND_COLOUR;
                    break;
                case 'W':
                    fill_colour = COLOR_WHITE;
                    break;
                case 'Y':
                    fill_colour = COLOR_YELLOW;
                    break;
                case 'B':
                    fill_colour = COLOR_BLUE;
                    break;
                case 'G':
                    fill_colour = COLOR_BRIGHT_GREEN;
                    break;
                case 'R':
                    fill_colour = COLOR_RED;
                    break;
                case 'O':
                    fill_colour = COLOR_ORANGE;
                    break;
                default:
                    std::cerr << "Invalid Colour\n";
                }

                fill_rectangle(fill_colour, (1 + j) * width, (1 + i) * height, width, height);
                draw_rectangle(COLOR_BLACK, (1 + j) * width, (1 + i) * height, width, height);
            }
        }

        break;
    }
    }
}

void draw_moves_used(std::string moves_used)
{
    draw_text(moves_used, COLOR_WHITE, FONT, FONT_SIZE, 0, 0);
}

void draw_menu()
{
    draw_text("1. Choose 2x2 Rubicks Cube", COLOR_WHITE, FONT, 15, 10, SCREEN_HEIGHT - 110);
    draw_text("2. Choose 3x3 Rubicks Cube", COLOR_WHITE, FONT, 15, 10, SCREEN_HEIGHT - 90);
    draw_text("3. Scramble Cube", COLOR_WHITE, FONT, 15, 10, SCREEN_HEIGHT - 70);
    draw_text("4. Solve with Korf's Algorithm", COLOR_WHITE, FONT, 15, 10, SCREEN_HEIGHT - 50);
    draw_text("5. Solve with Kociemba's Algorithm", COLOR_WHITE, FONT, 15, 10, SCREEN_HEIGHT - 30);
}

void process_inputs(cube_data &data, const pattern_dbs &db, std::string &moves_used, timer &t)
{
    if (key_typed(NUM_1_KEY))
    {
        data.current = CUBE_TYPE_2X2;
        data.r2 = rubicks_2x2();
    }

    if (key_typed(NUM_2_KEY))
    {
        data.current = CUBE_TYPE_3X3;
        data.r3 = rubicks_3x3();
    }

    if (key_typed(NUM_3_KEY))
    {
        std::string scramble;
        scramble = generate_scramble();
        if (data.current == CUBE_TYPE_2X2)
            data.r2.input_scramble(scramble);
        else
            data.r3.input_scramble(scramble);
    }

    if (key_typed(NUM_4_KEY))
    {
        if (data.current == CUBE_TYPE_2X2)
            moves_used = korfs_algorithm(data.r2, db);
        else
            moves_used = korfs_algorithm(data.r3, db);
        start_timer(t);
    }

    if (key_typed(NUM_5_KEY))
    {
        if (data.current == CUBE_TYPE_2X2)
            std::cout << "Sorry, but Kociemba's algorithm only works on 3x3 Rubicks Cubes\n";
        else
        {
            moves_used = kociembas_algorithm(data.r3, db);
            start_timer(t);
        }
    }

    if (key_typed(R_KEY))
    {
        if (key_down(LEFT_SHIFT_KEY))
        {
            if (data.current == CUBE_TYPE_2X2)
                data.r2.input_move(R_PRIME);
            else
                data.r3.input_move(R_PRIME);
        }
        else if (key_down(LEFT_CTRL_KEY))
        {
            if (data.current == CUBE_TYPE_2X2)
                data.r2.input_move(R2);
            else
                data.r3.input_move(R2);
        }
        else
        {
            if (data.current == CUBE_TYPE_2X2)
                data.r2.input_move(R);
            else
                data.r3.input_move(R);
        }
    }

    if (key_typed(L_KEY))
    {
        if (key_down(LEFT_SHIFT_KEY))
        {
            if (data.current == CUBE_TYPE_2X2)
                data.r2.input_move(L_PRIME);
            else
                data.r3.input_move(L_PRIME);
        }
        else if (key_down(LEFT_CTRL_KEY))
        {
            if (data.current == CUBE_TYPE_2X2)
                data.r2.input_move(L2);
            else
                data.r3.input_move(L2);
        }
        else
        {
            if (data.current == CUBE_TYPE_2X2)
                data.r2.input_move(L);
            else
                data.r3.input_move(L);
        }
    }

    if (key_typed(F_KEY))
    {
        if (key_down(LEFT_SHIFT_KEY))
        {
            if (data.current == CUBE_TYPE_2X2)
                data.r2.input_move(F_PRIME);
            else
                data.r3.input_move(F_PRIME);
        }
        else if (key_down(LEFT_CTRL_KEY))
        {
            if (data.current == CUBE_TYPE_2X2)
                data.r2.input_move(F2);
            else
                data.r3.input_move(F2);
        }
        else
        {
            if (data.current == CUBE_TYPE_2X2)
                data.r2.input_move(F);
            else
                data.r3.input_move(F);
        }
    }

    if (key_typed(U_KEY))
    {
        if (key_down(LEFT_SHIFT_KEY))
        {
            if (data.current == CUBE_TYPE_2X2)
                data.r2.input_move(U_PRIME);
            else
                data.r3.input_move(U_PRIME);
        }
        else if (key_down(LEFT_CTRL_KEY))
        {
            if (data.current == CUBE_TYPE_2X2)
                data.r2.input_move(U2);
            else
                data.r3.input_move(U2);
        }
        else
        {
            if (data.current == CUBE_TYPE_2X2)
                data.r2.input_move(U);
            else
                data.r3.input_move(U);
        }
    }

    if (key_typed(B_KEY))
    {
        if (key_down(LEFT_SHIFT_KEY))
        {
            if (data.current == CUBE_TYPE_2X2)
                data.r2.input_move(B_PRIME);
            else
                data.r3.input_move(B_PRIME);
        }
        else if (key_down(LEFT_CTRL_KEY))
        {
            if (data.current == CUBE_TYPE_2X2)
                data.r2.input_move(B2);
            else
                data.r3.input_move(B2);
        }
        else
        {
            if (data.current == CUBE_TYPE_2X2)
                data.r2.input_move(B);
            else
                data.r3.input_move(B);
        }
    }

    if (key_typed(D_KEY))
    {
        if (key_down(LEFT_SHIFT_KEY))
        {
            if (data.current == CUBE_TYPE_2X2)
                data.r2.input_move(D_PRIME);
            else
                data.r3.input_move(D_PRIME);
        }
        else if (key_down(LEFT_CTRL_KEY))
        {
            if (data.current == CUBE_TYPE_2X2)
                data.r2.input_move(D2);
            else
                data.r3.input_move(D2);
        }
        else
        {
            if (data.current == CUBE_TYPE_2X2)
                data.r2.input_move(D);
            else
                data.r3.input_move(D);
        }
    }
}

int main()
{
    pattern_dbs db;

    std::string moves_used = "";
    timer display_moves_used_timer = create_timer("moves used time");

    open_data_files(db);

    cube_data data;

    open_window("Rubicks Cube Visualiser", SCREEN_WIDTH, SCREEN_HEIGHT);

    while (!quit_requested() && !key_typed(ESCAPE_KEY))
    {
        process_events();

        visualise_cube(data);
        draw_menu();

        if (timer_ticks(display_moves_used_timer) < DISPLAY_MOVES_ACTIVE_TIME &&
            timer_started(display_moves_used_timer))
        {
            draw_moves_used(moves_used);
        }
        else
        {
            stop_timer(display_moves_used_timer);
        }

        process_inputs(data, db, moves_used, display_moves_used_timer);

        refresh_screen(REFRESH_RATE);
    }

    close_data_files(db);

    return 0;
}
