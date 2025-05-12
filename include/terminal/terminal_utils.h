#ifndef TERMINAL_UTILS_H
#define TERMINAL_UTILS_H

// Cannonical mode allows to read the input after the Enter button is pressed
#include "board/board.h"
#include "terminal/arrow_keys.h"
#include "terminal/board_sizes.h"
#include <string_view>
#include <string>
#include <sys/ioctl.h>

void disable_canon_mode();
void enable_canon_mode();

// Toggle showing typed characters
void disable_echo();
void enable_echo();

void clear_input();
void clear_terminal(); // clears everything on the screen

// Next 2 methods read the input searching for specific key
// If they find unexpected char, they put all taken chars back in the input stream
ArrowKey read_arrow_key();
bool ignore_esc_sequence(); // returns true if sequence was found and cleared

bool fits_on_screen(BoardSize size, Board *p_board, const winsize &win_size);

// outputs a full border line (whole row)
// takes characters in params to output them on specified places 
void print_border_line(
    int cols,
    int cell_width,
    std::string_view main_char, // fills main space (e.g. ═══)
    std::string_view first_char, // char at the beginning of the line
    std::string_view split_char, // char at the place where the borders may somehow cross
    std::string_view last_char // char at the end of the line
);

void display_finish_button(int board_width, bool is_active = false);
std::string focuse_option(bool is_focused, std::string str);
void draw_menu(int focused_opt);

#endif

