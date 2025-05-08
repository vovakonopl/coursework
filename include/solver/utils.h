#ifndef UTILS_H
#define UTILS_H

#include "board/coordinate.h"
#include "board/board.h"
#include <vector>

enum class Direction {
    Up,
    Right,
    Down,
    Left,
}; 

bool is_coord_valid(Board &board, Coord coord);
bool can_be_added_to_region(Board &board, Coord coord, Region *p_region);

Coord get_free_adj_cell(Board &board, Cell &cell, Direction dir);
bool has_any_free_adj_cell(Board &board, Cell &cell);
int get_next_unfilled_fixed_cell_idx(Board &board); // -1 = all fixed cells were filled
void get_adjs_with_same_val(Board &board, Cell &cell, std::vector<Coord> &vec);

void undo_cell(Board &board, Region *p_region, Cell &cell); // if region contains cell, it resets it

int find_idx(std::vector<int> vect, int value);

// fills empty board or board with valid single "1"  cells if board sizes >= 2x2 
void fill_empty_board(Board &board);

#endif
