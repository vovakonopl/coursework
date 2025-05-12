#include "solver/solver.h"
#include "solver/utils.h"
#include <vector>

bool solve(Board &board) {
    int idx = get_next_unfilled_fixed_cell_idx(board);
    if (idx == -1) {
        fill_empty_board(board);
        return true; 
    }

    Coord fixed_cell_coord = board.fixed_cell_coords.at(idx);
    Cell &fixed_cell = board.cell_at(fixed_cell_coord);
    Region region = board.create_region(fixed_cell.get_value());

    fixed_cell.region_id = region.get_id();
    region.push(fixed_cell_coord);

    bool result = solve(board, &region, fixed_cell);
    if (result) {
        // Results arrive in the end-to-start order. We need to reverse them
        reverse_vector(board.result);
    }

    return result;
}

bool solve(Board &board, Region *p_region, Cell &cell) {
    std::vector<Coord> adjs_with_same_val;
    get_adjs_with_same_val(board, cell, adjs_with_same_val);
    int adj_coords_num = static_cast<int>(adjs_with_same_val.size());
    
    // these cells don't fit into the region, but can't stay adjacent to it, go back
    if (p_region->get_size() + adj_coords_num > p_region->get_target_size()) {
        return false;
    }

    // add every cell to region if there is any
    for (int i = 0; i < adj_coords_num; i++) {
        Coord adj_coord = adjs_with_same_val.at(i);
        Cell &adj_cell = board.cell_at(adj_coord);
        
        // adJ_cell.region_id has to be only -1 
        adj_cell.region_id = p_region->get_id();
        p_region->push(adj_coord);
    }

    // region completed
    if (p_region->get_size() == p_region->get_target_size()) {
        int idx = get_next_unfilled_fixed_cell_idx(board);
        if (idx == -1) {
            board.result.push_back(cell.get_coord());
            return true;
        }
     
        Coord next_cell_coord = board.fixed_cell_coords.at(idx);
        Cell &next_cell = board.cell_at(next_cell_coord);
        Region region = board.create_region(next_cell.get_value());
        
        next_cell.region_id = region.get_id();
        region.push(next_cell_coord);
        
        if (!solve(board, &region, next_cell)) {
            // undo for next_cell
            next_cell.region_id = -1;

            // undo every adjacent cell that we filled at the start
            for (int i = 0; i < adj_coords_num; i++) {
                Coord adj_coord = adjs_with_same_val.at(i);
                Cell &adj_cell = board.cell_at(adj_coord);
                undo_cell(board, p_region, adj_cell);
            }

            return false;
        } 
        
        board.result.push_back(cell.get_coord());
        return true;
    }


    if (solve_for_each_adjacent(board, p_region, cell)) {
        board.result.push_back(cell.get_coord());
        return true;
    }

    // try for every cell in current region
    for (int i = 0; i < p_region->get_size(); i++) {
        Coord coord = p_region->coord_at(i);
        if (coord == cell.get_coord()) continue;

        Cell &cell_in_reg = board.cell_at(coord);
        if (!has_any_free_adj_cell(board, cell_in_reg)) continue;
 
        if (solve_for_each_adjacent(board, p_region, cell_in_reg)) {
            board.result.push_back(cell.get_coord());
            return true;
        }
    }

    // undo every adjacent cell that we filled at the start
    for (int i = 0; i < adj_coords_num; i++) {
        Coord adj_coord = adjs_with_same_val.at(i);
        Cell &adj_cell = board.cell_at(adj_coord);
        undo_cell(board, p_region, adj_cell);
    }

    return false;
}
