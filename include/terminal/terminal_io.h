#ifndef TERMINAL_IO_H
#define TERMINAL_IO_H

#include "board/board.h"
#include "terminal/arrow_keys.h"
#include <string_view>

class Terminal {
    Board *p_board;
    int cell_width;
    int cell_height;

    // Cannonical mode allows to read the input after the Enter button is pressed
    // We need to disable it to listen every arrow key press
    void disable_canon_mode();
    void enable_canon_mode();
   
    void clear_input();
    void clear_terminal();
    ArrowKey read_arrow_key();
    void cursor_to_cell(int row, int col);
    void paint_cell(std::string_view bg_color, int row, int col);

public:
    Terminal();

    void set_board(Board *p_board);
    Board *read_board();
    void render_board();
    void fill_fixed_cells();
};

#endif
