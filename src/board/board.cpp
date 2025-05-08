#include "board/board.h"
#include "solver/utils.h"
#include <iostream>
#include <stdexcept>

Board::Board(int rows, int cols) {
    if (rows <= 0 || cols <= 0) {
        throw std::invalid_argument("Board dimensions must be greater than 0");
    }

    this->rows = rows;
    this->cols = cols;
    this->region_count = 0;
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
    if (row < 0 || row >= rows || col < 0 || col >= cols) {
        std::cerr << "create_cell(4): index out of range!";
        throw std::out_of_range("Index out of range!");
    }
    
    Cell &cell = this->cell_at(row, col);
    cell = Cell(row, col, value, is_fixed);
}

Cell &Board::cell_at(int row, int col) {
    if (row < 0 || row >= rows || col < 0 || col >= cols) {
        std::cerr << "cell_at(int, int): index out of range!";
        throw std::out_of_range("Index out of range!");
    }

    return this->board[row][col];
}

Cell &Board::cell_at(Coord coord) {
    return this->cell_at(coord.row, coord.col);
}

Region Board::create_region(int target_size) {
    Region region(region_count, target_size);
    region_count++;

    return region;
}

void Board::create_fixed_cells_list() {
    fixed_cell_coords.clear();

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            Cell &cell = cell_at(row, col);
            if (cell.get_is_fixed()) {
                // single cells are autocompleted from start
                if (cell.get_value() == 1) {
                    cell.region_id = -2; // any id is suitable for single cells. -1 stays for unfilled cells
                    continue;
                }

                fixed_cell_coords.push_back(Coord(row, col));
            }
        }
    }
}

void Board::fill_values_on_board() {
    values_on_board.clear();
   
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            Cell &cell = cell_at(row, col);
            if (cell.get_value() == 0) continue;
            if (find_idx(values_on_board, cell.get_value()) == -1) {
                values_on_board.push_back(cell.get_value());   
            }
        }
    }
}

