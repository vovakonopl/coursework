#include "board/board.h"
#include <stdexcept>

Board::Board(int rows, int cols) {
    if (rows <= 0 || cols <= 0) {
        throw std::invalid_argument("Board dimensions must be greater than 0");
    }

    this->rows = rows;
    this->cols = cols;
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

Cell &Board::cell_at(int x, int y) {
    if (x < 0 || x >= this->cols || y < 0 || y >= this->rows) {
        throw std::out_of_range("Index out of range!");
    }

    return this->board[y][x];
}

int Board::get_rows() {
    return rows;
}

int Board::get_cols() {
    return cols;
}
