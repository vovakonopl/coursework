#include "board/board.h"
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
        throw std::out_of_range("Index out of range!");
    }

    return this->board[row][col];
}

Cell &Board::cell_at(Coord coord) {
    return this->cell_at(coord.row, coord.col);
}

int Board::create_region(int target_size) {
    int idx = regions.size();
    regions.push_back(Region(idx, target_size));
    return idx;
}

void Board::pop_region() {
    regions.pop_back();    
}

Region &Board::region_at(long unsigned int idx) {
    if (idx >= regions.size()) {
        throw std::out_of_range("Index out of range!");
    }

    return regions.at(idx);
}

void Board::create_fixed_cells_list() {
    fixed_cell_coords.clear();

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            if (board[row][col].get_is_fixed()) {
                fixed_cell_coords.push_back(Coord(row, col));
            }
        }
    }
}
