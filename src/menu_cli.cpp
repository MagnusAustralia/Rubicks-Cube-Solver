#include "../include/depth_first_search.h"
#include "../include/generate_data.h"
#include "../include/generate_scramble.h"
#include "../include/kociembas_algorithm.h"
#include "../include/korfs_algorithm.h"
#include "../include/rubicks_2x2.h"
#include "../include/rubicks_3x3.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>

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
        current = CUBE_TYPE_3X3;
        r3 = rubicks_3x3();
    };
};

/**
 * The list of enum option in main menu.
 *
 * @option CREATE_NEW_CUBE_MENU The option to choose the cube type.
 * @option GENERATE_RANDOM_SCRAMBLE_MENU The option to generate a random scramble for the cube.
 * @option GENERATE_SET_LENGTH_SCRAMBLE_MENU The option to generate a scramble of a set length for the cube.
 * @option INPUT_SCRAMBLE_MENU The option input a scamble into the cube.
 * @option PRINT_CUBE_STATE_MENU The option to print the current cube state to the console.
 * @option RUN_KORFS_ALGORITHM_MENU The option run korf's algorithm on the cube.
 * @option RUN_KOCIEMBAS_ALGORITHM_MENU The option run kociemba's algorithm on the cube.
 * @option GENERATE_DATA_MENU The option to select a pattern database to generate data for.
 * @option EXIT_MAIN_MENU The option quit the main menu.
 */
enum main_menu_option
{
    CREATE_NEW_CUBE_MENU,
    GENERATE_RANDOM_SCRAMBLE_MENU,
    GENERATE_SET_LENGTH_SCRAMBLE_MENU,
    INPUT_SCRAMBLE_MENU,
    PRINT_CUBE_STATE_MENU,
    RUN_KORFS_ALGORITHM_MENU,
    RUN_KOCIEMBAS_ALGORITHM_MENU,
    GENERATE_DATA_MENU,
    EXIT_MAIN_MENU
};

/**
 * Lets the user select if they want to work with a 2x2 Rubicks Cube or a 3x3 Rubicks Cube
 *
 * @param cube The cube to update
 */
void handle_new_cube(cube_data &cube)
{
    std::string line;
    int choice = -1;
    while (choice < 1 || choice > 2)
    {
        std::cout << "Enter your choice for the new cube (1 for a 2x2; 2 for a 3x3): ";
        std::getline(std::cin, line);

        if (line == "1" || line == "2")
        {
            choice = std::stoi(line);
        }
        else
        {
            std::cout << "\nEnsure that you are input a number between 1-2\n\n";
        }
    }

    cube.current = (choice == 1) ? CUBE_TYPE_2X2 : CUBE_TYPE_3X3;

    cube.r2 = rubicks_2x2();
    cube.r3 = rubicks_3x3();

    std::cout << ((cube.current == CUBE_TYPE_2X2) ? "\nYou are now interacting with a new 2x2 Rubicks Cube.\n\n"
                                                  : "\nYou are now interacting with a new 3x3 Rubicks Cube.\n\n");
}

/**
 * Generates a random scramble for the Rubicks cube
 *
 * @param cube The rubicks cube to scramble
 */
void handle_generate_random_scramble(cube_data &cube)
{
    std::string scramble = generate_scramble();

    switch (cube.current)
    {
    case CUBE_TYPE_2X2:
        cube.r2.input_scramble(scramble);
        break;

    case CUBE_TYPE_3X3:
        cube.r3.input_scramble(scramble);
        break;
    }

    std::cout << "\nCube scrambled!\n\n";
}

/**
 * Generates a set length scramble for the Rubicks cube
 *
 * @param cube The rubicks cube to scramble
 */
