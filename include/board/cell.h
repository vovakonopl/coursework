#ifndef CELL_H
#define CELL_H

#include "board/coordinate.h"
#include <ostream>

class Cell {
    int value;
    bool is_fixed;
    Coord coord;

public:
    int region_id;
 
    Cell();
    Cell(int row, int col, int value, bool is_fixed = true);

    // getters/setters
    int get_value();
    void set_value(int value);
    bool get_is_fixed();
    Coord get_coord();

    // for debugging
    friend std::ostream &operator <<(std::ostream &stream, Cell &cell);
};

#endif
