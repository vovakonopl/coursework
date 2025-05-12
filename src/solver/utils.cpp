#include "solver/utils.h"
#include "solver/solver.h"

bool is_coord_valid(Board &board, Coord coord) {
    return 
        coord.row >= 0 && coord.row < board.get_rows() &&
        coord.col >= 0 && coord.col < board.get_cols();
}

bool can_be_added_to_region(Board &board, Coord coord, Region *p_region) {
    int row = coord.row;
    int col = coord.col;
    Coord adj_coords[4];
    adj_coords[0] = Coord(row - 1, col); // top
    adj_coords[1] = Coord(row + 1, col); // bottom
    adj_coords[2] = Coord(row, col - 1); // left
    adj_coords[3] = Coord(row, col + 1); // right

    for (int i = 0; i < 4; i++) {
        if (!is_coord_valid(board, adj_coords[i])) continue;

        Cell &adj_cell = board.cell_at(adj_coords[i]);
        if(
            adj_cell.region_id != -1 &&
            adj_cell.get_value() == p_region->get_target_size() &&
            adj_cell.region_id != p_region->get_id()
        ) {
            return false;
        }
    }

    return true;
}


Coord get_free_adj_cell(Board &board, Cell &cell, Direction dir) {
    Coord cell_coord = cell.get_coord();
    int row = cell_coord.row;
    int col = cell_coord.col;

    switch (dir) {
        case Direction::Up:
            row -= 1;
            break;

        case Direction::Right:
            col += 1;
            break;
        
        case Direction::Down:
            row += 1;
            break;

        case Direction::Left:
            col -= 1;
            break;
    }

    if (!is_coord_valid(board, Coord(row, col))) {
        return Coord(-1, -1);
    }

    Coord coord = Coord(row, col);
    Cell &adj_cell = board.cell_at(coord);
    // if cell already owned
    if (adj_cell.region_id != -1) {
        return Coord(-1, -1);
    }

    // if unowned fixed cell but it has different value
    if (adj_cell.get_is_fixed() && cell.get_value() != adj_cell.get_value()) {
        return Coord(-1, -1);
    }

    return Coord(row, col);
}

bool has_any_free_adj_cell(Board &board, Cell &cell) {
    // for each direction
    for (int i = 0; i < 4; i++) {
        Direction dir = static_cast<Direction>(i);
        Coord adj_cell_coord = get_free_adj_cell(board, cell, dir);
        if (adj_cell_coord.row == -1) continue;
        
        return true;
    }

    return false;
}

int get_next_unfilled_fixed_cell_idx(Board &board) {
    for (long unsigned int i = 0; i < board.fixed_cell_coords.size(); i++) {
        Coord cell_coord = board.fixed_cell_coords.at(i);
        if (board.cell_at(cell_coord).region_id == -1) {
            return i;
        }
    }

    return -1;
} 

void get_adjs_with_same_val(Board &board, Cell &cell, std::vector<Coord> &vec) {
    int row = cell.get_coord().row;
    int col = cell.get_coord().col;
    Coord adj_coords[4];
    adj_coords[0] = Coord(row - 1, col); // top
    adj_coords[1] = Coord(row + 1, col); // bottom
    adj_coords[2] = Coord(row, col - 1); // left
    adj_coords[3] = Coord(row, col + 1); // right

    for (int i = 0; i < 4; i++) {
        if (!is_coord_valid(board, adj_coords[i])) continue;
        
        Cell &adj_cell = board.cell_at(adj_coords[i]);
        if (adj_cell.region_id == cell.region_id) continue;

        if (adj_cell.get_value() == cell.get_value()) {
            vec.push_back(adj_coords[i]);   
        }
    }
}

// safe undo
void undo_cell(Board &board, Region *p_region, Cell &cell) {
    for (int i = 0; i < p_region->get_size(); i++) { 
        if (cell.get_coord() == p_region->coord_at(i)) {
            cell.region_id = -1;
            p_region->remove_at(i);
            if (!cell.get_is_fixed()) {
                cell.set_value(0);
            }

            return;
        }
    }
}


int find_idx(std::vector<int> vect, int value) {
    for (long unsigned int i = 0; i < vect.size(); i++) {
        if (vect.at(i) == value) {
            return i;
        }
    }

    return -1;
}

void fill_empty_board(Board &board) {
    for (int row = 0; row < board.get_rows(); row++) {
        for (int col = 0; col < board.get_cols(); col++) {
            if (board.cell_at(row, col).get_value() != 1) {
                board.result.push_back(Coord(row, col));
            }
        }
    }

    int filled_cell_num = board.result.size();
    for (int i = 0; i < filled_cell_num; i++) {
        board.cell_at(board.result.at(i)).set_value(filled_cell_num);
    }
}

bool solve_for_each_adjacent(Board &board, Region *p_region, Cell &cell) {
    // for each direction
    for (int i = 0; i < 4; i++) {
        Direction dir = static_cast<Direction>(i);
        Coord next_cell_coord = get_free_adj_cell(board, cell, dir);
        if (next_cell_coord.row == -1) continue;

        // check whether this cell can be added to region
        bool is_valid_adjacency = can_be_added_to_region(board, next_cell_coord, p_region);
        if (!is_valid_adjacency) continue;

        Cell &next_cell = board.cell_at(next_cell_coord);
        next_cell.region_id = p_region->get_id();
        next_cell.set_value(p_region->get_target_size());
        p_region->push(next_cell_coord);

        if (solve(board, p_region, next_cell)) return true; 
        undo_cell(board, p_region, next_cell);
    }

    return false;
}
