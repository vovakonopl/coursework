#include "solver/solver.h"
#include "solver/utils.h"
#include <iostream>

bool solve(Board &board, Cell &cell);

bool solve(Board &board) {
    if (board.filled_cells_count == board.get_rows() * board.get_cols()) {
        return true;
    }
    
    board.filled_cells_count++;
    int fixed_cell_idx = get_next_unfilled_fixed_cell_idx(board);
    if (fixed_cell_idx == -1) {
        return fill_remaining_cells(board);
    }     
    
    Coord coord = board.fixed_cell_coords.at(fixed_cell_idx);
    Cell &fixed_cell = board.cell_at(coord);

    fixed_cell.region_id = board.create_region(fixed_cell.get_value());
    board.region_at(fixed_cell.region_id).push(fixed_cell.get_coord()); 

    return solve(board, fixed_cell);
}

bool solve(Board &board, Cell &cell) {
    if (board.filled_cells_count == board.get_rows() * board.get_cols()) {
        std::cout << "Exit1\n";
        return true;
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
                undo_cell(board, region, adj_cell);
                continue;
            }
            
            int next_fixed_cell_idx = get_next_unfilled_fixed_cell_idx(board);
            // go to next unfilled fixed cell
            if (next_fixed_cell_idx != -1) {
                Coord next_cell_coord = board.fixed_cell_coords.at(next_fixed_cell_idx);
                Cell &next_cell = board.cell_at(next_cell_coord);

                board.filled_cells_count++;
                next_cell.region_id = board.create_region(next_cell.get_value());
                board.region_at(next_cell.region_id).push(next_cell.get_coord()); 
                
                if(!solve(board, next_cell)) {
                    // remove created region
                    board.filled_cells_count--;
                    next_cell.region_id = -1;
                    board.pop_region();

                    return false;
                }
                return true;
            }

            // All fixed cells were completed. Fill remaining cells
            return fill_remaining_cells(board);       
        } else if (solve(board, adj_cell)) {
            return true; // solved
        } 
        
        // try solve for cells in region WHICH HAVE at least 1 possible direction
        for (int i = 0; i < region.get_size(); i++) {
            Coord filled_cell_coord = region.coord_at(i); // PROBLEM IS HERE
            Cell &filled_cell = board.cell_at(filled_cell_coord);
            if (!has_any_free_adj_cell(board, filled_cell)) continue;

            // solved
            if (solve(board, filled_cell)) {
                std::cout << "returned true 2\n";
                return true;
            }
        }

        // undo changes
        undo_cell(board, region, adj_cell);
    }

    return false;
}

// NOTE: Not sure about function that fills all remaining cells because of check for adjacent regions. Additional checks may be required
