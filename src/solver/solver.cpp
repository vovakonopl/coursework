#include "solver/solver.h"
#include "logger/logger.h"
#include "solver/utils.h"
#include <iostream>

bool solve(Board &board, Region *p_region, Cell &cell);

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

    Region region = board.create_region(fixed_cell.get_value());
    fixed_cell.region_id = region.get_id();
    region.push(fixed_cell.get_coord());

    return solve(board, &region, fixed_cell);
}

long unsigned int count = 0; 

bool solve(Board &board, Region *p_region, Cell &cell) {
    if (count++ % 1000 == 0) {
        log_board(board); 
        log_board_regions(board); 
    }
    if (board.filled_cells_count == board.get_rows() * board.get_cols()) {
        std::cout << "Exit1\n";
        return true;
    }

    // for each direction
    for (int i = 0; i < 4; i++) {
        Direction dir = static_cast<Direction>(i);
        Coord adj_cell_coord = get_free_adj_cell(board, cell, dir);
        if (adj_cell_coord.row == -1) continue;
        
        board.filled_cells_count++;
        Cell &adj_cell = board.cell_at(adj_cell_coord);
        adj_cell.region_id = cell.region_id;
        p_region->push(adj_cell_coord);
        if (!adj_cell.get_is_fixed()) {
            adj_cell.set_value(cell.get_value());
        }

        // if region completed, then go to the next unfilled fixed cell or start filling remaining cells
        if (p_region->get_size() == p_region->get_target_size()) {
            // if completed incorrectly - undo and continue
            if (!is_region_valid(board, p_region)) {
                undo_cell(board, p_region, adj_cell);
                continue;
            }
            
            int next_fixed_cell_idx = get_next_unfilled_fixed_cell_idx(board);
            // go to next unfilled fixed cell
            if (next_fixed_cell_idx != -1) {
                Coord next_cell_coord = board.fixed_cell_coords.at(next_fixed_cell_idx);
                Cell &next_cell = board.cell_at(next_cell_coord);

                board.filled_cells_count++;
                Region next_region = board.create_region(next_cell.get_value());
                next_cell.region_id = next_region.get_id();
                next_region.push(next_cell.get_coord());
                
                if(!solve(board, &next_region, next_cell)) {
                    // remove created region
                    board.filled_cells_count--;
                    next_cell.region_id = -1;

                    return false;
                }
                return true;
            }

            // All fixed cells were completed. Fill remaining cells
            return fill_remaining_cells(board);       
        } else if (solve(board, p_region, adj_cell)) {
            return true; // solved
        } 
        
        // try solve for cells in region WHICH HAVE at least 1 possible direction
        for (int i = 0; i < p_region->get_size(); i++) {
            Coord filled_cell_coord = p_region->coord_at(i); // PROBLEM IS HERE
            Cell &filled_cell = board.cell_at(filled_cell_coord);
            if (!has_any_free_adj_cell(board, filled_cell)) continue;

            // solved
            if (solve(board, p_region, filled_cell)) {
                std::cout << "returned true 2\n";
                return true;
            }
        }

        // undo changes
        undo_cell(board, p_region, adj_cell);
    }

    return false;
}

// NOTE: Not sure about function that fills all remaining cells because of check for adjacent regions. Additional checks may be required
