#include "solver/utils.h"
#include "solver/coord_queue.h"
#include <iostream>
using namespace std;

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
void reset_region(Board &board, Region *p_region) {
    for (int i = 0; i < p_region->get_size(); i++) {
        Coord coord = p_region->coord_at(i);
        Cell &cell = board.cell_at(coord);
        
        cell.set_value(0);
        cell.region_id = -1;
    }
}

bool fill_remaining_cells(Board &board) {
    cout << "fill_remaining_cells!!!\n";
    return true; // Testing for now

    Coord first_unfilled_coord = find_first_unfilled(board);
    if (first_unfilled_coord.row == -1) return true;

    Region region = board.create_region(-1);;
    CoordQueue queue;
    queue.enqueue(first_unfilled_coord);
    while (queue.get_size() > 0) {
        Coord cell_coord = queue.dequeue();
        Cell &cell = board.cell_at(cell_coord);
      
        // mark cell
        cell.region_id = region.get_id();
        region.push(cell_coord);

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
        reset_region(board, &region);
        return false;
    }

    if (fill_remaining_cells(board)) return true;

    // if failed to fill remaining cells, then undo changes
    reset_region(board, &region);
    return false;
}

// helper function
bool is_valid_adjacency(Board &board, Cell &cell) {
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
        if (adj_cell.get_value() == cell.get_value() && adj_cell.region_id != cell.region_id) {
            return false;
        }
    }

    return true;
}

bool is_region_valid(Board &board, Region *p_region) {
    if (p_region->get_size() != p_region->get_target_size()) {
        return false;
    } 

    for (int i = 0; i < p_region->get_size(); i++) {
        Coord coord = p_region->coord_at(i);
        Cell &cell = board.cell_at(coord);

        if (!is_valid_adjacency(board, cell)) return false;
    }

    return true;
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

void get_adjs_with_same_val(Board &board, Cell &cell, vector<Coord> &vec) {
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
        if (adj_cell.region_id == -1) continue; 
        if(
            adj_cell.get_value() == p_region->get_target_size() &&
            adj_cell.region_id != p_region->get_id()
        ) {
            return false;
        }
    }

    return true;
}
