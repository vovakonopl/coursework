#include "solver/manual_solving.h"
#include "board/board.h"
#include "board/cell.h"
#include "solver/utils.h"
#include "solver/coord_queue.h"
#include <iostream>

Board *copy_board(Board &board) {
    Board *p_board = new Board(board.get_rows(), board.get_cols());

    for (int row = 0; row < board.get_rows(); row++) {
        for (int col = 0; col < board.get_cols(); col++) {
            Cell cell = board.cell_at(row, col);
            p_board->cell_at(row, col) = cell;
        }
    }

    p_board->create_fixed_cells_list();
    return p_board;
}

bool validate_single_cells(Board *p_board) {
    for (int row = 0; row < p_board->get_rows(); row++) {
        for (int col = 0; col < p_board->get_cols(); col++) {
            Cell &cell = p_board->cell_at(row, col);
            if (cell.get_value() != 1) continue;

            p_board->result.push_back(Coord(row, col));
         
            Coord adj_coords[4];
            adj_coords[0] = Coord(row - 1, col); // top
            adj_coords[1] = Coord(row + 1, col); // bottom
            adj_coords[2] = Coord(row, col - 1); // left
            adj_coords[3] = Coord(row, col + 1); // right
   
            for (int i = 0; i < 4; i++) {
                if (!is_coord_valid(*p_board, adj_coords[i])) continue;
         
                Cell &adj_cell = p_board->cell_at(adj_coords[i]);
                if (adj_cell.get_value() == 1) {
                    return false;
                } 
            }
        }
    }

    return true;
}

bool enqueue_adj_cells(Board *p_board, CoordQueue &queue, Cell &cell) {
    int row = cell.get_coord().row;
    int col = cell.get_coord().col;
    Coord adj_coords[4];
    adj_coords[0] = Coord(row - 1, col); // top
    adj_coords[1] = Coord(row + 1, col); // bottom
    adj_coords[2] = Coord(row, col - 1); // left
    adj_coords[3] = Coord(row, col + 1); // right
    
    for (int i = 0; i < 4; i++) {
        if (!is_coord_valid(*p_board, adj_coords[i])) continue;

        Cell &adj_cell = p_board->cell_at(adj_coords[i]);
        if (adj_cell.get_value() != cell.get_value()) continue;

        if (adj_cell.region_id != -1 && adj_cell.region_id != cell.region_id) {
            return false; // invalid region
        }

        if (adj_cell.region_id == -1) {
            queue.enqueue(adj_cell.get_coord());
        }
    }

    return true;
}

// validate board by filling it with BFS
bool validate_regions(Board *p_board) {
    int next_cell_idx = get_next_unfilled_fixed_cell_idx(*p_board);
    if (next_cell_idx == -1) return true; // all regions filled correctly

    Coord next_cell_coord = p_board->fixed_cell_coords.at(next_cell_idx);
    Cell &cell = p_board->cell_at(next_cell_coord);

    CoordQueue queue;
    queue.enqueue(cell.get_coord());
    Region region = p_board->create_region(cell.get_value());

    while (queue.get_size() > 0) {
        Coord coord = queue.dequeue();
        Cell &cell = p_board->cell_at(coord);
        if (cell.region_id != -1) continue; // added second time from other cells
       
        cell.region_id = region.get_id();
        region.push(coord);
        p_board->result.push_back(coord);

        if (!enqueue_adj_cells(p_board, queue, cell)) {
            return false;
        } 
    }

    // overflow or underflow
    if (region.get_size() != region.get_target_size()) {
        return false;
    }

    return validate_regions(p_board);
}

bool is_correctly_solved(Board &board) {
    Board *p_board_copy = copy_board(board);
    if (!validate_regions(p_board_copy)) {
        delete p_board_copy;
        return false;
    }

    // validate all single cells
    bool is_valid_board = validate_single_cells(p_board_copy);
    if (is_valid_board) {
        board.result = p_board_copy->result;
    }

    delete p_board_copy;
    return is_valid_board;
}

