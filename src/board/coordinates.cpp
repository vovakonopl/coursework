#include "board/coordinate.h"
#include <iostream>

Coord::Coord() {
    this->row = this->col = -1;
}

Coord::Coord(int row, int col) {
    this->row = row;
    this->col = col;
}

bool Coord::operator ==(Coord coord) {
    return row == coord.row && col == coord.col;
}

std::ostream &operator <<(std::ostream &stream, Coord coord) {
    stream << "Coord: { " << coord.row <<  ", " << coord.col << " }\n";
    return stream;
}