void handle_generate_set_length_scramble(cube_data &cube)
{
    std::string line;
    int length = 0;
    do
    {
        std::cout << "How long do you want the scamble to be: ";
        std::getline(std::cin, line);

        if (line.length() > 0)
        {
            length = std::stoi(line);

            if (length <= 0)
            {
                length = 0;
                std::cout << "\nThe scramble must have a positive length more than 0\n";
            }
        }
        else
        {
            std::cout << "\nYou must enter an integer\n";
        }

    } while (length == 0);

    std::string scramble = generate_scramble(length);

    std::cout << "Scramble Generated: " << scramble << "\n";

    switch (cube.current)
    {
    case CUBE_TYPE_2X2:
        cube.r2.input_scramble(scramble);
        break;

    case CUBE_TYPE_3X3:
        cube.r3.input_scramble(scramble);
        break;
    }

    std::cout << "\n";
}

/**
 * Lets the user an input a scramble which is then passed into the current cube type
 *
 * @param cube The cube to input the scramble into
 */
void handle_input_scramble(cube_data &cube)
{
    std::string scramble;
    std::cout << "Enter your scramble: ";
    std::getline(std::cin, scramble);

    switch (cube.current)
    {
    case CUBE_TYPE_2X2:
        cube.r2.input_scramble(scramble);
        break;

    case CUBE_TYPE_3X3:
        cube.r3.input_scramble(scramble);
        break;
    }

    std::cout << "\n";
}

/**
 * Prints to the console a display of the current cube state
 *
 * @param cube The cube to print
 */
void handle_print_cube(cube_data &cube)
{
    std::cout << "The current cube state is: \n";

    switch (cube.current)
    {
    case CUBE_TYPE_2X2:
        cube.r2.print_cube();
        break;

    case CUBE_TYPE_3X3:
        cube.r3.print_cube();
        break;
    }

    std::cout << "\n";
}

/**
 * Runs Korf's Algorithm on the passed in cube to solve it.
 *
 * @param cube The store of the cube_data to be worked with
 * @param db The pattern databases to be used in Korf's Algorithm
 */
void handle_run_korfs_algorithm(cube_data &cube, const pattern_dbs &db)
{
    switch (cube.current)
    {
    case CUBE_TYPE_2X2:
        korfs_algorithm(cube.r2, db);
        break;
    case CUBE_TYPE_3X3:
        korfs_algorithm(cube.r3, db);
        break;
    }
}

/**
 * Evaluates the current cube type being used, and then runs Kociemba's Algorithm if it is a 3x3 Rubicks Cube
 *
 * @param cube The store for the cube_data to be worked with
 * @param db The pattern databases to be used in Kociemba's Algorithm
 */
void handle_run_kociembas_algorithm(cube_data &cube, const pattern_dbs &db)
{
    switch (cube.current)
    {
    case CUBE_TYPE_2X2:
        std::cout << "Sorry, Kociemba's algorithm cannot be run on a 2x2 Rubicks Cube\n";
        break;

    case CUBE_TYPE_3X3:
        kociembas_algorithm(cube.r3, db);
        break;
    }

    std::cout << "\n";
}

/**
 * Shows a menu for data generation options, and after a valid input, generates the specified pattern data base.
 *
 * @param db The loaded pattern databases, to ensure that they are reloaded after the generation is done.
 */
