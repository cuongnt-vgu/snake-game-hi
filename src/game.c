#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "linked_list.h"
#include "mbstrings.h"

enum input_key last_known_direction = INPUT_RIGHT;

/** Updates the game by a single step, and modifies the game information
 * accordingly. Arguments:
 *  - cells: a pointer to the first integer in an array of integers representing
 *    each board cell.
 *  - width: width of the board.
 *  - height: height of the board.
 *  - snake_p: pointer to your snake struct (not used until part 2!)
 *  - input: the next input.
 *  - growing: 0 if the snake does not grow on eating, 1 if it does.
 */
void update(int* cells, size_t width, size_t height, snake_t* snake_p,
            enum input_key input, int growing) {
    // `update` should update the board, your snake's data, and global
    // variables representing game information to reflect new state. If in the
    // updated position, the snake runs into a wall or itself, it will not move
    // and global variable g_game_over will be 1. Otherwise, it will be moved
    // to the new position. If the snake eats food, the game score (`g_score`)
    // increases by 1. This function assumes that the board is surrounded by
    // walls, so it does not handle the case where a snake runs off the board.

    // TODO: implement!
    if (g_game_over) {
        return;
    }
    // Find the head of the snake
    int head_index = -1;
    for (size_t i = 0; i < width * height; ++i) {
        if (cells[i] == FLAG_SNAKE) {
            head_index = i;
            break;
        }
    }

    if (head_index == -1) {
        // Snake not found on the board, unexpected state
        g_game_over = 1;
        return;
    }

    // Determine the new head position based on the input (shift to the right)
    //size_t new_head_index = head_index + 1;

    size_t new_head_index;
    switch (input) {
        case INPUT_UP:
            new_head_index = head_index - width;
            break;
        case INPUT_DOWN:
            new_head_index = head_index + width;
            break;
        case INPUT_LEFT:
            new_head_index = head_index - 1;
            break;
        case INPUT_RIGHT:
            new_head_index = head_index + 1;
            break;
        default:
            // Invalid input, do nothing
            switch (last_known_direction) {
                case INPUT_UP:
                    new_head_index = head_index - width;
                    break;
                case INPUT_DOWN:
                    new_head_index = head_index + width;
                    break;
                case INPUT_LEFT:
                    new_head_index = head_index - 1;
                    break;
                case INPUT_RIGHT:
                    new_head_index = head_index + 1;
                    break;
                default:
                    // Invalid direction, do not update the snake's position
                    return;
            }
    }

    // Check if the new head position is valid
    if ((int)new_head_index < 0 || new_head_index >= width * height) {
        // Snake hit the board boundary
        g_game_over = 1;
        return;
    }

    if (cells[new_head_index] == FLAG_WALL) {
        // Snake collided with a wall
        g_game_over = 1;
        return;
    }

    if (cells[new_head_index] == FLAG_FOOD) {
        g_score += 1;

        // Move the snake to the new position
        cells[new_head_index] = FLAG_SNAKE;

        // If not growing, remove the tail
        if (!growing) {
            int tail_index = -1;
            for (size_t i = 0; i < width * height; ++i) {
                if (cells[i] == FLAG_SNAKE && i != new_head_index) {
                    tail_index = i;
                    break;
                }
            }

            if (tail_index != -1) {
                cells[tail_index] = FLAG_PLAIN_CELL;
            }
        }
        // Place new food on the board
        place_food(cells, width, height);
    } else {
        // Move the snake to the new position
        cells[new_head_index] = FLAG_SNAKE;

        // If not growing, remove the tail
        if (!growing) {
            int tail_index = -1;
            for (size_t i = 0; i < width * height; ++i) {
                if (cells[i] == FLAG_SNAKE && i != new_head_index) {
                    tail_index = i;
                    break;
                }
            }

            if (tail_index != -1) {
                cells[tail_index] = FLAG_PLAIN_CELL;
            }
        }
    }
    // Update last_known_direction only when a new input is provided
    if (input != INPUT_NONE) {
        last_known_direction = input;
    }
}

/** Sets a random space on the given board to food.
 * Arguments:
 *  - cells: a pointer to the first integer in an array of integers representing
 *    each board cell.
 *  - width: the width of the board
 *  - height: the height of the board
 */
void place_food(int* cells, size_t width, size_t height) {
    /* DO NOT MODIFY THIS FUNCTION */
    unsigned food_index = generate_index(width * height);
    if (*(cells + food_index) == FLAG_PLAIN_CELL) {
        *(cells + food_index) = FLAG_FOOD;
    } else {
        place_food(cells, width, height);
    }
    /* DO NOT MODIFY THIS FUNCTION */
}

/** Prompts the user for their name and saves it in the given buffer.
 * Arguments:
 *  - `write_into`: a pointer to the buffer to be written into.
 */
void read_name(char* write_into) {
    // TODO: implement! (remove the call to strcpy once you begin your
    // implementation)
    strcpy(write_into, "placeholder");
}

/** Cleans up on game over — should free any allocated memory so that the
 * LeakSanitizer doesn't complain.
 * Arguments:
 *  - cells: a pointer to the first integer in an array of integers representing
 *    each board cell.
 *  - snake_p: a pointer to your snake struct. (not needed until part 2)
 */
void teardown(int* cells, snake_t* snake_p) {
    // TODO: implement!
    free(cells);
}