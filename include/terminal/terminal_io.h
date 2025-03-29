#ifndef TERMINAL_IO_H
#define TERMINAL_IO_H

#include "board/board.h"
#include "terminal/arrow_keys.h"

class Terminal {
    void clear_input();
    void clear_terminal();
    ArrowKey read_arrow_key();
    void cursor_to_cell(Board &board, int cell_width, int cell_height, int row, int col);

public:
    Board read_board();
    void render_board(Board &board);
    // void fill_fixed_cells(Board &board);
};

#endif