void handle_generate_data(pattern_dbs &db)
{
    close_data_files(db);
    std::string menu_input;
    pdb_type pdb_option;
    bool input_valid = false;

    while (!input_valid)
    {
        std::cout << "1. Generate Corners pattern database\n";
        std::cout << "2. Generate Edges-Case-1 pattern database\n";
        std::cout << "3. Generate Edges-Case-2 pattern database\n";
        std::cout << "4. Generate Corners-Slice pattern database\n";
        std::cout << "5. Generate Edges-Slice pattern database\n";
        std::cout << "6. Generate Corners-Slices-Perm pattern database\n";
        std::cout << "7. Generate Edges-Slices-Perm pattern database\n";
        std::cout << "8. Quit\n\n";

        std::cout << "Enter your choice (a number between 1-8): ";
        std::getline(std::cin, menu_input);

        if (menu_input == "8")
        {
            std::cout << "\n";
            return;
        }

        if (menu_input == "1" || menu_input == "2" || menu_input == "3" || menu_input == "4" || menu_input == "5" ||
            menu_input == "6" || menu_input == "7")
        {
            pdb_option = static_cast<pdb_type>(std::stoi(menu_input) - 1);
            switch (pdb_option)
            {
            case CORNERS_PDB:
                std::cout << "\nGenerating corners database: \n\n";
                break;
            case EDGES_CASE_1_PDB:
                std::cout << "\nGenerating edges-case-1 database: \n\n";
                break;
            case EDGES_CASE_2_PDB:
                std::cout << "\nGenerating edges-case-2 database: \n\n";
                break;
            case CORNER_SLICE_PDB:
                std::cout << "\nGenerating corners-slice database: \n\n";
                break;
            case EDGE_SLICE_PDB:
                std::cout << "\nGenerating edges-slice database: \n\n";
                break;
            case CORNER_SLICE_PERM_PDB:
                std::cout << "\nGenerating corners-slice-perm database: \n\n";
                break;
            case EDGE_SLICE_PERM_PDB:
                std::cout << "\nGenerating edges-slice-perm database: \n\n";
                break;
            }

            input_valid = true;
            generate_db(pdb_option);
        }
        else
        {
            std::cout << "\nEnsure that you are input a number between 1-8\n\n";
        }
    }

    open_data_files(db);
}

/**
 * Show the main menu and get the chosen option from the user.
 *
 * @return The chosen option.
 */
main_menu_option read_main_menu()
{
    std::string menu_input;
    int menu_option = 0;

    while (menu_option < 1 || menu_option > 9)
    {
        std::cout << "1. Create a new cube\n";
        std::cout << "2. Generate a random scamble\n";
        std::cout << "3. Generate a set-length scamble\n";
        std::cout << "4. Input a scamble\n";
        std::cout << "5. Print cube\n";
        std::cout << "6. Run Korf's Algorithm\n";
        std::cout << "7. Run Kociemba's Algorithm\n";
        std::cout << "8. Generate Data\n";
        std::cout << "9. Quit\n\n";

        std::cout << "Enter your choice (a number between 1-9): ";
        std::getline(std::cin, menu_input);

        if (menu_input == "1" || menu_input == "2" || menu_input == "3" || menu_input == "4" || menu_input == "5" ||
            menu_input == "6" || menu_input == "7" || menu_input == "8" || menu_input == "9")
        {
            menu_option = std::stoi(menu_input);
        }
        else
        {
            std::cout << "\nEnsure that you are input a number between 1-9\n\n";
        }
    }

    return (main_menu_option)(menu_option - 1);
}

int main()
{
    // Required to ensure that random scrambles are generated with std::rand()
    srand(time(0));

    cube_data cube;

    pattern_dbs db;
    main_menu_option choice;

    open_data_files(db);

    do
    {
        choice = read_main_menu();

        switch (choice)
        {
        case CREATE_NEW_CUBE_MENU:
            handle_new_cube(cube);
            break;
        case GENERATE_RANDOM_SCRAMBLE_MENU:
            handle_generate_random_scramble(cube);
            break;
        case GENERATE_SET_LENGTH_SCRAMBLE_MENU:
            handle_generate_set_length_scramble(cube);
            break;
        case INPUT_SCRAMBLE_MENU:
            handle_input_scramble(cube);
            break;
        case PRINT_CUBE_STATE_MENU:
            handle_print_cube(cube);
            break;
        case RUN_KORFS_ALGORITHM_MENU:
            handle_run_korfs_algorithm(cube, db);
            break;
        case RUN_KOCIEMBAS_ALGORITHM_MENU:
            handle_run_kociembas_algorithm(cube, db);
            break;
        case GENERATE_DATA_MENU:
            handle_generate_data(db);
            break;
        case EXIT_MAIN_MENU:
            break;
        }
    } while (choice != EXIT_MAIN_MENU);

    close_data_files(db);

    return 0;
}