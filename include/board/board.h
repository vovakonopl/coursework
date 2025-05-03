#ifndef BOARD_H
#define BOARD_H

#include "board/coordinate.h"
#include "board/region.h"
#include "cell.h"
#include <vector>

class Board {
    int rows;
    int cols;
    Cell **board;
    std::vector<Region> regions;

public:
    std::vector<Coord> fixed_cell_coords;

    Board(int rows, int cols);
    ~Board();

    int get_rows();
    int get_cols();

    Cell &cell_at(int row, int col);
    Cell &cell_at(Coord coord);
    void create_cell(int row, int col, int value, bool is_fixed = false);

    int create_region(int target_size);
    void pop_region(); // remove last
    Region &region_at(long unsigned int idx);
};

#endif
