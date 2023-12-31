#include "game_setup.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Some handy dandy macros for decompression
#define E_CAP_HEX 0x45
#define E_LOW_HEX 0x65
#define S_CAP_HEX 0x53
#define S_LOW_HEX 0x73
#define W_CAP_HEX 0x57
#define W_LOW_HEX 0x77
#define DIGIT_START 0x30
#define DIGIT_END 0x39

/** Initializes the board with walls around the edge of the board.
 *
 * Modifies values pointed to by cells_p, width_p, and height_p and initializes
 * cells array to reflect this default board.
 *
 * Returns INIT_SUCCESS to indicate that it was successful.
 *
 * Arguments:
 *  - cells_p: a pointer to a memory location where a pointer to the first
 *             element in a newly initialized array of cells should be stored.
 *  - width_p: a pointer to a memory location where the newly initialized
 *             width should be stored.
 *  - height_p: a pointer to a memory location where the newly initialized
 *              height should be stored.
 */
enum board_init_status initialize_default_board(int** cells_p, size_t* width_p,
                                                size_t* height_p) {
    *width_p = 20;
    *height_p = 10;
    int* cells = malloc(20 * 10 * sizeof(int));
    *cells_p = cells;
    for (int i = 0; i < 20 * 10; i++) {
        cells[i] = FLAG_PLAIN_CELL;
    }

    // Set edge cells!
    // Top and bottom edges:
    for (int i = 0; i < 20; ++i) {
        cells[i] = FLAG_WALL;
        cells[i + (20 * (10 - 1))] = FLAG_WALL;
    }
    // Left and right edges:
    for (int i = 0; i < 10; ++i) {
        cells[i * 20] = FLAG_WALL;
        cells[i * 20 + 20 - 1] = FLAG_WALL;
    }

    // Add snake
    cells[20 * 2 + 2] = FLAG_SNAKE;

    return INIT_SUCCESS;
}

/** Initialize variables relevant to the game board.
 * Arguments:
 *  - cells_p: a pointer to a memory location where a pointer to the first
 *             element in a newly initialized array of cells should be stored.
 *  - width_p: a pointer to a memory location where the newly initialized
 *             width should be stored.
 *  - height_p: a pointer to a memory location where the newly initialized
 *              height should be stored.
 *  - snake_p: a pointer to your snake struct (not used until part 2!)
 *  - board_rep: a string representing the initial board. May be NULL for
 * default board.
 */
enum board_init_status initialize_game(int** cells_p, size_t* width_p,
                                       size_t* height_p, snake_t* snake_p,
                                       char* board_rep) {
    // Call initialize_default_board to initialize cells_p, width_p, and height_p
    enum board_init_status init_status;
    // Check if board_rep is provided for custom initialization
    if (board_rep != NULL) {
        // Call decompress_board_str to initialize cells_p, width_p, and height_p
        init_status = decompress_board_str(cells_p, width_p, height_p, snake_p, board_rep);
    }
    else {
        init_status = initialize_default_board(cells_p, width_p, height_p);
    }
    //init_status = initialize_default_board(cells_p, width_p, height_p);
    // Initialize global variables
    g_game_over = 0;  // Game is not over initially
    g_score = 0;      // Initial score is 0

    // Place food on the board
    place_food(*cells_p, *width_p, *height_p);

    // Initialize snake data (part 2)
    // For now, you can set some default values or leave it empty until part 2
    // snake_p->length = 1; // Set an initial length for the snake
    // snake_p->positionX = *width_p / 2; // Place the snake in the middle of the board
    // snake_p->positionY = *height_p / 2;
    // snake_p->direction = INPUT_RIGHT; // Set an initial direction for the snake

    // Return the status from initialize_default_board
    return init_status;
}

/** Takes in a string `compressed` and initializes values pointed to by
 * cells_p, width_p, and height_p accordingly. Arguments:
 *      - cells_p: a pointer to the pointer representing the cells array
 *                 that we would like to initialize.
 *      - width_p: a pointer to the width variable we'd like to initialize.
 *      - height_p: a pointer to the height variable we'd like to initialize.
 *      - snake_p: a pointer to your snake struct (not used until part 2!)
 *      - compressed: a string that contains the representation of the board.
 * Note: We assume that the string will be of the following form:
 * B24x80|E5W2E73|E5W2S1E72... To read it, we scan the string row-by-row
 * (delineated by the `|` character), and read out a letter (E, S or W) a number
 * of times dictated by the number that follows the letter.
 */
size_t get_index(size_t row, size_t col, size_t width) {
    return row * width + col;
}
enum board_init_status decompress_board_str(int** cells_p, size_t* width_p, size_t* height_p, snake_t* snake_p, char* compressed) {

    int height, width;
    char* saveptr;
    char* dimension_str = strtok_r(compressed, "|", &saveptr);
    printf("Saveptr: %s\nDimension_str:%s\n", saveptr, dimension_str);

    sscanf(dimension_str, "B%dx%d", &height, &width);
    printf("Height: %d\nWidth: %d\n", height, width);

    *height_p = height;
    *width_p = width;

    int* cells = malloc(width * height * sizeof(int));
    *cells_p = cells;

    for (int i = 0; i < width * height; i++) {
        cells[i] = FLAG_PLAIN_CELL;
    }

    int current_index = 0;
    int total_count = 0;
    int snake_count = 0;
    int cell_type_str_count = 0;
    while (*saveptr != '\0') {
        char* cell_type_str = strtok_r(NULL, "|", &saveptr);
        char cell_type;
        int count;

        cell_type_str_count++;

        // Error handling for test case 22
        if (cell_type_str_count > height) {
            return INIT_ERR_INCORRECT_DIMENSIONS;
        }

        for (int i = 0; cell_type_str[i] != '\0';) {
            // Use sscanf to extract cell_type and count
            if (sscanf(cell_type_str + i, "%c%d", &cell_type, &count) == 2) {
            // Process the extracted values (in this example, just print them)
                //printf("Cell Type: %c, Count: %d\n", cell_type, count);
                i += snprintf(NULL, 0, "%c%d", cell_type, count);
            }

            total_count = count + total_count;
            //printf("Total_count: %d\n", total_count);

            // Update cells based on cell_type and count
            for (int j = 0; j < count; j++) {
                // Assuming W represents wall, S represents snake, and E represents empty
                int flag;
                if (cell_type == 'W') {
                    flag = FLAG_WALL;
                } else if (cell_type == 'S') {
                    snake_count++;
                    flag = FLAG_SNAKE;
                } else if (cell_type == 'E') {
                    flag = FLAG_PLAIN_CELL;
                } else {
                    // Error handling for test case 23
                    return INIT_ERR_BAD_CHAR;
                }

                // Extract row and col based on the current index
                size_t row = current_index / width;
                size_t col = current_index % width;

                // Calculate the index using the helper function
                size_t index = get_index(row, col, width);

                // Update the cell in the array
                cells[index] = flag;
                current_index++;
            }
        }

        // Error handling for test case 19, 20
        if (total_count != width) {
            return INIT_ERR_INCORRECT_DIMENSIONS;
        }

        total_count = 0;

        //printf("cell_type_str_count: %d\n", cell_type_str_count);

    }

    // Error handling for test case 21
    if (cell_type_str_count != height) {
         return INIT_ERR_INCORRECT_DIMENSIONS;
    }

    // Error handling for test case 24, 25, 26
    if (snake_count != 1) {
        return INIT_ERR_WRONG_SNAKE_NUM;
    }

    return INIT_SUCCESS;  // Board decompression was successful
}