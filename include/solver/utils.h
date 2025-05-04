#ifndef UTILS_H
#define UTILS_H

#include "board/coordinate.h"
#include "board/board.h"

enum class Direction {
    Up,
    Right,
    Down,
    Left,
}; 

bool is_coord_valid(Board &board, Coord coord);

Coord get_free_adj_cell(Board &board, Cell &cell, Direction dir);
bool has_any_free_adj_cell(Board &board, Cell &cell);

int get_next_unfilled_fixed_cell_idx(Board &board); // -1 = all fixed cells were filled

// Returns true if everything filled corectly and false if not.
// False may be the reason of uncorrect single "1" cell placement.
bool fill_remaining_cells(Board &board);

// Validate for adjacency with other regions or fixed cells after finishing region.
// There mast be no adjacent unfilled fixed cells or same adjacent regions.
bool is_region_valid(Board &board, Region &region);

#endif
