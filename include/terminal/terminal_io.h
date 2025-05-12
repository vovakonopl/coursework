#ifndef TERMINAL_IO_H
#define TERMINAL_IO_H

#include "board/board.h"
#include "solver/solve_mode.h"
#include <string_view>

class Terminal {
    Board *p_board;
    int cell_width;
    int cell_height;
    int board_width;
    int board_height;

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
    void render_board(bool with_interval = false);
    void fill_fixed_cells(const char *msg = nullptr); // msg will be outputed under the board
    void ask_board_sizes(int &rows, int &cols);
    SolveMode select_mode_menu();
};

#endif
