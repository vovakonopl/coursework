#include "board/cell.h"

Cell::Cell() {
    this->value = 0;
    this->is_fixed = false;
    this->region_id = -1;
} 

// TODO: remove these constructors
Cell::Cell(int value) {
    this->value = value;
    this->is_fixed = false;
    this->region_id = -1;
}

Cell::Cell(int value, bool is_fixed) {
    this->value = value;
    this->is_fixed = is_fixed;
    this->region_id = -1;
}
// -------------------------------

Cell::Cell(int row, int col, int value, bool is_fixed) {
    this->coord.row = row;
    this->coord.col = col;
    this->value = value;
    this->is_fixed = is_fixed;
    this->region_id = -1;
}

int Cell::get_value() {
    return this->value;
} 

void Cell::set_value(int value) {
    if (this->is_fixed) return;
    this->value = value;
}

bool Cell::get_is_fixed() {
    return this->is_fixed;
}

Coord Cell::get_coord() {
    return coord;
}

std::ostream &operator <<(std::ostream &stream, Cell &cell) {
    stream << "Cell: { " << cell.get_coord().row <<  ", " << cell.get_coord().col
        << " }; val = " << cell.get_value() << "; fixed = " << cell.get_is_fixed()
        << "; reg id = " << cell.region_id << std::endl;
    return stream;
}
