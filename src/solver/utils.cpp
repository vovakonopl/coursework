#include "solver/utils.h"
#include "solver/coord_queue.h"

bool is_coord_valid(Board &board, Coord coord) {
    return 
        coord.row >= 0 && coord.row < board.get_rows() &&
        coord.col >= 0 && coord.col < board.get_cols();
}

Coord get_free_adj_cell(Board &board, Cell &cell, Direction dir) {
    Coord cell_coord = cell.get_coord();
    int row = cell_coord.row;
    int col = cell_coord.col;

    switch (dir) {
        case Direction::Up:
            row += 1;
            break;

        case Direction::Right:
            col += 1;
            break;
        
        case Direction::Down:
            row -= 1;
            break;

        case Direction::Left:
            col -= 1;
            break;
    }

    if (!is_coord_valid(board, Coord(row, col))) {
        return Coord(-1, -1);
    }

    Coord coord = Coord(row, col);
    Cell adj_cell = board.cell_at(coord);
    // if cell already owned
    if (adj_cell.region_id != -1) {
        return Coord(-1, -1);
    }

    // if unowned fixed cell but it is of different value
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

// helper function
Coord find_first_unfilled(Board &board) {
    for (int row = 0; row < board.get_rows(); row++) {
        for (int col = 0; col < board.get_cols(); col++) {
            Coord coord(row, col);
            if (board.cell_at(coord).region_id == -1) return coord;    
        }
    }

    return Coord(-1, -1);
}

// helper function
bool is_valid_single_cell(Board &board, Coord coord) {
    for (int dir_y = -1; dir_y <= 1; dir_y++) {
        for (int dir_x = -1; dir_x <= 1; dir_x++) {
            if (dir_x == 0 && dir_y == 0) continue;
            
            int row = coord.row + dir_y;
            int col = coord.col + dir_x;

            // while coord in range
            while (is_coord_valid(board, Coord(row, col))) {
                if (board.cell_at(row, col).get_value() == 1) {
                    return false;
                }

                row += dir_y;
                col += dir_x;
            }
        }
    }

    return true;
}

// helper function. Resets changes made in fill_remaining_cells() 
void reset_last_region(Board &board, Region &region) {
    for (int i = 0; i < region.get_size(); i++) {
        Coord coord = region.coord_at(i);
        Cell &cell = board.cell_at(coord);
        
        cell.set_value(0);
        cell.region_id = -1;
        board.filled_cells_count--;
    }

    board.pop_region();
}

bool fill_remaining_cells(Board &board) {
    if (board.filled_cells_count == board.get_cols() * board.get_rows()) {
        return true;
    }

    Coord first_unfilled_coord = find_first_unfilled(board);
    if (first_unfilled_coord.row == -1) return true; // must be impossible because of first check, but just extra care

    CoordQueue queue;
    queue.enqueue(first_unfilled_coord);

    int reg_idx = board.create_region(-1);
    Region &region = board.region_at(reg_idx);

    while (queue.get_size() > 0) {
        Coord cell_coord = queue.dequeue();
        Cell &cell = board.cell_at(cell_coord);
      
        // mark cell
        cell.region_id = region.get_id();
        region.push(cell_coord);
        board.filled_cells_count++;

        // for each direction
        for (int i = 0; i < 4; i++) {
            Direction dir = static_cast<Direction>(i);
            Coord adj_cell_coord = get_free_adj_cell(board, cell, dir);
            if (adj_cell_coord.row == -1) continue;

            queue.enqueue(adj_cell_coord);
        }
    }

    // set region's size as value for each member cell
    for (int i = 0; i < region.get_size(); i++) {
        Coord coord = region.coord_at(i);
        Cell &cell = board.cell_at(coord);
        cell.set_value(region.get_size());
    }
    
    // if it is a single cell, then validate it
    if (region.get_size() == 1 && !is_valid_single_cell(board, region.coord_at(0))) {
        // Undo all changes and come back to solve()
        reset_last_region(board, region);
        return false;
    }

    if (fill_remaining_cells(board)) return true;

    // if failed to fill remaining cells, then undo changes
    reset_last_region(board, region);
    return false;
}

// helper function
bool is_valid_adjacency(Board &board, Cell &cell) {
    for (int dir_y = -1; dir_y <= 1; dir_y++) {
        if (dir_y == 0) continue;
        for (int dir_x = -1; dir_x <= 1; dir_x++) {
            if (dir_x == 0) continue;

            Coord coord(cell.get_coord().row + dir_y, cell.get_coord().col + dir_x);
            if (!is_coord_valid(board, coord)) continue;

            Cell &adj_cell = board.cell_at(coord);
            if (
                adj_cell.region_id != -1 && // not empty
                cell.region_id != adj_cell.region_id && // not from same region
                cell.get_value() == adj_cell.get_value() // same region sizes
            ) {
                return false;
            }
        }
    }
    
    return true;
}

bool is_region_valid(Board &board, Region &region) {
    for (int i = 0; i < region.get_size(); i++) {
        Coord coord = region.coord_at(i);
        Cell &cell = board.cell_at(coord);

        if (!is_valid_adjacency(board, cell)) return false;
    }

    return true;
}
