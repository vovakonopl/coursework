#include "board/cell.h"

Cell::Cell() {
    this->value = 0;
    this->is_fixed = false;
    this->region_id = -1;
} 

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
