#ifndef TERMINAL_IO_H
#define TERMINAL_IO_H

#include "board/board.h"
#include "terminal/arrow_keys.h"
#include <string_view>

class Terminal {
    // Cannonical mode allows to read the input after the Enter button is pressed
    // We need to disable it to listen every arrow key press
    void disable_canon_mode();
    void enable_canon_mode();
   
    void clear_input();
    void clear_terminal();
    ArrowKey read_arrow_key();
    void cursor_to_cell(Board &board, int cell_width, int cell_height, int row, int col);
    void paint_cell(Board &board, std::string_view bg_color, int row, int col, int width, int height);

public:
    Board read_board();
    void render_board(Board &board);
    void fill_fixed_cells(Board &board);
};

#endif
