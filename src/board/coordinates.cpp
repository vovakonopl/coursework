#include "board/coordinate.h"

Coord::Coord() {
    this->row = this->col = -1;
}

Coord::Coord(int row, int col) {
    this->row = row;
    this->col = col;
}

