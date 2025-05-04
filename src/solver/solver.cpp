#include "solver/solver.h"
#include "solver/utils.h"

bool solve(Board &board, Cell &cell);

bool solve(Board &board) {
    if (board.filled_cells_count == board.get_rows() * board.get_cols()) {
        return true;
    }

    // empty board
    if (board.fixed_cell_coords.size() == 0) {
        return solve(board, board.cell_at(0, 0));
    }
 
    Coord initial_coord = board.fixed_cell_coords.front();
    Cell &initial_cell = board.cell_at(initial_coord);
    return solve(board, initial_cell);
}

bool solve(Board &board, Cell &cell) {
    if (board.filled_cells_count == board.get_rows() * board.get_cols()) {
        return true;
    }

    // if we start working with new region
    bool is_new_region = false;
    if (cell.region_id == -1) {
        cell.region_id = board.create_region(cell.get_value()); // WARNING: doesn't work if we are filling remaining cells (value will be 0, not -1)
        is_new_region = true;
    }
    Region &region = board.region_at(cell.region_id);
    
    // for each direction
    for (int i = 0; i < 4; i++) {
        Direction dir = static_cast<Direction>(i);
        Coord adj_cell_coord = get_free_adj_cell(board, cell, dir);
        if (adj_cell_coord.row == -1) continue;
        
        board.filled_cells_count++;
        Cell &adj_cell = board.cell_at(adj_cell_coord);
        adj_cell.region_id = cell.region_id;
        region.push(adj_cell_coord);
        if (!adj_cell.get_is_fixed()) {
            adj_cell.set_value(cell.get_value());
        }

        // if region completed, then go to the next unfilled fixed cell or start filling remaining cells
        if (region.get_size() == region.get_target_size()) {
            // if completed incorrectly - undo and continue
            if (!is_region_valid(board, region)) {
                board.filled_cells_count--;
                adj_cell.region_id = -1;
                region.pop();
                if (!adj_cell.get_is_fixed()) {
                    adj_cell.set_value(0);
                }
         
                continue;
            }

            int next_fixed_cell_idx = get_next_unfilled_fixed_cell_idx(board);
            if (next_fixed_cell_idx != -1) {
                Coord next_cell_coord = board.fixed_cell_coords.at(next_fixed_cell_idx);
                Cell &next_cell = board.cell_at(next_cell_coord);
                
                return solve(board, next_cell);
            }

            // All fixed cells were completed. Fill remaining cells
            return fill_remaining_cells(board);       
        } else if (solve(board, adj_cell)) return true; // solved
        
        // try solve for cells in region WHICH HAVE at least 1 possible direction
        for (int i = 0; i < region.get_size(); i++) {
            Coord filld_cell_coord = region.coord_at(i);
            Cell &filled_cell = board.cell_at(filld_cell_coord);
            if (!has_any_free_adj_cell(board, filled_cell)) continue;

            // solved
            if (solve(board, filled_cell)) return true;
        }

        // undo changes
        board.filled_cells_count--;
        adj_cell.region_id = -1;
        region.pop();
        if (!adj_cell.get_is_fixed()) {
            adj_cell.set_value(0);
        }
    }

    // undo chages and go back
    if (is_new_region) {
        cell.region_id = -1;
        board.pop_region();
    }

    return false;
}

// WARNING: there is no check for adjacent regions of same size after filling remaining cells.
// NOTE: Not sure about function that fills all remaining cells because of check for adjacent regions. Additional checks may be required
