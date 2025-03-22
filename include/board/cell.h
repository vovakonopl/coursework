#ifndef CELL_H
#define CELL_H

class Cell {
    int value;
    bool is_fixed;

public:
    int region_id;
 
    Cell();
    Cell(int value);
    Cell(int value, bool is_fixed);

    // getters/setters
    int get_value();
    void set_value(int value);
    bool get_is_fixed();
};

#endif
