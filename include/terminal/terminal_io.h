#ifndef TERMINAL_IO_H
#define TERMINAL_IO_H

#include "board/board.h"
#include "solver/solve_mode.h"
#include "terminal/arrow_keys.h"
#include <string_view>

class Terminal {
    Board *p_board;
    int cell_width;
    int cell_height;
    int board_width;
    int board_height;

    // Cannonical mode allows to read the input after the Enter button is pressed
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

    void cursor_to_cell_start(int row, int col);
    void draw_cell(std::string_view bg_color, int row, int col);
    void focus_cell(int row, int col);

    // Resizes cells according to their number so that the board fits on the screen
    // Returns true if cell size has been changed
    bool resize_cells();

    int get_size(); // returns valid board size (rows/cols)

public:
    Terminal();

    // sets the pointer for actual board
    void set_board(Board *p_board);
 
    Board *read_board();
    void render_board();
    void fill_fixed_cells();
    void ask_board_sizes(int &rows, int &cols);
    SolveMode select_mode_menu();
};

#endif
