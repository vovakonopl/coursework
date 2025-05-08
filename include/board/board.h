#ifndef BOARD_H
#define BOARD_H

#include "board/coordinate.h"
#include "board/region.h"
#include "cell.h"
#include <vector>

class Board {
    int rows;
    int cols;
    int region_count;
    Cell **board;

public:
    std::vector<int> values_on_board;
    std::vector<Coord> fixed_cell_coords;
    std::vector<Coord> result; // in which order cells were filled

    Board(int rows, int cols);
    ~Board();

    int get_rows();
    int get_cols();

    Cell &cell_at(int row, int col);
    Cell &cell_at(Coord coord);
    void create_cell(int row, int col, int value, bool is_fixed = false);
    
    Region create_region(int target_size); 
    void create_fixed_cells_list();
    void fill_values_on_board();
};

#endif
