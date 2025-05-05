#include "board/board.h"
#include <iostream>
#include <stdexcept>

Board::Board(int rows, int cols) {
    if (rows <= 0 || cols <= 0) {
        throw std::invalid_argument("Board dimensions must be greater than 0");
    }

    this->rows = rows;
    this->cols = cols;
    this->filled_cells_count = 0;
    this->board = new Cell*[rows];
    for (int i = 0; i < rows; i++) {
        this->board[i] = new Cell[cols];
    }

    // fill coordinates
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            this->create_cell(row, col, 0);
        }
    }
}

Board::~Board() {
    for (int i = 0; i < this->rows; i++) {
        delete[] this->board[i];
    }

    delete[] this->board;
}

int Board::get_rows() {
    return rows;
}

int Board::get_cols() {
    return cols;
}

void Board::create_cell(int row, int col, int value, bool is_fixed) {
    if (row < 0 || row >= this->cols || col < 0 || col >= this->rows) {
        throw std::out_of_range("Index out of range!");
    }
    
    Cell &cell = this->cell_at(row, col);
    cell = Cell(row, col, value, is_fixed);
}

Cell &Board::cell_at(int row, int col) {
    if (row < 0 || row >= this->cols || col < 0 || col >= this->rows) {
        std::cout << "Error!!!! Coord: { " << row << ", " << col << " }\n";
        throw std::out_of_range("Index out of range!");
    }

    return this->board[row][col];
}

Cell &Board::cell_at(Coord coord) {
    return this->cell_at(coord.row, coord.col);
}

int Board::create_region(int target_size) {
    int idx = static_cast<int>(regions.size());
    regions.push_back(Region(idx, target_size));
    return idx;
}

void Board::pop_region() {
    if (regions.empty()) return;

    /*
    // undo all cells inside
    Region &region = regions.back();
    for (int i = 0; i < region.get_size(); i++) {
        Cell &cell = cell_at(region.coord_at(i));
        if (cell.region_id != -1) {
            filled_cells_count--;
            cell.region_id = -1;
            region.pop();
            if (!cell.get_is_fixed()) {
                cell.set_value(0);
            }
        }
    }
    */

    regions.pop_back();    
}

Region &Board::region_at(long unsigned int idx) {
    if (idx >= regions.size()) {
        throw std::out_of_range("Index out of range!");
    }

    return regions.at(idx);
}

int Board::get_regions_count() {
    return static_cast<int>(regions.size());
}

void Board::create_fixed_cells_list() {
    fixed_cell_coords.clear();
    filled_cells_count = 0;

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            Cell &cell = cell_at(row, col);
            if (cell.get_is_fixed()) {
                fixed_cell_coords.push_back(Coord(row, col));
                
                // single cells are autocompleted from start
                if (cell.get_value() == 1) {
                    cell.region_id = -2; // any id is suitable for single cells. -1 stays for unfilled cells
                    filled_cells_count++;
                } 
            }
        }
    }
}
