#ifndef BOARD_H
#define BOARD_H

#include "cell.h"

class Board {
    int rows;
    int cols;
    Cell **board;

public:
    Board(int rows, int cols);
    ~Board();

    Cell &cell_at(int x, int y);
    int get_rows();
    int get_cols();
};

#endif
